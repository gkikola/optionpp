Option++
========

Option++ is a C++ library for reading command-line program
options. Option++ provides an easy way to read, validate, and process
command-line options and it can also print detailed program help
information.

Option++ supports both long and short option names (e.g., `myprogram
--version` or `myprogram -abcd file.txt`). Each program option may
take a mandatory or optional argument (e.g., `myprogram --color=red`
or `myprogram -C red` or even `myprogram -Cred`). A double dash, `--`,
may be used by itself to indicate the end of the options, so that
remaining command-line arguments do not get parsed by Option++.


Copyright
---------

Copyright &copy; 2017-2020 Greg Kikola. License BSL-1.0: [Boost
Software License version 1.0](https://www.boost.org/LICENSE_1_0.txt).

Option++ is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. See the file
[LICENSE.txt](LICENSE.txt) for more details.
