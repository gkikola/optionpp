#include <iostream>
#include <optionpp/optionpp.hpp>

// Snippets from this file are used in the Getting Started tutorial.

int main(int argc, char* argv[]) {
  optionpp::parser my_parser;
  bool show_help = false;
  my_parser["help"].short_name('?').bind_bool(&show_help)
    .description("Display help message");
  my_parser["verbose"].short_name('v')
    .description("Increase verbosity");
  my_parser["file"].short_name('f').argument("FILE", true)
    .description("Write output to FILE");

  unsigned int precision = 16;
  std::string method;
  bool optimize = false;

  auto& math = my_parser.group("Math options");
  math["precision"].short_name('p').bind_uint(&precision)
    .argument("DIGITS").description("Output precision");
  math["method"].argument("TYPE").bind_string(&method)
    .short_name('m')
    .description("Algorithm to use: "
                 "'newton' for Newton's method (default), "
                 "'bisection' for bisection method, "
                 "'secant' for secant method, "
                 "'hybrid' for hybrid method");
  math["optimize"].short_name('o').bind_bool(&optimize)
    .description("Use experimental speed optimizations");

  optionpp::parser_result result;
  try {
    result = my_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  bool verbose = false;
  std::string file;
  my_parser["verbose"].bind_bool(&verbose);
  my_parser["file"].bind_string(&file);

  unsigned int num_iterations = 100; // Default number of iterations
  my_parser["num-iterations"].short_name('i').bind_uint(&num_iterations)
    .description("Number of iterations to perform").argument("ITERS");

  my_parser.parse(argc, argv);

  if (show_help) {
    std::cout << "You are using Solve version 1.0\n\n"
              << "This program accepts the following options:\n";
    my_parser.print_help(std::cout) << std::endl;
    return 0;
  }

  for (const auto& entry : optionpp::non_option_iterator(result)) {
    std::cout << "Received argument '" << entry.original_text << "'\n";
  }

  return 0;
}
