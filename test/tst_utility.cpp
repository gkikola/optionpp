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

  SECTION("allow empty") {
    split("\t hey   hello  ", back_inserter(output),
          " \t\n\r", "", '\\', true);
    REQUIRE(output.size() == 8);
    REQUIRE(output[0] == "");
    REQUIRE(output[1] == "");
    REQUIRE(output[2] == "hey");
    REQUIRE(output[3] == "");
    REQUIRE(output[4] == "");
    REQUIRE(output[5] == "hello");
    REQUIRE(output[6] == "");
    REQUIRE(output[7] == "");
  }
}

TEST_CASE("utility::wrap_text") {
  std::string text{"I am the very model of a modern Major-General, I've information vegetable, animal, and mineral, I know the kings of England, and I quote the fights historical, from Marathon to Waterloo, in order categorical."};

  std::string multiline{R"(I'm Nobody! Who are you?
Are you - Nobody - too?
Then there's a pair of us!
Don't tell! they'd advertise - you know!

How dreary - to be - Somebody!
How public - like a Frog -
To tell one's name - the livelong June -
To an admiring Bog!
    - Emily Dickinson)"};

  SECTION("unlimited length") {
    REQUIRE(text == wrap_text(text, -1));
    REQUIRE(multiline == wrap_text(multiline, 0));
  }

  SECTION("limited length") {
    std::string text_wrapped{R"(I am the very model of a modern
Major-General, I've information
vegetable, animal, and mineral, I
know the kings of England, and I
quote the fights historical, from
Marathon to Waterloo, in order
categorical.)"};
    REQUIRE(text_wrapped == wrap_text(text, 33));

    std::string multi_wrapped{R"(I'm Nobody! Who are
you?
Are you - Nobody -
too?
Then there's a pair
of us!
Don't tell! they'd
advertise - you
know!

How dreary - to be -
Somebody!
How public - like a
Frog -
To tell one's name -
the livelong June -
To an admiring Bog!
    - Emily
Dickinson)"};
    REQUIRE(multi_wrapped == wrap_text(multiline, 20));
  }

  SECTION("indent") {
    std::string text_indented{R"(      I am the very model of a modern
      Major-General, I've information vegetable,
      animal, and mineral, I know the kings of
      England, and I quote the fights historical,
      from Marathon to Waterloo, in order
      categorical.)"};
    REQUIRE(text_indented == wrap_text(text, 50, 6));

    std::string multi_indented{R"(  I'm Nobody! Who are you?
  Are you - Nobody - too?
  Then there's a pair of us!
  Don't tell! they'd advertise - you
  know!

  How dreary - to be - Somebody!
  How public - like a Frog -
  To tell one's name - the livelong June
  -
  To an admiring Bog!
      - Emily Dickinson)"};
    REQUIRE(multi_indented == wrap_text(multiline, 40, 2));
  }

  SECTION("first line indent") {
    std::string text_indented{R"(  I am the very model of a modern
     Major-General, I've information
     vegetable, animal, and mineral, I
     know the kings of England, and I
     quote the fights historical, from
     Marathon to Waterloo, in order
     categorical.)"};
    REQUIRE(text_indented == wrap_text(text, 40, 5, 2));

    std::string multi_indented{R"(               I'm
  Nobody! Who are
  you?
  Are you - Nobody -
  too?
  Then there's a
  pair of us!
  Don't tell! they'd
  advertise - you
  know!

  How dreary - to be
  - Somebody!
  How public - like
  a Frog -
  To tell one's name
  - the livelong
  June -
  To an admiring
  Bog!
      - Emily
  Dickinson)"};
    REQUIRE(multi_indented == wrap_text(multiline, 20, 2, 15));
  }

  SECTION("very short length") {
    std::string text_short{R"(I am
the
very
mode
l of
a
mode
rn
Majo
r-Ge
nera
l,
I've
info
rmat
ion
vege
tabl
e,
anim
al,
and
mine
ral,
I
know
the
king
s of
Engl
and,
and
I
quot
e
the
figh
ts
hist
oric
al,
from
Mara
thon
to
Wate
rloo
, in
orde
r
cate
gori
cal.)"};
    REQUIRE(text_short == wrap_text(text, 4));

    std::string multiline_short{R"(I'm
Nobody
! Who
are
you?
Are
you -
Nobody
- too?
Then
there'
s a
pair
of us!
Don't
tell!
they'd
advert
ise -
you
know!

How
dreary
- to
be -
Somebo
dy!
How
public
- like
a Frog
-
To
tell
one's
name -
the
livelo
ng
June -
To an
admiri
ng
Bog!
    -
Emily
Dickin
son)"};
    REQUIRE(multiline_short == wrap_text(multiline, 6));
  }
}

TEST_CASE("utility::is_substr_at_pos") {
  REQUIRE(is_substr_at_pos("Hello world", "wor", 6));
  REQUIRE_FALSE(is_substr_at_pos("Hello world", "wor", 5));
  REQUIRE_FALSE(is_substr_at_pos("Hello world", "wor", 4));
  REQUIRE_FALSE(is_substr_at_pos("Hello world", "wor", 7));
  REQUIRE_FALSE(is_substr_at_pos("Hello world", "wor", 8));
  REQUIRE_FALSE(is_substr_at_pos("Hello world", "wor", 99));

  REQUIRE(is_substr_at_pos("This is not my beautiful house", "", 17));
  REQUIRE(is_substr_at_pos("This is not my beautiful house", "", 0));
  REQUIRE(is_substr_at_pos("Twister", "", 5));
  REQUIRE(is_substr_at_pos("Twister", "", 6));
  REQUIRE(is_substr_at_pos("Twister", "", 7));
  REQUIRE_FALSE(is_substr_at_pos("Twister", "", 8));
  REQUIRE_FALSE(is_substr_at_pos("Twister", "string", 6));
  REQUIRE_FALSE(is_substr_at_pos("Twister", "string", 7));

  REQUIRE(is_substr_at_pos("--version", "--"));
  REQUIRE_FALSE(is_substr_at_pos("--version", "--", 1));
  REQUIRE(is_substr_at_pos("--version", "si", 5));

  REQUIRE_FALSE(is_substr_at_pos("small", "really really big", 2));
  REQUIRE(is_substr_at_pos("small", "small", 0));
}
