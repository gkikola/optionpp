/*
 * example_mygrep -- Simple grep clone to demonstrate Option++
 *
 * Run example_mygrep --help for usage information.
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include <optionpp/optionpp.hpp>

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::regex;
using std::string;
using std::vector;
using optionpp::parser;

enum class PatternType { basic_regex, extended_regex };

struct Options {
  // Pattern options
  PatternType type { PatternType::basic_regex };
  vector<string> files;
  vector<string> patterns;
  string pattern_file;
  bool ignore_case{};

  // Miscellaneous options
  bool suppress_errors{};
  bool invert_match{};
  bool show_version{};
  bool show_help{};

  // Output options
  unsigned max_lines{};
  bool limit_lines{};
  bool quiet{};
  bool count_only{};
};

template <typename InputIt>
void read_patterns_from_file(const string& filename,
                             InputIt dest);
bool match_file(const string& filename, const Options& opts);
bool does_line_match(const string& line, const Options& opts);

int main(int argc, char* argv[]) {
  const string usage{"mygrep [OPTION]... PATTERNS [FILE]..."};

  Options opts;
  parser opt_parser;

  // Set up the options
  auto& pattern_group = opt_parser.group("Pattern selection and interpretation:");
  pattern_group["extended-regexp"].short_name('E')
    .description("PATTERNS are extended regular expressions");
  pattern_group["basic-regexp"].short_name('G')
    .description("PATTERNS are basic regular expressions (default)");
  pattern_group["regexp"].short_name('e').argument("PATTERNS", true)
    .description("Use PATTERNS for matching");
  pattern_group["file"].short_name('f').argument("FILE", true)
    .description("take PATTERNS from FILE").bind_string(&opts.pattern_file);
  pattern_group["ignore-case"].short_name('i')
    .description("ignore case distinctions in patterns and data");
  pattern_group["no-ignore-case"]
    .description("do not ignore case distinctions (default)");

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
    .bind_uint(&opts.max_lines).bind_bool(&opts.limit_lines)
    .description("stop after NUM selected lines");
  output_group["quiet"].short_name('q').bind_bool(&opts.quiet)
    .description("suppress all normal output");
  output_group["count"].short_name('c').bind_bool(&opts.count_only)
    .description("print only a count of selected lines per FILE");

  optionpp::parser_result result;
  try {
    result = opt_parser.parse(argc, argv);
  } catch(const std::exception& e) {
    if (!opts.suppress_errors)
      cerr << "Error: " << e.what() << std::endl;
    return 2;
  }

  if (opts.show_help) {
    cout << "Usage: " << usage << "\n"
         << "Search for PATTERNS in each FILE.\n"
         << "Example: grep -i 'hello world' menu.h main.c\n\n";
    opt_parser.print_help(std::cout);
    cout << "\n\nExit status is 0 if any line is selected, "
         << "1 otherwise;\nif any error occurs and -q is not given, "
         << "the exit status is 2." << std::endl;
    return 0;
  }

  if (opts.show_version) {
    cout << "mygrep 1.0\n"
         << "An example program using the Option++ library.\n\n"
         << "Copyright (C) 2020 Greg Kikola\n"
         << "License: BSL-1.0: Boost Software License version 1.0"
         << endl;
    return 0;
  }

  // Iterate over options and collect filenames
  bool first_arg = true;
  for (const auto& entry : result) {
    if (entry.is_option) {
      if (entry.short_name == 'E')
        opts.type = PatternType::extended_regex;
      else if (entry.short_name == 'G')
        opts.type = PatternType::basic_regex;
      else if (entry.short_name == 'i')
        opts.ignore_case = true;
      else if (entry.long_name == "ignore-case")
        opts.ignore_case = false;
      else if (entry.short_name == 'e')
        opts.patterns.push_back(entry.argument);
    } else { // Non-option
      // First argument is the pattern
      if (first_arg) {
        first_arg = false;
        opts.patterns.push_back(entry.original_text);
      } else {
        opts.files.push_back(entry.original_text);
      }
    }
  } // End for

  // Read and match in each file
  bool match_found = false;
  try {
    if (!opts.pattern_file.empty())
      read_patterns_from_file(opts.pattern_file,
                              std::back_inserter(opts.patterns));
    for (const auto& filename : opts.files) {
      if (match_file(filename, opts))
        match_found = true;
    }
  } catch(const std::exception& e) {
    if (!opts.suppress_errors)
      cerr << "Error: " << e.what() << endl;
    return 2;
  }

  return match_found ? 0 : 1;
}

// Read patterns into dest
template <typename InputIt>
void read_patterns_from_file(const string& filename,
                             InputIt dest) {
  ifstream file(filename);
  if (!file.is_open()) {
    throw std::invalid_argument("Could not open file '" + filename + "'");
  }

  string line;
  while (std::getline(file, line))
    *dest++ = line;
}

// Print matches and return true if match found
bool match_file(const string& filename, const Options& opts) {
  ifstream file(filename);

  if (!file.is_open())
    throw std::invalid_argument("Could not open file '" + filename + "'");

  string cur_line;
  int count = 0;

  while (std::getline(file, cur_line)) {
    bool match = does_line_match(cur_line, opts);

    if (opts.invert_match)
      match = !match;

    if (match)
      ++count;

    if (!opts.count_only) {
      if (match && !opts.quiet) {
        if (opts.files.size() > 1)
          cout << filename << ":";
        cout << cur_line << "\n";
      }
    }

    if (opts.limit_lines && count >= opts.max_lines)
      break;
  } // End while

  if (opts.count_only && !opts.quiet) {
    if (opts.files.size() > 1)
      cout << filename << ":";
    cout << count << "\n";
  }

  return count > 0;
}

// Return true if line matches any of the patterns in opts.patterns
bool does_line_match(const string& line, const Options& opts) {
  auto flags = regex::basic;
  if (opts.type == PatternType::extended_regex)
    flags = regex::extended;
  if (opts.ignore_case)
    flags |= regex::icase;

  for (const auto& p : opts.patterns) {
    regex pattern{p, flags};
    std::smatch m;
    if (std::regex_search(line, m, pattern))
      return true;
  }

  return false;
}
