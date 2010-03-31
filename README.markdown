msieve Ruby gem
===============

This gem (library) provides Ruby bindings for the msieve library, "A Library for Factoring Large Integers" by Jason Papadopoulos.

Usage
-----

It's entirely simple right now, and there is probably a lot to be desired. In any case, use it as follows:

<pre><code>$irb -r lib/msieve --simple-prompt
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

Caveat
------

msieve has two headers that have the line

#include <util.h>

This is no good here, because mkmf always puts Ruby's include paths before the
ones you request. So it ends up including ruby/util.h. No good! I had to hack
my copy of msieve, the msieve.h and mp.h files, to change that line to

#include "util.h"

and boom! Everything works. No word on how to... autotically do this in the
gem? I am wholly against toying with the global vars that mkmf sets up, in
order to reorganize the include paths. Don't even suggest that. Yuck.


Bugs
----

It has some bugs:

<pre><code>$irb -r lib\msieve --simple-prompt

>> m = Msieve.new(99999999)
=> #<Msieve:0x14e26c0>
>> m.input
=> "99999999"
>> m.factor!
=> [3, 3, 11, 73, 101, 137]
>> m.factor!
=> [3, 3, 11, 73, 101, 137, 3, 3, 11, 73, 101, 137]</code></pre>

Todo
----

* Tests
* Fix weird crashing bug that occurs sporadically
* Get all options working that msieve_obj_new() accepts
* Internal logging???
* Stop, Restart
* Integrate with Ruby gmp gem if available (passing in GMP::Z, factors coming
  out as GMP::Z, adding a GMP::Z.factor! method
* Make Ruby bindings for GMP-ECM, then integrate those, haha.
* Oh, verify that anything works on !windows.
* Documentation
* Write Jason about that caveat
* Versioning, CHANGELOG, gemify, etc.