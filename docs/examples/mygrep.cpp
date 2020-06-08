#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <optionpp/optionpp.hpp>

enum class PatternType { string, basic_regex,
                         perl_regex, extended_regex };

struct Options {
  // Pattern options
  PatternType type;
  std::string pattern_file;
  std::string pattern_str;
  bool ignore_case{};
  bool whole_words{};
  bool whole_lines{};

  // Miscellaneous options
  bool suppress_errors{};
  bool invert_match{};
  bool show_version{};
  bool show_help{};

  // Output options
  unsigned max_lines{};
  bool quiet{};
  bool count_only{};
};

int main(int argc, char* argv[]) {
  const std::string usage{"mygrep [OPTION]... PATTERNS [FILE]..."};

  Options opts;
  optionpp::parser opt_parser;

  auto& pattern_group = opt_parser.group("Pattern selection and interpretation:");
  pattern_group["extended-regexp"].short_name('E')
    .description("PATTERNS are extended regular expressions");
  pattern_group["fixed-strings"].short_name('F')
    .description("PATTERNS are strings");
  pattern_group["basic-regexp"].short_name('G')
    .description("PATTERNS are basic regular expressions (default)");
  pattern_group["perl-regexp"].short_name('P')
    .description("PATTERNS are Perl regular expressions");
  pattern_group["regexp"].short_name('e').argument("PATTERNS", true)
    .description("Use PATTERNS for matching");
  pattern_group["file"].short_name('f').argument("FILE", true)
    .description("take PATTERNS from FILE").bind_string(&opts.pattern_file);
  pattern_group["ignore-case"].short_name('i')
    .description("ignore case distinctions in patterns and data");
  pattern_group["no-ignore-case"]
    .description("do not ignore case distinctions (default)");
  pattern_group["word-regexp"].short_name('w').bind_bool(&opts.whole_words)
    .description("match only whole words");
  pattern_group["line-regexp"].short_name('x').bind_bool(&opts.whole_lines)
    .description("match only whole lines");

  auto& misc_group = opt_parser.group("Miscellaneous:");
  misc_group["no-message"].short_name('s').bind_bool(&opts.suppress_errors)
    .description("suppress error messages");
  misc_group["invert-match"].short_name('v').bind_bool(&opts.invert_match)
    .description("select non-matching lines");
  misc_group["version"].short_name('V').bind_bool(&opts.show_version)
    .description("display version information and exit");
  misc_group["help"].bind_bool(&opts.show_help)
    .description("display this help text and exit");

  auto& output_group = opt_parser.group("Output control:");
  output_group["max-count"].short_name('m').argument("NUM", true)
    .bind_uint(&opts.max_lines).description("stop after NUM selected lines");
  output_group["quiet"].short_name('q').bind_bool(&opts.quiet)
    .description("suppress all normal output");
  output_group["count"].short_name('c').bind_bool(&opts.count_only)
    .description("print only a count of selected lines per FILE");

  optionpp::parser_result result;
  try {
    result = opt_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 2;
  }

  if (opts.show_help) {
    std::cout << "Usage: " << usage << "\n"
              << "Search for PATTERNS in each FILE.\n"
              << "Example: grep -i 'hello world' menu.h main.c\n"
              << "PATTERNS can contain multiple patterns "
              << "separated by newlines.\n\n";
    opt_parser.print_help(std::cout);
    std::cout << "\n\nExit status is 0 if any line is selected, "
              << "1 otherwise;\nif any error occurs and -q is not given, "
              << "the exit status is 2." << std::endl;
    return 0;
  }

  if (opts.show_version) {
    std::cout << "mygrep 1.0\n"
              << "An example program using the Option++ library.\n\n"
              << "Copyright (C) 2020 Greg Kikola\n"
              << "License: BSL-1.0: Boost Software License version 1.0"
              << std::endl;
    return 0;
  }
}
