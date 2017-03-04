/* optionpp -- read command-line program options
   Copyright (C) 2017 Gregory Kikola.

   This file is part of option++.

   option++ is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   option++ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with option++.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Written by Gregory Kikola <gkikola@gmail.com>. */

#include <iostream>
#include "optionpp.hpp"

void print_usage()
{
  std::cout << "Usage: option++ [options]" << std::endl;
}

int main(int argc, char* argv[])
{
  OptionParser opt = {
    {'?', "help", "", "give detailed usage information", -1},
    {0, "usage", "", "give a short usage message", -1},
    {0, "version", "", "display version information and exit", -1}
  };

  opt.allow_bad_opts();
  opt.allow_bad_args();
  opt.parse(argc, argv);

  if (opt.find('?') != opt.end()) {
    print_usage();
    opt.print_usage(std::cout);
    return 0;
  } else if (opt.find("usage") != opt.end()) {
    print_usage();
    return 0;
  } else if (opt.find("version") != opt.end()) {
    std::cout << "option++ " << OPTIONPP_VERSION << std::endl;
    std::cout << "Copyright (C) 2017 Gregory Kikola\n"
              << "License GPLv3+: GNU GPL version 3 or later\n"
              << "<http://www.gnu.org/licenses/gpl.html>.\n"
              << "This is free software: you are free to change and "
              << "redistribute it.\n"
              << "There is NO WARRANTY, to the extent permitted by law.\n"
              << std::endl;
    std::cout << "Written by Gregory Kikola <gkikola@gmail.com>." << std::endl;

    return 0;
  }
  
  for (const auto& o : opt) {
    if (o.short_name)
      std::cout << "-" << o.short_name;
    if (o.short_name && o.long_name != "")
      std::cout << " (";
    if (o.long_name != "")
      std::cout << "--" << o.long_name;
    if (o.short_name && o.long_name != "")
      std::cout << ")";
    if (o.argument != "")
      std::cout << "=" << o.argument;
    std::cout << "\n";
  }

  return 0;
}
