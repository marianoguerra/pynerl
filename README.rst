pynerl
======


status
------

I would really like to finish this, but for now it's on hold, if you would like to help, fork it and send pull requests.

about
-----

pynerl is a set of NIFs that allow to run python code from erlang

the NIFs embed python and allow to run eval and call functions.

pynerl does the transformation of data types.

an example session::

        1> pynerl:eval("t = 1", "t").
        1
        2> pynerl:eval("t = 1 * 2", "t").
        2
        3> pynerl:eval("import time;t = time.time()", "t").            
        1274236802.877999
        4> pynerl:eval("import random;t = random.random()", "t").         
        0.45102117275294684
        5> pynerl:eval("t = print('hello erlang or python')", "t").                                 
        hello erlang or python
                              none
        6> pynerl:call("time", "time", []).                        
        1274236859.510859
        7> pynerl:call("random", "random", []).
        0.9623136682858975
        8> pynerl:eval("t = True", "t").                           
        true
        9> pynerl:eval("t = 2.3", "t"). 
        2.3

Information
-----------

This is just an experiment, some of the code may be usable for other things but
as the documentation on NIFs says::

        Avoid doing lengthy work in NIF calls as that may degrade the
        responsiveness of the VM. NIFs are called directly by the same
        scheduler thread that executed the calling Erlang code. The calling
        scheduler will thus be blocked from doing any other work until the NIF
        returns.

That means that you shouldn't run a really long python method with it since it would
block the scheduler.

I may try to make something similar to `erlectricity`__

__ http://github.com/mojombo/erlectricity/
