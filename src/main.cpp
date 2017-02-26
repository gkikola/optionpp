#include <iostream>
#include "optionpp.hpp"

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
