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
#include "../src/parser_result.hpp"

using namespace optionpp;

TEST_CASE("parser_result") {
  parser_result result;

  parser_result::item version { "--version", true, "version", '\0', "" };
  parser_result::item help { "-?", true, "help", '?', "" };
  parser_result::item file { "-f myfile.txt", true, "file", 'f', "myfile.txt" };
  parser_result::item non_option { "command", false, "", '\0', "" };

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

    std::vector<parser_result::item> items;
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

    parser_result::item non_opt2{"another command", false, "", '\0', ""};
    result3.push_back(std::move(non_opt2));
    REQUIRE_FALSE(result3.empty());
    REQUIRE(result3.size() == 4);
  }
}
