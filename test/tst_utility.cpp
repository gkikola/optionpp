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

#include <iterator>
#include <string>
#include <vector>
#include <catch2/catch.hpp>
#include "../src/utility.hpp"

using std::back_inserter;
using std::string;
using std::vector;
using namespace optionpp::utility;

TEST_CASE("utility::split") {
  vector<string> output;

  SECTION("empty string") {
    split("", back_inserter(output));
    REQUIRE(output.empty());

    split("", back_inserter(output), "", "");
    REQUIRE(output.empty());

    split("", back_inserter(output), " abcdef", "xyz");
    REQUIRE(output.empty());
  }

  SECTION("single delimiter") {
    split("The quick brown fox", back_inserter(output), " ");
    REQUIRE(output.size() == 4);
    REQUIRE(output[0] == "The");
    REQUIRE(output[1] == "quick");
    REQUIRE(output[2] == "brown");
    REQUIRE(output[3] == "fox");

    output.clear();
    split("banana", back_inserter(output), "a");
    REQUIRE(output.size() == 3);
    REQUIRE(output[0] == "b");
    REQUIRE(output[1] == "n");
    REQUIRE(output[2] == "n");
  }

  SECTION("multiple delimiters") {
    split(" this\tis\na test\tsentence\n", back_inserter(output), " \t\n");
    REQUIRE(output.size() == 5);
    REQUIRE(output[0] == "this");
    REQUIRE(output[1] == "is");
    REQUIRE(output[2] == "a");
    REQUIRE(output[3] == "test");
    REQUIRE(output[4] == "sentence");
  }

  SECTION("repeated delimiters") {
    split("    hello\t\t\tthere     world    ", back_inserter(output), " \t\n\r");
    REQUIRE(output.size() == 3);
    REQUIRE(output[0] == "hello");
    REQUIRE(output[1] == "there");
    REQUIRE(output[2] == "world");
  }

  SECTION("quotes") {
    split("arg1 \"argument number 2\" arg3 \'argument number 4\' arg5", back_inserter(output));
    REQUIRE(output.size() == 5);
    REQUIRE(output[0] == "arg1");
    REQUIRE(output[1] == "argument number 2");
    REQUIRE(output[2] == "arg3");
    REQUIRE(output[3] == "argument number 4");
    REQUIRE(output[4] == "arg5");

    output.clear();
    split("words:Xmore wordsX:@even more: words@:words", back_inserter(output),
          ": ", "X@");
    REQUIRE(output.size() == 4);
    REQUIRE(output[0] == "words");
    REQUIRE(output[1] == "more words");
    REQUIRE(output[2] == "even more: words");
    REQUIRE(output[3] == "words");
  }

  SECTION("empty delimiters") {
    split("The quick brown fox", back_inserter(output), "");
    REQUIRE(output.size() == 1);
    REQUIRE(output[0] == "The quick brown fox");
  }

  SECTION("empty quotes") {
    split("Ask yourself, \"how do I work this?\"", back_inserter(output), " ,", "");
    REQUIRE(output.size() == 7);
    REQUIRE(output[0] == "Ask");
    REQUIRE(output[1] == "yourself");
    REQUIRE(output[2] == "\"how");
    REQUIRE(output[3] == "do");
    REQUIRE(output[4] == "I");
    REQUIRE(output[5] == "work");
    REQUIRE(output[6] == "this?\"");
  }

  SECTION("escape characters") {
    split(R"(Hello, "Here is a quote:\"" and 'another \'quote\' within a quote')",
          back_inserter(output), ", ", "\"\'", '\\');
    REQUIRE(output.size() == 4);
    REQUIRE(output[0] == "Hello");
    REQUIRE(output[1] == "Here is a quote:\"");
    REQUIRE(output[2] == "and");
    REQUIRE(output[3] == "another 'quote' within a quote");

    output.clear();
    split("'Escape char: \\\\' arg2 arg3", back_inserter(output), " ", "\"\'", '\\');
    REQUIRE(output.size() == 3);
    REQUIRE(output[0] == "Escape char: \\");
    REQUIRE(output[1] == "arg2");
    REQUIRE(output[2] == "arg3");
  }

  SECTION("escaping delimiters") {
    split(R"(this\ is\ a\ single\ token)", back_inserter(output));
    REQUIRE(output.size() == 1);
    REQUIRE(output[0] == "this is a single token");

    output.clear();
    split(R"(another\ \"single\ token\")", back_inserter(output));
    REQUIRE(output.size() == 1);
    REQUIRE(output[0] == "another \"single token\"");
  }

  SECTION("mid-token quote") {
    split("arg1 argument\" number \"two arg3", back_inserter(output));
    REQUIRE(output.size() == 3);
    REQUIRE(output[0] == "arg1");
    REQUIRE(output[1] == "argument number two");
    REQUIRE(output[2] == "arg3");

    output.clear();
    split("abc\"def\"gh", back_inserter(output));
    REQUIRE(output.size() == 1);
    REQUIRE(output[0] == "abcdefgh");
  }
}
