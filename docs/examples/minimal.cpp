#include <iostream>
#include <optionpp/optionpp.hpp>

using optionpp::parser;

int main(int argc, char* argv[]) {
  bool show_help = false;
  parser opt_parser;
  opt_parser["help"].short_name('?').bind_bool(&show_help)
    .description("Show help information");
  try {
    opt_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  if (show_help)
    opt_parser.print_help(std::cout) << std::endl;

  return 0;
}
