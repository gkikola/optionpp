#include <iostream>
#include <string>
#include <optionpp/optionpp.hpp>

struct Settings {
  bool show_help {false};
  bool show_version {false};
  bool verbose {false};
  std::string output_file;
};

using optionpp::parser;

void print_version() {
  std::cout << "My Program 1.0" << std::endl;
}

int main(int argc, char* argv[]) {
  Settings settings;
  parser opt_parser;

  // Set up options
  try {
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
  } catch(const optionpp::error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  // Parse options
  opt_parser.parse(argc, argv);

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

  /*
   * Example input: example_basic -omyfile.txt --verbose
   * Example output:
   * Writing output to myfile.txt
   */

  /*
   * Example input: example_basic --help
   * Example output:
   * My Program 1.0
   * This program does important stuff.
   *
   * My Program accepts the following options:
   *   -?, --help Show help information
   *       --version Show program version information
   *   -v, --verbose Display additional explanations
   *   -o, --output[=STRING] File to write output
   */

  return 0;
}
