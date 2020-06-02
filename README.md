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


Development Status
------------------

Option++ is currently being redesigned and will be (mostly)
rewritten. You can find the unfinished source files for version 2.0 in
the 'v2' branch.


Requirements
------------

Option++ depends only on the C++ Standard Library. To compile and use
Option++, your compiler must support C++11 features. Unfortunately,
Option++ is not designed for projects that require compatibility with
older compilers.


Setup
-----

The Option++ library itself consists of only two files: a header file
and an implementation source file. Option++ is designed to be used
simply by copying these files
([optionpp.cpp](src/optionpp/optionpp.cpp) and
[optionpp.hpp](src/optionpp/optionpp.hpp)) directly into your
project's source directory. Then simply put `#include
"optionpp/optionpp.hpp"` in the appropriate source file(s).

See the example below to get started.

For detailed information about using Option++ and a complete class
reference, see the
[documentation](http://www.gregkikola.com/optionpp/).


Example Program
---------------

The following is a simple demonstration of how to use Option++.

    #include <exception>
    #include <iostream>
    #include <string>
    #include "optionpp/optionpp.hpp"

    int main(int argc, char* argv[])
    {
      bool show_line_nums = false;
      int line_width = 16;

      optionpp::OptionParser opt = {
        {'n', "line-numbers", "", "display line numbers"},
        {'w', "line-width", "WIDTH", "set maximum display width "
         "for each line"},
        {'?', "help", "", "give detailed usage information"},
      };

      try {
        opt.parse(argc, argv);

        for (const auto& o : opt) {
          switch (o.short_name) {
          case 'n':
            show_line_nums = true;
            break;
          case 'w':
            line_width = o.arg_to_unsigned();
            break;
          case '?':
            opt.print_usage(std::cout);
            return 0;
          }
        }
      } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
      }

      //...

      return 0;
    }


Copyright
---------

Copyright &copy; 2017-2018 Greg Kikola. License BSL-1.0: [Boost
Software License version 1.0](https://www.boost.org/LICENSE_1_0.txt).

Option++ is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. See the file
[LICENSE.txt](LICENSE.txt) for more details.
