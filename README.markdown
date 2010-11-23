msieve Ruby gem
===============

This gem (library) provides Ruby bindings for the msieve library, "A Library for Factoring Large Integers" by Jason Papadopoulos.

Usage
-----

It's entirely simple right now, and there is probably a lot to be desired. In any case, use it as follows:

    $irb -r lib/msieve --simple-prompt
    >> m = Msieve.new("1234567890")
    => #<Msieve:0x14418e0>
    >> m.factor!
    => [2, 3, 3, 5, 3607, 3803]
    >> n = Msieve.new("235711131719")
    => #<Msieve:0x14f28c0>
    >> n.factor!
    => [7, 4363, 7717859]
    >> p = Msieve.new("149162536496481")
    => #<Msieve:0x15b3140>
    >> p.factor!
    => [3, 7549, 48437, 135979]</code></pre>

You can also include elementary arithmetic as listed in msieve's readme. I quote:

> Starting with v1.08, the inputs to msieve can be integer arithmetic 
> expressions using any of the following operators:
> 
> * \+ -  plus, minus        (lowest priority)
> * %    integer remainder
> * \* /  multiply, integer divide
> * ^    power
> * ( )  grouping           (highest priority)

Examples:

    $irb -r lib\msieve --simple-prompt
    >> m = Msieve.new("(10^17-1)/9")
    => #<Msieve:0x13ea4a0>
    >> m.factor!
    => [2071723, 5363222357]
    >> m = Msieve.new("(10^19-1)/9")
    => #<Msieve:0x1467b80>
    >> m.factor!
    => [1111111111111111111]
    
Options
-------

The `Msieve.new()` method takes first a number to factor. This "number" can be a Fixnum, Bignum, or String. Second, it
takes a hash of options. These roughly correspond to the msieve demo program's cmd-line options:

* `:logfile_name =>` _`file_name`_ - Use _`file_name`_ as the log file instead of `msieve.log`.
* `:quiet => true` - A bit misleading, but it doesn't generate any log information.
* `:max_relations =>` _`number`_ - Stop sieving after finding _`number`_ relations.
* `:num_threads =>` _`number`_ - Use at most _`number`_ threads.
* `:mem_mb =>` _`mb`_ - Hint for number of megabytes of memory for postprocessing.

Caveat
------

msieve has two headers that have the line

    #include <util.h>

This is no good here, because mkmf always puts Ruby's include paths before the
ones you request. So it ends up including `ruby/util.h`. No good! I had to hack
my copy of msieve, the `msieve.h` and `mp.h` files, to change that line to

    #include "util.h"

and boom! Everything works. No word on how to... automatically do this in the
gem? I am wholly against toying with the global vars that mkmf sets up, in
order to reorganize the include paths. Don't even suggest that. Yuck.

Bugs
----

It has some bugs:

    $irb -r lib\msieve --simple-prompt
    >> m = Msieve.new(99999999)
    => #<Msieve:0x14e26c0>
    >> m.input
    => "99999999"
    >> m.factor!
    => [3, 3, 11, 73, 101, 137]
    >> m.factor!
    => [3, 3, 11, 73, 101, 137, 3, 3, 11, 73, 101, 137]

Todo
----

* Fix weird crashing bug that occurs sporadically (related to not mallocing integer_string?)
* Get all options working that msieve_obj_new() accepts
* Internal logging???
* Stop, Restart
* Integrate with Ruby gmp gem if available (passing in GMP::Z, factors coming
  out as GMP::Z, adding a GMP::Z.factor! method
* Make Ruby bindings for GMP-ECM, then integrate those, haha.
* Documentation
* CHANGELOG, etc.