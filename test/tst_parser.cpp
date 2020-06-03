/* Option++ -- read command-line program options
 * Copyright (C) 2017-2020 Greg Kikola.
 *
 * This file is part of Option++.
 *
 * Option++ is free software: you can redistribute it and/or modify
 * it under the terms of the Boost Software License version 1.0.
 *
 * Option++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Boost Software License for more details.
 *
 * You should have received a copy of the Boost Software License
 * along with Option++.  If not, see
 * <https://www.boost.org/LICENSE_1_0.txt>.
 */
/* Written by Greg Kikola <gkikola@gmail.com>. */

#include <string>
#include <vector>
#include <catch2/catch.hpp>
#include "../src/parser.hpp"

using namespace optionpp;

TEST_CASE("parser") {
  parser empty{};
  parser example{};
  example.add_option().long_name("help").short_name('?')
    .description("Show help information");
  example.add_option().long_name("version").description("Get version info");
  example.add_option().long_name("verbose").short_name('v')
    .description("Show verbose output");
  example.add_option().long_name("output").short_name('o')
    .argument("FILE", argument_type::required)
    .description("Write output to FILE").group("File options");
  example.add_option().short_name('n').description("Show line numbers")
    .group("File options");
  example.add_option().long_name("all").short_name('a')
    .description("Show all lines").group("Display options");
  example.add_option().long_name("indent")
    .argument("WIDTH", argument_type::optional)
    .description("Indent each line by WIDTH spaces (default: 2)")
    .group("Display options");
  example.add_option().long_name("force").short_name('f')
    .description("Force file creation").group("File options");

  SECTION("simple parsing") {
    std::vector<std::string> cmd_line;
    auto result = empty.parse(cmd_line.begin(), cmd_line.end());
    REQUIRE(result.empty());

    result = example.parse(cmd_line.begin(), cmd_line.end());
    REQUIRE(result.empty());

    cmd_line = {"myprog", "-afv", "--help", "command1", "-n", "--version", "command2"};
    REQUIRE(result.size() == 8);
    REQUIRE(result[0].original_text == "-a");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "all");
    REQUIRE(result[0].short_name == 'a');
    REQUIRE(result[0].argument.empty());

    REQUIRE(result[1].original_text == "-f");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].long_name == "force");
    REQUIRE(result[1].short_name == 'f');
    REQUIRE(result[1].argument.empty());

    REQUIRE(result[2].original_text == "-v");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].long_name == "verbose");
    REQUIRE(result[2].short_name == 'v');
    REQUIRE(result[2].argument.empty());

    REQUIRE(result[3].original_text == "--help");
    REQUIRE(result[3].is_option);
    REQUIRE(result[3].long_name == "help");
    REQUIRE(result[3].short_name == '?');
    REQUIRE(result[3].argument.empty());

    REQUIRE(result[4].original_text == "command1");
    REQUIRE_FALSE(result[4].is_option);
    REQUIRE(result[4].long_name.empty());
    REQUIRE(result[4].short_name == '\0');
    REQUIRE(result[4].argument.empty());

    REQUIRE(result[5].original_text == "-n");
    REQUIRE(result[5].is_option);
    REQUIRE(result[5].long_name.empty());
    REQUIRE(result[5].short_name == 'n');
    REQUIRE(result[5].argument.empty());

    REQUIRE(result[6].original_text == "--version");
    REQUIRE(result[6].is_option);
    REQUIRE(result[6].long_name == "version");
    REQUIRE(result[6].short_name == '\0');
    REQUIRE(result[6].argument.empty());

    REQUIRE(result[7].original_text == "command2");
    REQUIRE_FALSE(result[7].is_option);
    REQUIRE(result[7].long_name.empty());
    REQUIRE(result[7].short_name == '\0');
    REQUIRE(result[7].argument.empty());
  }

  SECTION("argc, argv") {
    int argc = 4;
    const char* argv[] = { "myprog", "command", "-an", "--help" };
    auto result = example.parse(argc, argv);
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == "command");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-a");
    REQUIRE(result[1].is_option);
    REQUIRE(result[2].original_text == "-n");
    REQUIRE(result[2].is_option);
    REQUIRE(result[3].original_text == "--help");
    REQUIRE(result[3].is_option);
  }

  SECTION("no ignore_first") {
    std::vector<std::string> cmd_line{"command1 -n command2"};
    auto result = example.parse(cmd_line.begin(), cmd_line.end(), false);
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "command1");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-n");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].short_name == 'n');
    REQUIRE(result[2].original_text == "command2");
    REQUIRE_FALSE(result[2].is_option);
  }

  SECTION("string parsing") {
    auto result = example.parse("cmd1 -n cmd2");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "cmd1");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-n");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].short_name == 'n');
    REQUIRE(result[2].original_text == "cmd2");
    REQUIRE_FALSE(result[2].is_option);

    result = example.parse("-an 'command 1' another\\ command --version");
    REQUIRE(result.size() == 5);
    REQUIRE(result[0].original_text == "-a");
    REQUIRE(result[1].original_text == "-n");
    REQUIRE(result[2].original_text == "command 1");
    REQUIRE(result[3].original_text == "another command");
    REQUIRE(result[4].original_text == "--version");

    result = empty.parse("");
    REQUIRE(result.empty());
  }

  SECTION("no options") {
    auto result = example.parse("command1 \"command -n 2\" - 'command 3'");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == "command1");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "command -n 2");
    REQUIRE_FALSE(result[1].is_option);
    REQUIRE(result[2].original_text == "-");
    REQUIRE_FALSE(result[2].is_option);
    REQUIRE(result[3].original_text == "command 3");
    REQUIRE_FALSE(result[3].is_option);
  }

  SECTION("invalid options") {
    REQUIRE_THROWS_AS(example.parse("myprog -q", true), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("cmd1 -nvb? --version"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("--fix-broken"), std::invalid_argument);
  }

  SECTION("missing argument") {
    REQUIRE_THROWS_AS(example.parse("myprog -o"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("command -o -n"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("command -o --version"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("myprog -o= -n"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("command --output= -n"), std::invalid_argument);
    REQUIRE_THROWS_AS(example.parse("command --output --version"), std::invalid_argument);
    REQUIRE_NOTHROW(example.parse("myprog -o out"));
  }

  SECTION("arguments") {
    auto result = example.parse("myprog --indent", true);
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].original_text == "--indent");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "indent");
    REQUIRE(result[0].argument.empty());

    result = example.parse("myprog --indent 4", true);
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].original_text == "--indent 4");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "indent");
    REQUIRE(result[0].argument == "4");

    result = example.parse("command -fo myfile.txt --indent=5 -n");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == "command");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-f");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].argument.empty());
    REQUIRE(result[2].original_text == "-o myfile.txt");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].long_name == "output");
    REQUIRE(result[2].short_name == 'o');
    REQUIRE(result[2].argument == "myfile.txt");
    REQUIRE(result[3].original_text == "--indent=5");
    REQUIRE(result[3].is_option);
    REQUIRE(result[3].long_name == "indent");
    REQUIRE(result[3].short_name == '\0');
    REQUIRE(result[3].argument == "5");
    REQUIRE(result[4].original_text == "-n");
    REQUIRE(result[4].is_option);

    result = example.parse("--indent --version");
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].original_text == "--indent");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "indent");
    REQUIRE(result[0].argument.empty());
    REQUIRE(result[1].original_text == "--version");
    REQUIRE(result[1].is_option);

    result = example.parse("-omyfile");
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].original_text == "-omyfile");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "output");
    REQUIRE(result[0].short_name == 'o');
    REQUIRE(result[0].argument == "myfile");

    result = example.parse("command -o=myfile -n");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "command");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-o=myfile");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].long_name == "output");
    REQUIRE(result[1].short_name == 'o');
    REQUIRE(result[1].argument == "myfile");
    REQUIRE(result[2].original_text == "-n");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].long_name.empty());
    REQUIRE(result[2].short_name == 'n');
    REQUIRE(result[2].argument.empty());
  }

  SECTION("end-of-option separator") {
    auto result = example.parse("myprog --help -avofile.txt -- command --version -n --output", true);
    REQUIRE(result.size() == 8);
    REQUIRE(result[0].original_text == "--help");
    REQUIRE(result[0].is_option);
    REQUIRE(result[1].original_text == "-a");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].argument.empty());
    REQUIRE(result[2].original_text == "-v");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].argument.empty());
    REQUIRE(result[3].original_text == "-ofile.txt");
    REQUIRE(result[3].is_option);
    REQUIRE(result[3].argument == "file.txt");
    REQUIRE(result[4].original_text == "command");
    REQUIRE_FALSE(result[4].is_option);
    REQUIRE(result[5].original_text == "--version");
    REQUIRE_FALSE(result[5].is_option);
    REQUIRE(result[5].long_name.empty());
    REQUIRE(result[5].short_name == '\0');
    REQUIRE(result[6].original_text == "-n");
    REQUIRE_FALSE(result[6].is_option);
    REQUIRE(result[6].long_name.empty());
    REQUIRE(result[6].short_name == '\0');
    REQUIRE(result[7].original_text == "--output");
    REQUIRE_FALSE(result[7].is_option);
    REQUIRE(result[7].long_name.empty());
    REQUIRE(result[7].short_name == '\0');
  }

  SECTION("custom strings") {
    struct mystrings : public string_traits<std::string> {
      using string_type = std::string;
      using char_type = char;
      static string_type whitespace() { return "."; }
      static string_type quotes() { return "^"; }
      static char_type escape_char() { return '\\'; }
      static string_type long_option_prefix() { return ":::"; }
      static string_type short_option_prefix() { return "<>"; }
      static string_type end_of_options() { return "+++"; }
      static string_type assignment() { return "->"; }
    };

    basic_parser<std::string, mystrings> parser;
    parser.add_option().long_name("help").short_name('?')
      .description("Show help information");
    parser.add_option().long_name("version").description("Get version info");
    parser.add_option().long_name("verbose").short_name('v')
      .description("Show verbose output");
    parser.add_option().long_name("output").short_name('o')
      .argument("FILE", argument_type::required)
      .description("Write output to FILE").group("File options");
    parser.add_option().short_name('n').description("Show line numbers")
      .group("File options");
    parser.add_option().long_name("all").short_name('a')
      .description("Show all lines").group("Display options");
    parser.add_option().long_name("indent")
      .argument("WIDTH", argument_type::optional)
      .description("Indent each line by WIDTH spaces (default: 2)")
      .group("Display options");
    parser.add_option().long_name("force").short_name('f')
      .description("Force file creation").group("File options");

    auto result = parser.parse(":::help.<>nf.:::output->file.+++.<>a.:::version.^hello.world^");

    REQUIRE(result.size() == 7);
    REQUIRE(result[0].original_text == ":::help");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "help");
    REQUIRE(result[0].short_name == '?');
    REQUIRE(result[0].argument.empty());

    REQUIRE(result[1].original_text == "<>n");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].long_name.empty());
    REQUIRE(result[1].short_name == 'n');
    REQUIRE(result[1].argument.empty());

    REQUIRE(result[2].original_text == "<>f");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].long_name == "force");
    REQUIRE(result[2].short_name == 'f');
    REQUIRE(result[2].argument.empty());

    REQUIRE(result[3].original_text == ":::output->file");
    REQUIRE(result[3].is_option);
    REQUIRE(result[3].long_name == "output");
    REQUIRE(result[3].short_name == 'o');
    REQUIRE(result[3].argument == "file");

    REQUIRE(result[4].original_text == "<>a");
    REQUIRE_FALSE(result[4].is_option);
    REQUIRE(result[4].long_name.empty());
    REQUIRE(result[4].short_name == '\0');
    REQUIRE(result[4].argument.empty());

    REQUIRE(result[5].original_text == ":::version");
    REQUIRE_FALSE(result[5].is_option);
    REQUIRE(result[5].long_name.empty());
    REQUIRE(result[5].short_name == '\0');
    REQUIRE(result[5].argument.empty());

    REQUIRE(result[6].original_text == "hello.world");
    REQUIRE_FALSE(result[6].is_option);
  }

  SECTION("wide strings") {
    using wstring = std::basic_string<wchar_t>;
    basic_parser<wstring, string_traits<wstring>> lexample;
    lexample.add_option().long_name(L"help").short_name(L'?');
    lexample.add_option().long_name(L"width").short_name(L'w')
      .argument(L"WIDTH", argument_type::optional);

    auto result = lexample.parse(L"--help -?w 32 command");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == L"--help");
    REQUIRE(result[0].is_option);
    REQUIRE(result[1].original_text == L"-?");
    REQUIRE(result[1].is_option);
    REQUIRE(result[2].original_text == L"-w 32");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].argument == L"32");
    REQUIRE(result[3].original_text == L"command");
    REQUIRE_FALSE(result[3].is_option);
  }
}
