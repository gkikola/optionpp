/*
 * Parsing options from a string
 */

#include <iostream>
#include <string>
#include <optionpp/optionpp.hpp>

int main() {
  optionpp::parser opt_parser;
  opt_parser['a'];
  opt_parser['b'];
  opt_parser['c'];

  std::string line;
  while (true) {
    std::cout << "Enter some words, or leave blank to exit:\n";

    if (!std::getline(std::cin, line))
      break;

    if (line.empty())
      break;

    optionpp::parser_result result;
    try {
      result = opt_parser.parse(line);
    } catch(const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      continue;
    }

    for (auto& entry : result) {
      if (entry.is_option) {
        switch (entry.short_name) {
        case 'a':
          std::cout << "Found option a\n";
          break;
        case 'b':
          std::cout << "Found option b\n";
          break;
        case 'c':
          std::cout << "Found option c\n";
          break;
        }
      } else {
        std::cout << "Found word '"
                  << entry.original_text << "'\n";
      }
    }
  }

}
