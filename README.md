option++
========

**option++** is a C++ facility for reading command-line program options.
**option++** provides an easy way to read, validate, and process
command-line options and it can also print detailed program help information.

**option++** can be used simply by copying [optionpp.cpp](src/optionpp.cpp)
and [optionpp.hpp](src/optionpp.hpp) into a project's source directory,
provided that the project is distributed under a license that is compatible
with the [GNU GPL](https://www.gnu.org/licenses/gpl.html) (see the file
[COPYING](COPYING) for more details).


Example Program
---------------

The following is a simple demonstration of how to use **option++**.

    #include <iostream>
    #include <string>
    #include "optionpp.hpp"

    int main(int argc, char* argv[])
    {
      bool show_line_nums = false;
      int line_width = 16;
  
      OptionParser opt = {
        {'n', "line-numbers", "", "display line numbers"},
        {'w', "line-width", "WIDTH", "set maximum display width "
        "for each line"},
        {'?', "help", "", "give detailed usage information"},
      };

      opt.parse(argc, argv);

      for (const auto& o : opt) {
        switch (o.short_name) {
        case 'n':
          show_line_nums = true;
          break;
        case 'w':
          line_width = std::stoi(o.argument);
          break;
        case '?':
          opt.print_usage(std::cout);
          return 0;
        }
      }
    
      //...
      
      return 0;
    }


Copyright
---------

Copyright &copy; 2017 Gregory Kikola. License GPLv3+: [GNU GPL version 3
or later](http://www.gnu.org/licenses/gpl.html).

**option++** is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law. See the file
[COPYING](COPYING) for more details.
