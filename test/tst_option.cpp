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
#include <optionpp/option.hpp>

using namespace optionpp;

TEST_CASE("option") {
  option empty{};
  option short_name_only{'v'};
  option long_name_only{"version"};
  option long_and_short{"version", 'v'};

  option with_argument_req{"file", 'f'};
  with_argument_req.argument("FILE", true);

  option with_argument_opt{"dir", 'd'};
  with_argument_opt.argument("DIRECTORY", false);

  option combo;
  combo.long_name("all").short_name('a').description("show all");

  SECTION("constructors") {
    REQUIRE_NOTHROW(option{});

    REQUIRE(empty.name() == "");
    REQUIRE(empty.long_name() == "");
    REQUIRE(empty.short_name() == '\0');
    REQUIRE(empty.description() == "");
    REQUIRE(empty.argument_name() == "");
    REQUIRE_FALSE(empty.is_argument_required());

    REQUIRE(short_name_only.name() == "v");
    REQUIRE(short_name_only.long_name() == "");
    REQUIRE(short_name_only.short_name() == 'v');
    REQUIRE(short_name_only.description() == "");
    REQUIRE(short_name_only.argument_name() == "");
    REQUIRE_FALSE(short_name_only.is_argument_required());

    REQUIRE(long_name_only.long_name() == "version");
    REQUIRE(long_name_only.long_name() == "version");
    REQUIRE(long_name_only.short_name() == '\0');
    REQUIRE(long_name_only.description() == "");
    REQUIRE(long_name_only.argument_name() == "");
    REQUIRE_FALSE(long_name_only.is_argument_required());

    REQUIRE(long_and_short.name() == "version");
    REQUIRE(long_and_short.long_name() == "version");
    REQUIRE(long_and_short.short_name() == 'v');
    REQUIRE_FALSE(long_and_short.is_argument_required());
  }

  SECTION("setters") {
    REQUIRE(with_argument_req.argument_name() == "FILE");
    REQUIRE(with_argument_req.is_argument_required());
    REQUIRE(with_argument_req.argument_type() == option::string_arg);

    REQUIRE(with_argument_opt.argument_name() == "DIRECTORY");
    REQUIRE_FALSE(with_argument_opt.is_argument_required());
    REQUIRE(with_argument_opt.argument_type() == option::string_arg);

    REQUIRE(combo.name() == "all");
    REQUIRE(combo.long_name() == "all");
    REQUIRE(combo.short_name() == 'a');
    REQUIRE(combo.description() == "show all");

    combo.name("line-numbers", 'n');
    REQUIRE(combo.name() == "line-numbers");
    REQUIRE(combo.long_name() == "line-numbers");
    REQUIRE(combo.short_name() == 'n');

    combo.name("block-size", 'b')
      .argument("SIZE", true);
    REQUIRE(combo.name() == "block-size");
    REQUIRE(combo.long_name() == "block-size");
    REQUIRE(combo.short_name() == 'b');
    REQUIRE(combo.argument_name() == "SIZE");
    REQUIRE(combo.is_argument_required());
  }

  SECTION("variable binding") {
    bool is_set{};
    REQUIRE_FALSE(combo.has_bound_argument_variable());
    combo.bind_bool(&is_set);
    REQUIRE_FALSE(is_set);
    combo.write_bool(true);
    REQUIRE(is_set);
    combo.write_bool(false);
    REQUIRE_FALSE(is_set);
    REQUIRE_FALSE(combo.has_bound_argument_variable());

    std::string str_value;
    combo.bind_string(&str_value);
    REQUIRE(combo.argument_type() == option::string_arg);
    REQUIRE(combo.has_bound_argument_variable());
    combo.write_string("Hello world");
    REQUIRE(str_value == "Hello world");
    REQUIRE_THROWS_WITH(combo.write_int(-37),
                        "option 'all' does not accept an int argument");
    REQUIRE_THROWS_WITH(combo.write_uint(12),
                        "option 'all' does not accept an unsigned int argument");
    REQUIRE_THROWS_WITH(combo.write_double(42.0), "option 'all' does not accept a double argument");

    int ivalue{};
    combo.bind_int(&ivalue);
    REQUIRE(combo.argument_type() == option::int_arg);
    combo.write_int(-109);
    REQUIRE(ivalue == -109);
    REQUIRE_THROWS_WITH(combo.write_string("Hello"),
                        "option 'all' does not accept a string argument");

    unsigned uvalue{};
    combo.bind_uint(&uvalue);
    REQUIRE(combo.argument_type() == option::uint_arg);
    combo.write_uint(42);
    REQUIRE(uvalue == 42);
    REQUIRE_THROWS_WITH(combo.write_double(42.5),
                        "option 'all' does not accept a double argument");

    double dvalue{};
    combo.bind_double(&dvalue);
    REQUIRE(combo.argument_type() == option::double_arg);
    combo.write_double(1.234);
    REQUIRE(dvalue == Approx(1.234));
  }
}
