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

#include <catch2/catch.hpp>
#include <optionpp/parser.hpp>
#include <optionpp/result_iterator.hpp>

using namespace optionpp;

TEST_CASE("result_iterator") {
  parser p;
  p["help"].short_name('?')
    .description("Output a usage message and exit.");
  p['V'].long_name("version")
    .description("Output the version number and exit.");
  p["extended-regexp"].short_name('E')
    .description("Interpret patterns as extended regular expressions");
  p["regexp"].short_name('e').argument("PATTERNS", true)
    .description("Use PATTERNS as the patterns.");
  p['f'].long_name("file").argument("FILE", true)
    .description("Obtain PATTERNS from FILE.");
  p["ignore-case"].short_name('i')
    .description("Ignore case distinctions in patterns.");

  SECTION("non_option_iterator") {
    auto result = p.parse("cmd1 -Eif file.txt cmd2 --version cmd3");
    auto it = non_option_iterator{result};
    auto it_end = end(it);
    REQUIRE_THROWS_WITH(*it_end, "tried to dereference a nullptr");
    REQUIRE(it->original_text == "cmd1");
    REQUIRE_FALSE(it->is_option);
    REQUIRE((it++)->original_text == "cmd1");
    REQUIRE(it->original_text == "cmd2");
    REQUIRE_FALSE(it->is_option);
    REQUIRE((it++)->original_text == "cmd2");
    REQUIRE(it->original_text == "cmd3");
    REQUIRE_FALSE(it->is_option);
    REQUIRE((it++)->original_text == "cmd3");
    REQUIRE(it == it_end);
    REQUIRE(it_end == it);
    REQUIRE(it_end == it_end);
    REQUIRE(it == it);
    REQUIRE_THROWS_WITH(*it, "tried to dereference past-the-end iterator");

    REQUIRE((it--) == it_end);
    REQUIRE(it->original_text == "cmd3");
    REQUIRE((it--)->original_text == "cmd3");
    REQUIRE(it->original_text == "cmd2");
    REQUIRE((--it)->original_text == "cmd1");
  }

  SECTION("non_option_const_iterator") {
    const auto result = p.parse("--file output.txt command another command --ignore-case");
    auto it = non_option_const_iterator{result};
    auto it_end = end(it);
    REQUIRE_THROWS_WITH(*it_end, "tried to dereference a nullptr");
    REQUIRE(it != it_end);
    REQUIRE(it->original_text == "command");
    REQUIRE_FALSE(it->is_option);
    REQUIRE((++it)->original_text == "another");
    REQUIRE_FALSE(it->is_option);
    REQUIRE((++it)->original_text == "command");
    REQUIRE_FALSE(it->is_option);
    REQUIRE(++it == it_end);
    REQUIRE(it == it_end);
    REQUIRE(it_end == it);
    REQUIRE(it_end == it_end);
    REQUIRE(it == it);
    REQUIRE_THROWS_WITH(*it, "tried to dereference past-the-end iterator");
    REQUIRE((--it)->original_text == "command");
    REQUIRE(it != it_end);
    REQUIRE(it_end != it);
  }

  SECTION("option_iterator") {
    auto result = p.parse("cmd1 -Eif file.txt cmd2 --version cmd3");
    auto it = option_iterator{result};
    auto it_end = end(it);
    REQUIRE_THROWS_WITH(*it_end, "tried to dereference a nullptr");
    REQUIRE(it != it_end);
    REQUIRE(it->original_text == "-E");
    REQUIRE((++it)->original_text == "-i");
    REQUIRE((++it)->original_text == "-f file.txt");
    REQUIRE((++it)->original_text == "--version");
    REQUIRE(++it == it_end);
    REQUIRE_THROWS_WITH(*it, "tried to dereference past-the-end iterator");
  }

  SECTION("option_const_iterator") {
    const auto result = p.parse("--file output.txt command another command --ignore-case");
    auto it = option_const_iterator{result};
    auto it_end = end(it);
    REQUIRE_THROWS_WITH(*it_end, "tried to dereference a nullptr");
    REQUIRE(it != it_end);
    REQUIRE(it->original_text == "--file output.txt");
    REQUIRE((++it)->original_text == "--ignore-case");
    REQUIRE(++it == it_end);
    REQUIRE_THROWS_WITH(*it, "tried to dereference past-the-end iterator");
    REQUIRE(--it != it_end);
    REQUIRE(it->original_text == "--ignore-case");
    REQUIRE((it--)->original_text == "--ignore-case");
    REQUIRE(it->original_text == "--file output.txt");
  }
}
