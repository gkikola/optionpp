#include <iostream>
#include <string>

#define OPTIONPP_MAIN
#include <optionpp/optionpp.hpp>

struct Settings {
  bool show_help {false};
  bool show_version {false};
  bool verbose {false};
  std::string output_file;
};

using optionpp::parser;
using optionpp::parser_result;
using optionpp::non_option_const_iterator;

void print_version() {
  std::cout << "My Program 1.0" << std::endl;
}

int main(int argc, char* argv[]) {
  Settings settings;
  parser opt_parser;

  // Set up options
  opt_parser["help"].short_name('?')
    .description("Show help information")
    .bind_bool(&settings.show_help);
  opt_parser["version"]
    .description("Show program version information")
    .bind_bool(&settings.show_version);
  opt_parser["verbose"].short_name('v')
    .description("Display additional explanations")
    .bind_bool(&settings.verbose);
  opt_parser["output"].short_name('o')
    .description("File to write output")
    .bind_string(&settings.output_file);

  parser_result result;

  // Parse options
  try {
    result = opt_parser.parse(argc, argv);
  } catch(const optionpp::error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  // Show help/version info
  if (settings.show_help) {
    print_version();
    std::cout << "This program does important stuff.\n\n"
              << "My Program accepts the following options:\n"
              << opt_parser << std::endl;
    return 0;
  } else if (settings.show_version) {
    print_version();
    return 0;
  }

  if (!settings.output_file.empty()) {
    if (settings.verbose)
      std::cout << "Writing output to "
                << settings.output_file << std::endl;

    // Write output...
  } else {
    if (settings.verbose)
      std::cout << "Writing output to standard out" << std::endl;

    // Write output...
  }

  // Iterate over non-option arguments
  for (const auto& entry : non_option_const_iterator(result)) {
    if (!entry.is_option)
      std::cout << "Received argument '"
                << entry.original_text
                << "'" << std::endl;
  }

  return 0;
}
