#include <msieve.h>
#include <ruby.h>

#if !defined(RSTRING_LEN)
#  define RSTRING_LEN(x) (RSTRING(x)->len)
#  define RSTRING_PTR(x) (RSTRING(x)->ptr)
#endif

#define msieve_get_struct(ruby_var,c_var) { Data_Get_Struct(ruby_var, msieve_obj, c_var); }
#define msieve_make_struct(ruby_var,c_var) { ruby_var = Data_Make_Struct(cMsieve, msieve_obj, 0, 0, c_var); }
#define BIGNUM_P(value) (TYPE(value) == T_BIGNUM)
#define HASH_P(value) (TYPE(value) == T_HASH)
#define NUMERIC_P(value) (FIXNUM_P(value) || BIGNUM_P(value))
#define STRING_P(value) (TYPE(value) == T_STRING)
#define MSIEVE_P(value) (rb_obj_is_instance_of(value, cMsieve) == Qtrue)
#define rb_symbol(value) (rb_funcall(rb_str_new2(value), rb_intern("to_sym"), 0))

#define DEFUN_0ARG_UINT32(fname)             \
static VALUE r_msieve_##fname(VALUE self)    \
{                                            \
  msieve_obj *self_val;                      \
  msieve_get_struct (self, self_val);        \
                                             \
  uint32 attr_val = self_val->fname;         \
  return INT2FIX(attr_val);                  \
}

#define DEFUN_0ARG_UINT64(fname)             \
static VALUE r_msieve_##fname(VALUE self)    \
{                                            \
  msieve_obj *self_val;                      \
  msieve_get_struct (self, self_val);        \
                                             \
  uint64 attr_val = self_val->fname;         \
  return INT2FIX(attr_val);                  \
}

#define DEFUN_0ARG_CHARSTAR(fname)           \
static VALUE r_msieve_##fname(VALUE self)    \
{                                            \
  msieve_obj *self_val;                      \
  msieve_get_struct (self, self_val);        \
                                             \
  char *attr_val = self_val->fname;          \
  return rb_str_new2(attr_val);              \
}

VALUE cMsieve;
VALUE cMsieve_Factor;

void msieve_set_factors(VALUE obj);

/*
 * Swiped from msieve's demo for now. Will push to Ruby (and maybe GMP) later.
 */
void get_random_seeds(uint32 *seed1, uint32 *seed2) {

	uint32 tmp_seed1, tmp_seed2;

	/* In a multithreaded program, every msieve object
	   should have two unique, non-correlated seeds
	   chosen for it */

#if !defined(WIN32) && !defined(_WIN64)

	FILE *rand_device = fopen("/dev/urandom", "r");

	if (rand_device != NULL) {

		/* Yay! Cryptographic-quality nondeterministic randomness! */

		fread(&tmp_seed1, sizeof(uint32), (size_t)1, rand_device);
		fread(&tmp_seed2, sizeof(uint32), (size_t)1, rand_device);
		fclose(rand_device);
	}
	else

#endif
	{
		/* <Shrug> For everyone else, sample the current time,
		   the high-res timer (hopefully not correlated to the
		   current time), and the process ID. Multithreaded
		   applications should fold in the thread ID too */

		uint64 high_res_time = read_clock();
		tmp_seed1 = ((uint32)(high_res_time >> 32) ^
			     (uint32)time(NULL)) * 
			    (uint32)getpid();
		tmp_seed2 = (uint32)high_res_time;
	}

	/* The final seeds are the result of a multiplicative
	   hash of the initial seeds */

	(*seed1) = tmp_seed1 * ((uint32)40499 * 65543);
	(*seed2) = tmp_seed2 * ((uint32)40499 * 65543);
}

void msieve_free(void *p) {
  msieve_obj_free(p);
}

VALUE msieve_alloc(VALUE klass) {
  msieve_obj *msieve_val;
  VALUE obj;
  
  char *integer_val = "0";
  uint32 default_flags = MSIEVE_FLAG_USE_LOGFILE;
  char *savefile_name = NULL;
  char *logfile_name = NULL;
  char *nfs_fbfile_name = NULL;
  uint32 seed1, seed2;
  get_random_seeds(&seed1, &seed2);
  uint32 max_relations = 0;
  uint64 nfs_lower = 0;
  uint64 nfs_upper = 0;
  enum cpu_type cpu = get_cpu_type();
  uint32 cache_size1;
  uint32 cache_size2;
  get_cache_sizes(&cache_size1, &cache_size2);
  uint32 num_threads = 0;
  uint32 mem_mb = 0;
  uint32 which_gpu = 0;
  
  msieve_val = msieve_obj_new(integer_val, default_flags,
      savefile_name, logfile_name, nfs_fbfile_name,
      seed1, seed2, max_relations,
      nfs_lower, nfs_upper, cpu,
      cache_size1, cache_size2,
      num_threads, mem_mb, which_gpu);
  obj = Data_Wrap_Struct(klass, 0, msieve_free, msieve_val);
  return obj;
}

// /*
 // * call-seq:
 // *   Msieve.new(hash)
 // *
 // * Creates a new Msieve object (state), with certain arguments and variables
 // * set with <i>hash</i>.
 // */
// VALUE r_msievesg_new(int argc, VALUE *argv, VALUE klass)
// {
  // msieve_obj *res_val;
  // VALUE res;
  // (void)klass;

  // if (argc > 2)
    // rb_raise(rb_eArgError, "wrong # of arguments(%d for 0, 1, or 2)", argc);

  // msieve_make_struct (res, res_val);
  // rb_obj_call_init (res, argc, argv);

  // return res;
// }

VALUE r_msieve_initialize(int argc, VALUE *argv, VALUE self)
{
  msieve_obj *self_val;
  VALUE integer, hash, hash_value, str;
  char *integer_val;

  msieve_get_struct (self, self_val);
  rb_scan_args (argc, argv, "11", &integer, &hash);
  if (NUMERIC_P(integer)) {
    VALUE integer_s = rb_funcall (integer, rb_intern("to_s"), 0);
	str = StringValue (integer_s);
  }
  else if (STRING_P(integer)) {
    str = StringValue (integer);
  }
  else {
    rb_raise (rb_eArgError, "integer must be Numeric or String");
  }
  integer_val = RSTRING_PTR(str);
  self_val->input = integer_val;
  
  if (NIL_P(hash)) { hash = rb_hash_new (); }
  if (! HASH_P(hash)) {
    rb_raise (rb_eArgError, "options must be a Hash");
  }
  
  hash_value = rb_hash_aref(hash, rb_symbol("mem_mb"));
  if (hash_value != Qnil) {
    self_val->mem_mb = FIX2INT(hash_value);
  }
  
  hash_value = rb_hash_aref(hash, rb_symbol("quiet"));
  if (hash_value != Qnil) {
    self_val->flags &= ~(MSIEVE_FLAG_USE_LOGFILE | MSIEVE_FLAG_LOG_TO_STDOUT);
  }
  
  hash_value = rb_hash_aref(hash, rb_symbol("max_relations"));
  if (hash_value != Qnil) {
    self_val->max_relations = FIX2INT(hash_value);
  }
  
  hash_value = rb_hash_aref(hash, rb_symbol("num_threads"));
  if (hash_value != Qnil) {
    self_val->num_threads = FIX2INT(hash_value);
  }
  
  return Qnil;
}

VALUE r_msieve_factor_bang(VALUE self)
{
  msieve_obj *self_val;
  msieve_get_struct (self, self_val);
  
  msieve_run(self_val);
  
  if (!(self_val->flags & MSIEVE_FLAG_FACTORIZATION_DONE)) {
    rb_raise (rb_eRuntimeError, "current factorization was interrupted");
  }
  
  msieve_set_factors(self);
  
  return rb_iv_get (self, "@factors");
}

void msieve_set_factors(VALUE obj) {
  msieve_obj *obj_val;
  msieve_get_struct (obj, obj_val);
  VALUE factors_array = rb_ary_new ();
  
  //rb_iv_set (obj, "@factors", rb_ary_new);
  
  msieve_factor *factor = obj_val->factors;
  while (factor != NULL) {
    VALUE factor_type;

    if (factor->factor_type == MSIEVE_PRIME)
      factor_type = rb_symbol("prime");
    else if (factor->factor_type == MSIEVE_COMPOSITE)
      factor_type = rb_symbol("composite");
    else
      factor_type = rb_symbol("probably_prime");
      
    VALUE factor_obj = rb_funcall (cMsieve_Factor,
                                   rb_intern("new"),
                                   3,
                                   factor_type,
                                   rb_str_new2(factor->number),
                                   INT2FIX((int32)strlen(factor->number)));
    rb_ary_push(factors_array, factor_obj);

    factor = factor->next;
  }
  rb_iv_set (obj, "@factors", factors_array);
}

VALUE r_msieve_seed1(VALUE self)
{
  msieve_obj *self_val;
  msieve_get_struct (self, self_val);
  
  long seed1_val = self_val->seed1;
  VALUE seed1 = LONG2FIX(seed1_val);
  
  return seed1;
}

DEFUN_0ARG_CHARSTAR(input)
DEFUN_0ARG_UINT32(seed2)
DEFUN_0ARG_UINT32(max_relations)
DEFUN_0ARG_UINT64(nfs_lower)
DEFUN_0ARG_UINT64(nfs_upper)
DEFUN_0ARG_UINT32(cache_size1)
DEFUN_0ARG_UINT32(cache_size2)
DEFUN_0ARG_UINT32(num_threads)
DEFUN_0ARG_UINT32(mem_mb)

VALUE r_msieve_factor_initialize(int argc, VALUE *argv, VALUE self) {
  VALUE factor_type, number, digits;
  
  rb_scan_args (argc, argv, "3", &factor_type, &number, &digits);
  rb_iv_set (self, "@factor_type", factor_type);
  rb_iv_set (self, "@number", number);
  rb_iv_set (self, "@digits", digits);
  
  return Qnil;
}

VALUE r_msieve_factor_inspect(VALUE self) {
  return rb_iv_get (self, "@number");
}

VALUE r_msieve_factor_to_i(VALUE self) {
  return rb_funcall(rb_iv_get (self, "@number"), rb_intern("to_i"), 0);
}

VALUE r_msieve_factor_to_s(VALUE self) {
  return rb_iv_get (self, "@number");
}

void Init_msieve() {
  cMsieve = rb_define_class ("Msieve", rb_cObject);
  char msieve_version_string[5];
  sprintf (msieve_version_string, "%d.%02d", MSIEVE_MAJOR_VERSION, MSIEVE_MINOR_VERSION);
  rb_define_const (cMsieve, "MSIEVE_VERSION", rb_str_new2 (msieve_version_string));
  rb_define_const (cMsieve, "VERSION",        rb_str_new2 ("0.1.0"));
  
  rb_define_alloc_func (cMsieve, msieve_alloc);
  rb_define_method (cMsieve, "initialize",    r_msieve_initialize, -1);
  rb_define_method (cMsieve, "factor!",       r_msieve_factor_bang, 0);
  rb_define_method (cMsieve, "input",         r_msieve_input, 0);
  rb_define_method (cMsieve, "seed1",         r_msieve_seed1, 0);
  rb_define_method (cMsieve, "seed2",         r_msieve_seed2, 0);
  rb_define_method (cMsieve, "max_relations", r_msieve_max_relations, 0);
  rb_define_method (cMsieve, "nfs_lower",     r_msieve_nfs_lower, 0);
  rb_define_method (cMsieve, "nfs_upper",     r_msieve_nfs_upper, 0);
  rb_define_method (cMsieve, "cache_size1",   r_msieve_cache_size1, 0);
  rb_define_method (cMsieve, "cache_size2",   r_msieve_cache_size2, 0);
  rb_define_method (cMsieve, "num_threads",   r_msieve_num_threads, 0);
  rb_define_method (cMsieve, "mem_mb",        r_msieve_mem_mb, 0);
  rb_define_attr (cMsieve, "factors", 1, 0);
  
  cMsieve_Factor = rb_define_class_under (cMsieve, "Factor", rb_cObject);
  rb_define_method (cMsieve_Factor, "initialize", r_msieve_factor_initialize, -1);
  rb_define_method (cMsieve_Factor, "inspect",    r_msieve_factor_inspect, 0);
  rb_define_method (cMsieve_Factor, "to_i",       r_msieve_factor_to_i, 0);
  rb_define_method (cMsieve_Factor, "to_s",       r_msieve_factor_to_s, 0);
  rb_define_attr (cMsieve_Factor, "factor_type", 1, 0);
  rb_define_attr (cMsieve_Factor, "number",      1, 0);
  rb_define_attr (cMsieve_Factor, "digits",      1, 0);
}
