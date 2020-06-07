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

#include <vector>
#include <catch2/catch.hpp>
#include <optionpp/parser_result.hpp>

using namespace optionpp;

TEST_CASE("parser_result") {
  parser_result result;

  parsed_entry version { "--version", true, "version", '\0', "" };
  parsed_entry help { "-?", true, "help", '?', "" };
  parsed_entry non_option { "command", false, "", '\0', "" };
  parsed_entry file { "-f myfile.txt", true, "file", 'f', "myfile.txt" };
  parsed_entry file_sonly { "-f myfile.txt", true, "", 'f', "myfile.txt" };
  parsed_entry file_lonly { "--file=myfile.txt", false, "file", '\0', "myfile.txt" };

  SECTION("constructors, push_back, size, and empty") {
    result = parser_result{};
    REQUIRE(result.empty());
    REQUIRE(result.size() == 0);

    result.push_back(version);
    REQUIRE_FALSE(result.empty());
    REQUIRE(result.size() == 1);
    result.push_back(help);
    REQUIRE_FALSE(result.empty());
    REQUIRE(result.size() == 2);
    result.push_back(non_option);
    REQUIRE_FALSE(result.empty());
    REQUIRE(result.size() == 3);
    result.push_back(file);
    REQUIRE_FALSE(result.empty());
    REQUIRE(result.size() == 4);

    std::vector<parsed_entry> items;
    items.push_back(version);
    items.push_back(help);
    items.push_back(non_option);
    items.push_back(file);
    parser_result result2{items.begin(), items.end()};
    REQUIRE_FALSE(result2.empty());
    REQUIRE(result2.size() == 4);

    parser_result result3{version, help, file};
    REQUIRE_FALSE(result3.empty());
    REQUIRE(result3.size() == 3);

    parsed_entry non_opt2{"another command", false, "", '\0', ""};
    result3.push_back(std::move(non_opt2));
    REQUIRE_FALSE(result3.empty());
    REQUIRE(result3.size() == 4);
  }

  SECTION("iteration") {
    result = parser_result{version, help, non_option, file};

    // Plain iterator
    auto it = result.begin();
    REQUIRE(it->original_text == "--version");
    ++it;
    REQUIRE(it->original_text == "-?");
    ++it;
    REQUIRE(it->original_text == "command");
    ++it;
    REQUIRE(it->original_text == "-f myfile.txt");
    ++it;
    REQUIRE(it == result.end());

    // const_iterator
    const parser_result cresult = result;
    auto cit = cresult.begin();
    REQUIRE(cit->original_text == "--version");
    ++cit;
    REQUIRE(cit->original_text == "-?");
    ++cit;
    REQUIRE(cit->original_text == "command");
    ++cit;
    REQUIRE(cit->original_text == "-f myfile.txt");
    ++cit;
    REQUIRE(cit == cresult.end());

    // reverse_iterator
    auto rit = result.rbegin();
    REQUIRE(rit->original_text == "-f myfile.txt");
    ++rit;
    REQUIRE(rit->original_text == "command");
    ++rit;
    REQUIRE(rit->original_text == "-?");
    ++rit;
    REQUIRE(rit->original_text == "--version");
    ++rit;
    REQUIRE(rit == result.rend());

    // const_reverse_iterator
    auto crit = cresult.rbegin();
    REQUIRE(crit->original_text == "-f myfile.txt");
    ++crit;
    REQUIRE(crit->original_text == "command");
    ++crit;
    REQUIRE(crit->original_text == "-?");
    ++crit;
    REQUIRE(crit->original_text == "--version");
    ++crit;
    REQUIRE(crit == cresult.rend());
  }

  SECTION("clear") {
    result = parser_result{help, version, non_option, file};

    REQUIRE(result.size() == 4);
    REQUIRE_FALSE(result.empty());

    result.clear();
    REQUIRE(result.size() == 0);
    REQUIRE(result.empty());
    REQUIRE(result.begin() == result.end());

    result.push_back(help);
    REQUIRE(result.size() == 1);
    REQUIRE_FALSE(result.empty());
  }

  SECTION("operator[], at, and back") {
    result = parser_result{version, help, non_option, file};

    REQUIRE(result[0].original_text == "--version");
    REQUIRE(result[1].original_text == "-?");
    REQUIRE(result[2].original_text == "command");
    REQUIRE(result[3].original_text == "-f myfile.txt");
    REQUIRE(result.back().original_text == "-f myfile.txt");

    REQUIRE(result.at(0).original_text == "--version");
    REQUIRE(result.at(1).original_text == "-?");
    REQUIRE(result.at(2).original_text == "command");
    REQUIRE(result.at(3).original_text == "-f myfile.txt");

    REQUIRE_THROWS_AS(result.at(4), out_of_range);
    REQUIRE_THROWS_AS(result.at(5), out_of_range);
    REQUIRE_THROWS_AS(result.at(10), out_of_range);

    const parser_result cresult = result;
    REQUIRE(cresult.at(0).original_text == "--version");
    REQUIRE(cresult.at(1).original_text == "-?");
    REQUIRE(cresult.at(2).original_text == "command");
    REQUIRE(cresult.at(3).original_text == "-f myfile.txt");
    REQUIRE(cresult.back().original_text == "-f myfile.txt");

    REQUIRE_THROWS_AS(cresult.at(4), out_of_range);
    REQUIRE_THROWS_AS(cresult.at(5), out_of_range);
    REQUIRE_THROWS_AS(cresult.at(10), out_of_range);

    parser_result empty = parser_result{};
    REQUIRE_THROWS_AS(empty.back(), out_of_range);

    const parser_result cempty = parser_result{};
    REQUIRE_THROWS_AS(cempty.back(), out_of_range);
  }

  SECTION("is_option_set") {
    result = parser_result{version, help, non_option, file};
    REQUIRE(result.is_option_set("version"));
    REQUIRE(result.is_option_set('?'));
    REQUIRE(result.is_option_set('f'));
    REQUIRE_FALSE(result.is_option_set("verbose"));
    REQUIRE_FALSE(result.is_option_set(""));
    REQUIRE_FALSE(result.is_option_set('\0'));
    REQUIRE_FALSE(result.is_option_set("command"));
  }

  SECTION("get_argument") {
    result = parser_result{version, help, non_option, file};
    REQUIRE(result.get_argument("file") == "myfile.txt");
    REQUIRE(result.get_argument('f') == "myfile.txt");
    REQUIRE(result.get_argument("width") == "");
    REQUIRE(result.get_argument("command") == "");
    REQUIRE(result.get_argument("") == "");
    REQUIRE(result.get_argument('\0') == "");

    result = parser_result{file_sonly};
    REQUIRE(result.get_argument("") == "");
    REQUIRE(result.get_argument('\0') == "");

    result = parser_result{file_lonly};
    REQUIRE(result.get_argument("") == "");
    REQUIRE(result.get_argument('\0') == "");
  }

  SECTION("non_option_iterator") {
    result = parser_result{version, help, non_option, file};
    auto it = non_option_iterator{result};
    REQUIRE(it != end(it));
    REQUIRE(it->original_text == "command");
    ++it;
    REQUIRE(it == end(it));
    --it;
    REQUIRE(it->original_text == "command");
    REQUIRE_THROWS_WITH(--it, "out of bounds parser_result access");

    result = parser_result{version, help, file };
    it = non_option_iterator{result};
    REQUIRE(it == end(it));
  }
}
