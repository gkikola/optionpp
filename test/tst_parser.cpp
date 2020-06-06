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

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <catch2/catch.hpp>
#include "../src/parser.hpp"

using namespace optionpp;

TEST_CASE("parser") {
  struct settings {
    bool help{};
    bool version{};
    bool verbose{};
    std::string file;
    bool has_file{};
    unsigned indent{2};
    bool force{};
    bool line_nos{};
    std::string color;
  };

  settings data;
  parser empty{};
  parser example{};
  example.add_option().long_name("help").short_name('?')
    .description("Show help information").bind_bool(&data.help);
  example.add_option().long_name("version").description("Get version info")
    .bind_bool(&data.version);
  example.add_option().long_name("verbose").short_name('v')
    .description("Show verbose output").bind_bool(&data.verbose);
  example.group("Output options").add_option()
    .long_name("output").short_name('o')
    .argument("FILE", true).bind_string(&data.file)
    .bind_bool(&data.has_file)
    .description("Write output to FILE");
  example.group("Output options").add_option()
    .short_name('n').bind_bool(&data.line_nos)
    .description("Show line numbers");
  example.add_option().long_name("all").short_name('a')
    .description("Show all lines");
  example.group("Output options").add_option().long_name("indent")
    .argument("WIDTH", false).bind_uint(&data.indent)
    .description("Indent each line by WIDTH spaces (default: 2)");
  example.add_option().long_name("force").short_name('f').bind_bool(&data.force)
    .description("Force file creation");
  example.add_option("color", 'c', "Set the color of the output",
                     "COLOR", false, "Output options").bind_string(&data.color);

  SECTION("simple parsing") {
    std::vector<std::string> cmd_line;
    auto result = empty.parse(cmd_line.begin(), cmd_line.end());
    REQUIRE(result.empty());

    result = example.parse(cmd_line.begin(), cmd_line.end());
    REQUIRE(result.empty());

    cmd_line = {"myprog", "-afv", "--help", "command1", "-n", "--version", "command2"};
    result = example.parse(cmd_line.begin(), cmd_line.end());
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
    std::vector<std::string> cmd_line{"command1", "-n", "command2"};
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
    auto result = example.parse(R"(command1 "command -n 2" - 'command 3')");
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
    REQUIRE_THROWS_WITH(example.parse("myprog -q", true),
                        "invalid option: '-q'");
    REQUIRE_THROWS_WITH(example.parse("cmd1 -nvb? --version"),
                        "invalid option: '-b'");
    REQUIRE_THROWS_WITH(example.parse("--fix-broken"),
                        "invalid option: '--fix-broken'");
  }

  SECTION("missing argument") {
    REQUIRE_THROWS_WITH(example.parse("myprog -o"),
                        "option '-o' requires an argument");
    REQUIRE_THROWS_WITH(example.parse("myprog --output  "),
                        "option '--output' requires an argument");

    // '=' should allow empty arguments
    REQUIRE_NOTHROW(example.parse("command --output= -n"));

    // Shouldn't throw if argument is provided
    REQUIRE_NOTHROW(example.parse("myprog -o out"));
  }

  SECTION("invalid arguments") {
    REQUIRE_THROWS_WITH(example.parse("command -fn=hello"),
                        "option '-n' does not accept arguments");
    REQUIRE_THROWS_WITH(example.parse("command --version=myversion"),
                        "option '--version' does not accept arguments");
    REQUIRE_THROWS_WITH(example.parse("cmd1 -= cmd2"),
                        "invalid option: '-='");
    REQUIRE_THROWS_WITH(example.parse("cmd1 -=abc cmd2"),
                        "invalid option: '-='");
    REQUIRE_THROWS_WITH(example.parse("cmd1 --= cmd2"),
                        "invalid option: '--='");
    REQUIRE_THROWS_WITH(example.parse("cmd1 --=lalala cmd2"),
                        "invalid option: '--='");
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
    REQUIRE(result.size() == 5);
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

    result = example.parse("-nfo=myfile.txt --indent 5");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == "-n");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].argument.empty());
    REQUIRE(result[1].original_text == "-f");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].argument.empty());
    REQUIRE(result[2].original_text == "-o=myfile.txt");
    REQUIRE(result[2].is_option);
    REQUIRE(result[2].long_name == "output");
    REQUIRE(result[2].short_name == 'o');
    REQUIRE(result[2].argument == "myfile.txt");
    REQUIRE(result[3].original_text == "--indent 5");
    REQUIRE(result[3].is_option);
    REQUIRE(result[3].long_name == "indent");
    REQUIRE(result[3].short_name == '\0');
    REQUIRE(result[3].argument == "5");

    result = example.parse("--indent --version");
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].original_text == "--indent");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "indent");
    REQUIRE(result[0].argument.empty());
    REQUIRE(result[1].original_text == "--version");
    REQUIRE(result[1].is_option);

    // Optional argument for -c not provided
    result = example.parse("command -c --version");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "command");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "-c");
    REQUIRE(result[1].is_option);
    REQUIRE(result[2].original_text == "--version");
    REQUIRE(result[2].is_option);
    REQUIRE(data.color == "");

    // Optional argument for --color not provided
    result = example.parse("command --color -nv");
    REQUIRE(result.size() == 4);
    REQUIRE(result[0].original_text == "command");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "--color");
    REQUIRE(result[1].is_option);
    REQUIRE(result[2].original_text == "-n");
    REQUIRE(result[2].is_option);
    REQUIRE(result[3].original_text == "-v");
    REQUIRE(result[3].is_option);
    REQUIRE(data.color == "");

    // Required argument: should interpret '--version' as argument
    result = example.parse("cmd --output --version");
    REQUIRE(result.size() == 2);
    REQUIRE(result[0].original_text == "cmd");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "--output --version");
    REQUIRE(result[1].is_option);
    REQUIRE(result[1].argument == "--version");

    result = example.parse("-omyfile");
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].original_text == "-omyfile");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "output");
    REQUIRE(result[0].short_name == 'o');
    REQUIRE(result[0].argument == "myfile");

    result = example.parse("-omy=file");
    REQUIRE(result.size() == 1);
    REQUIRE(result[0].original_text == "-omy=file");
    REQUIRE(result[0].is_option);
    REQUIRE(result[0].long_name == "output");
    REQUIRE(result[0].short_name == 'o');
    REQUIRE(result[0].argument == "my=file");

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

  SECTION("hyphen argument") {
    auto result = example.parse("cmd1 -a?n - cmd2");
    REQUIRE(result.size() == 6);
    REQUIRE(result[0].original_text == "cmd1");
    REQUIRE(result[1].original_text == "-a");
    REQUIRE(result[2].original_text == "-?");
    REQUIRE(result[3].original_text == "-n");
    REQUIRE(result[4].original_text == "-");
    REQUIRE_FALSE(result[4].is_option);
    REQUIRE(result[5].original_text == "cmd2");
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
    example.set_custom_strings(".", "<>", ":::", "+++", "->");
    auto result = example.parse(":::help.<>nf.:::output->file.+++.<>a.:::version.'hello.world'");

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

    result = example.parse("cmd1.:::.cmd2");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "cmd1");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == ":::");
    REQUIRE_FALSE(result[1].is_option);
    REQUIRE(result[2].original_text == "cmd2");
    REQUIRE_FALSE(result[2].is_option);

    result = example.parse("cmd1.<>.cmd2");
    REQUIRE(result.size() == 3);
    REQUIRE(result[0].original_text == "cmd1");
    REQUIRE_FALSE(result[0].is_option);
    REQUIRE(result[1].original_text == "<>");
    REQUIRE_FALSE(result[1].is_option);
    REQUIRE(result[2].original_text == "cmd2");
    REQUIRE_FALSE(result[2].is_option);
  }

  SECTION("bound variables") {
    REQUIRE_FALSE(data.help);
    REQUIRE_FALSE(data.version);
    REQUIRE_FALSE(data.verbose);
    REQUIRE(data.file == "");
    REQUIRE_FALSE(data.has_file);
    REQUIRE_FALSE(data.force);
    REQUIRE_FALSE(data.line_nos);

    example.parse("command --help -no output.txt");
    REQUIRE(data.help);
    REQUIRE(data.line_nos);
    REQUIRE(data.has_file);
    REQUIRE(data.file == "output.txt");
    REQUIRE_FALSE(data.force);
    REQUIRE_FALSE(data.version);
    REQUIRE_FALSE(data.verbose);

    example.parse("--indent=4 command -fn");
    REQUIRE(data.indent == 4);
    REQUIRE(data.force);
    REQUIRE(data.line_nos);
  }

  SECTION("type errors") {
    struct settings_ex {
      double temperature;
      int net_worth;
    } data_ex;
    example.add_option().short_name('t').long_name("temperature")
      .bind_double(&data_ex.temperature);
    example.add_option().long_name("net-worth").bind_int(&data_ex.net_worth);

    example.parse("--indent=13");
    REQUIRE(data.indent == 13);
    REQUIRE_THROWS_WITH(example.parse("--indent=-32"),
                        "argument for option '--indent' must not be negative");
    REQUIRE_THROWS_WITH(example.parse("--indent=two"),
                        "argument for option '--indent' must be an integer");
    REQUIRE_THROWS_WITH(example.parse("--indent=2.5"),
                        "argument for option '--indent' must be an integer");

    example.parse("command --net-worth=-48 -t=-12.08e+4");
    REQUIRE(data_ex.net_worth == -48);
    REQUIRE(data_ex.temperature == Approx(-12.08e4));

    example.parse("command --net-worth 0 -t 2.559e-4");
    REQUIRE(data_ex.net_worth == 0);
    REQUIRE(data_ex.temperature == Approx(2.559e-4));

    REQUIRE_THROWS_WITH(example.parse("--net-worth=-0.9"),
                        "argument for option '--net-worth' must be an integer");
    REQUIRE_THROWS_WITH(example.parse("--net-worth=5billion -fn"),
                        "argument for option '--net-worth' must be an integer");
    REQUIRE_THROWS_WITH(example.parse("-t=100f"),
                        "argument for option '-t' must be a number");
    REQUIRE_THROWS_WITH(example.parse("-t xxx"),
                        "argument for option '-t' must be a number");
  }

  SECTION("help message") {
    std::ostringstream oss;
    oss << empty;
    REQUIRE(oss.str() == "");

    std::string desired = R"(  -?, --help                  Show help information
      --version               Get version info
  -v, --verbose               Show verbose output
  -a, --all                   Show all lines
  -f, --force                 Force file creation

Output options
  -o, --output=FILE           Write output to FILE
  -n                          Show line numbers
      --indent[=WIDTH]        Indent each line by WIDTH spaces (default: 2)
  -c, --color[=COLOR]         Set the color of the output
)";
    oss << example;
    REQUIRE(oss.str() == desired);

    desired = R"(        -?, --help  Show help
                      information
            --version
                    Get version info
        -v, --verbose
                    Show verbose output
        -a, --all   Show all lines
        -f, --force
                    Force file creation

    Output options
        -o, --output=FILE
                    Write output to FILE
        -n          Show line numbers
            --indent[=WIDTH]
                    Indent each line by
                      WIDTH spaces
                      (default: 2)
        -c, --color[=COLOR]
                    Set the color of the
                      output
)";
    oss.str("");
    example.print_help(oss, 40, 4, 8, 20, 22);
    REQUIRE(oss.str() == desired);

    desired = R"(-?, --help
Show help information
    --version
Get version info
-v, --verbose
Show verbose output
-a, --all
Show all lines
-f, --force
Force file creation

Output options
-o, --output=FILE
Write output to FILE
-n
Show line numbers
    --indent[=WIDTH]
Indent each line by WIDTH spaces (default: 2)
-c, --color[=COLOR]
Set the color of the output
)";
    oss.str("");
    example.print_help(oss, 0, 0, 0, 0, 0);
    REQUIRE(oss.str() == desired);

    desired = R"(  -?, --help                                                Show help
                                                          information
      --version                                             Get version info
  -v, --verbose                                             Show verbose output
  -a, --all                                                 Show all lines
  -f, --force                                               Force file creation

Output options
  -o, --output=FILE                                         Write output to FILE
  -n                                                        Show line numbers
      --indent[=WIDTH]                                      Indent each line by
                                                          WIDTH spaces (default:
                                                          2)
  -c, --color[=COLOR]                                       Set the color of the
                                                          output
)";
    oss.str("");
    example.print_help(oss, 80, 0, 2, 60, 58);
    REQUIRE(oss.str() == desired);
  }
}
