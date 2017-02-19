#include <iostream>
#include "optionpp.hpp"

int main(int argc, char* argv[])
{
  OptionParser opt = {
      {'?', "help", "", "Display help text"},
      {0, "version", "", "Display program version"}
  };

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
