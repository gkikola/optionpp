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
  example.add_option().long_name("file").short_name('f')
    .argument("FILE", argument_type::required)
    .description("Display contents of FILE").group("File options");
  example.add_option().short_name('n').description("Show line numbers")
    .group("File options");
  example.add_option().long_name("all").short_name('a')
    .description("Show all lines").group("Display options");
  example.add_option().long_name("indent")
    .argument("WIDTH")
    .description("Indent each line by WIDTH spaces (default: 2)")
    .group("Display options");
  example.add_option().long_name("force").short_name('f')
    .description("Force file creation").group("File options");

  SECTION("constructors") {
    REQUIRE_NOTHROW(parser{});
  }
}
