/* Option++ -- read command-line program options
   Copyright (C) 2017-2018 Greg Kikola.

   This file is part of Option++.

   Option++ is free software: you can redistribute it and/or modify
   it under the terms of the Boost Software License version 1.0.

   Option++ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   Boost Software License for more details.

   You should have received a copy of the Boost Software License
   along with Option++.  If not, see
   <https://www.boost.org/LICENSE_1_0.txt>.
 */

/* Written by Greg Kikola <gkikola@gmail.com>. */

#include <cstring>
#include <iostream>
#include "gtest/gtest.h"
#include "../src/optionpp/optionpp.hpp"

using optionpp::OptionParser;
using optionpp::Option;
using optionpp::OptionDesc;
using optionpp::BadOption;
using optionpp::BadOptionArgument;

class OptionParserTest : public ::testing::Test {
public:
  OptionParserTest();
protected:
  void parse(OptionParser& parser,
             const std::vector<std::string>& args);
  OptionParser sm_parser;
  OptionParser md_parser;
  OptionParser lg_parser;
  OptionParser copy;
};

OptionParserTest::OptionParserTest():
  sm_parser({
      {'a', "all", "", "list all files"},
      {'s', "sort", "", "sort files in list"},
      {'v', "verbose", "", "verbose mode"},
      {0, "version", "", "display program version"},
      {'?', "help", "", "display help text"}
      })
{
  md_parser.add({'?', "help", "", "display help text"});
  md_parser.add({ {0, "version", "", "display program version"},
        {'v', "verbose", "", "verbose mode"},
          {'f', "force", "", "write file even if it exists"} });
  md_parser.add('a', "all", "", "list all files", 1);
  md_parser.add('A', "almost-all", "", "do not list . and ..");
  md_parser.add(0, "block-size", "SIZE", "scale sizes by SIZE");

  lg_parser.add('x', "nonexistent", "blank", "overwritten");
  lg_parser = md_parser;
  lg_parser.add({
      {'b', "buffer", "N", "buffer size for each file"},
      {'B', "auto-buffers", "", "buffers allocated automatically"},
      {'c', "clear-screen", "", "clear screen on each repaint"},
      {'d', "dumb", "", "suppress error message if terminal is dumb"},
      {0, "color", "COLOR", "set color of text displayed"},
      {'e', "quit-at-eof", "", "automatically exit when end-of-file "
          "is reached"},
      {'h', "max-back-scroll", "N", "maximum number of lines to scroll "
          "backward"},
      {'i', "ignore-case", "", "searches ignore case"},
      {'I', "IGNORE-CASE", "", "really really ignores case"},
      {'n', "line-numbers", "", "show line numbers"},
      {'p', "pattern", "PATTERN", "start at first occurrence of "
          "PATTERN"},
      {'P', "", "PROMPT", "use custom prompt", 0, true},
      {'q', "quiet", "", "quiet mode, do not ring terminal bell"},
      {'s', "", "", "squeeze consecutive blank lines into one"},
      {'S', "", "", "chop long lines"},
      {'t', "tag", "TAG", "edit file containing tag TAG", 0, true},
      {'u', "underline-special", "", "underline special characters"},
      {'z', "window", "N", "change default scrolling window to N lines"}
    });
}

void OptionParserTest::parse(OptionParser& parser,
                             const std::vector<std::string>& args)
{
  int argc = args.size();
  char** argv = new char*[argc + 1];
  for (int i = 0; i < argc; ++i) {
    argv[i] = new char[args[i].size() + 1];
    std::strcpy(argv[i], args[i].c_str());
  }
  argv[argc] = nullptr;

  parser.parse(argc, argv);

  for (int i = 0; i < argc; ++i)
    delete[] argv[i];
  delete[] argv;
}

TEST_F(OptionParserTest, BadLookup) {
  EXPECT_EQ(nullptr, md_parser.lookup('V'));
  EXPECT_EQ(nullptr, md_parser.lookup("unknown"));
  EXPECT_EQ(nullptr, sm_parser.lookup("vERSION"));
  EXPECT_EQ(nullptr, lg_parser.lookup('r'));
}

TEST_F(OptionParserTest, Lookup) {
  OptionDesc* od;
  od = sm_parser.lookup('?');
  EXPECT_EQ("help", od->long_name);
  EXPECT_EQ("", od->argument_name);

  od = md_parser.lookup("version");
  EXPECT_EQ(0, od->short_name);
  EXPECT_EQ("version", od->long_name);

  od = lg_parser.lookup('p');
  EXPECT_EQ("pattern", od->long_name);
  EXPECT_EQ("PATTERN", od->argument_name);

  od = lg_parser.lookup("line-numbers");
  EXPECT_EQ('n', od->short_name);
  EXPECT_EQ("line-numbers", od->long_name);
  EXPECT_EQ("show line numbers", od->description);
}

TEST_F(OptionParserTest, NoArgs) {
  parse(sm_parser, {"prog"});

  EXPECT_EQ("prog", sm_parser.program_cmd());
  EXPECT_EQ(0, sm_parser.program_args().size());
  EXPECT_EQ(0, sm_parser.size());
  EXPECT_EQ(true, sm_parser.empty());
  EXPECT_EQ(sm_parser.end(), sm_parser.begin());
}

TEST_F(OptionParserTest, NoOptions) {
  parse(sm_parser, {"prog", "arg1", "arg2", "arg3", "arg4"});

  EXPECT_EQ("prog", sm_parser.program_cmd());
  EXPECT_EQ(0, sm_parser.size());
  EXPECT_EQ(true, sm_parser.empty());
  EXPECT_EQ(sm_parser.end(), sm_parser.begin());

  EXPECT_EQ(4, sm_parser.program_args().size());
  auto it = sm_parser.program_args().begin();
  EXPECT_EQ("arg1", *it++);
  EXPECT_EQ("arg2", *it++);
  EXPECT_EQ("arg3", *it++);
  EXPECT_EQ("arg4", *it++);
  EXPECT_EQ(sm_parser.program_args().end(), it);
}

TEST_F(OptionParserTest, NoOptionHyphen) {
  parse(lg_parser, {"prog", "-", "blank"});

  EXPECT_EQ(true, lg_parser.empty());
  EXPECT_EQ(2, lg_parser.program_args().size());
  auto it = lg_parser.program_args().begin();
  EXPECT_EQ("-", *it++);
  EXPECT_EQ("blank", *it++);
  EXPECT_EQ(lg_parser.program_args().end(), it);
}

TEST_F(OptionParserTest, Options) {
  parse(lg_parser, {"prog", "-BiuqS", "--line-numbers", "clear-screen", "-I"});

  EXPECT_EQ(7, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("auto-buffers", (it++)->long_name);
  EXPECT_EQ("ignore-case", (it++)->long_name);
  EXPECT_EQ("underline-special", (it++)->long_name);
  EXPECT_EQ("quiet", (it++)->long_name);
  EXPECT_EQ('S', (it++)->short_name);
  EXPECT_EQ("line-numbers", (it++)->long_name);
  EXPECT_EQ("IGNORE-CASE", (it++)->long_name);
  EXPECT_EQ(lg_parser.end(), it);

  EXPECT_EQ("prog", lg_parser.program_cmd());
  auto arg = lg_parser.program_args().begin();
  EXPECT_EQ(1, lg_parser.program_args().size());
  EXPECT_EQ("clear-screen", *arg++);
  EXPECT_EQ(lg_parser.program_args().end(), arg);
}

TEST_F(OptionParserTest, CopyCtor) {
  OptionParser copy = lg_parser;

  EXPECT_EQ(0, copy.size());
  EXPECT_EQ(true, copy.empty());

  OptionDesc* od = nullptr;
  od = copy.lookup('p');
  EXPECT_EQ("pattern", od->long_name);
  EXPECT_EQ("PATTERN", od->argument_name);

  od = copy.lookup("line-numbers");
  EXPECT_EQ('n', od->short_name);
  EXPECT_EQ("line-numbers", od->long_name);
  EXPECT_EQ("show line numbers", od->description);

  parse(copy, {"prog", "-BiuqS", "--line-numbers", "clear-screen", "-I"});

  EXPECT_EQ(7, copy.size());
  EXPECT_EQ(false, copy.empty());
  auto it = copy.begin();
  EXPECT_EQ("auto-buffers", (it++)->long_name);
  EXPECT_EQ("ignore-case", (it++)->long_name);
  EXPECT_EQ("underline-special", (it++)->long_name);
  EXPECT_EQ("quiet", (it++)->long_name);
  EXPECT_EQ('S', (it++)->short_name);
  EXPECT_EQ("line-numbers", (it++)->long_name);
  EXPECT_EQ("IGNORE-CASE", (it++)->long_name);
  EXPECT_EQ(copy.end(), it);

  EXPECT_EQ("prog", copy.program_cmd());
  auto arg = copy.program_args().begin();
  EXPECT_EQ(1, copy.program_args().size());
  EXPECT_EQ("clear-screen", *arg++);
  EXPECT_EQ(copy.program_args().end(), arg);
}

TEST_F(OptionParserTest, CopyAssignment) {
  OptionParser copy = sm_parser;

  EXPECT_EQ(0, copy.size());
  EXPECT_EQ(true, copy.empty());

  OptionDesc* od = nullptr;
  od = copy.lookup('p');
  EXPECT_EQ(nullptr, od);

  od = copy.lookup("sort");
  EXPECT_EQ('s', od->short_name);
  EXPECT_EQ("sort", od->long_name);
  EXPECT_EQ("sort files in list", od->description);

  parse(lg_parser, {"prog", "-BiuqS", "--line-numbers", "clear-screen", "-I"});

  copy = lg_parser;

  od = copy.lookup('p');
  EXPECT_EQ("pattern", od->long_name);
  EXPECT_EQ("PATTERN", od->argument_name);

  od = copy.lookup("sort");
  EXPECT_EQ(nullptr, od);

  EXPECT_EQ(7, copy.size());
  EXPECT_EQ(false, copy.empty());
  auto it = copy.begin();
  EXPECT_EQ("auto-buffers", (it++)->long_name);
  EXPECT_EQ("ignore-case", (it++)->long_name);
  EXPECT_EQ("underline-special", (it++)->long_name);
  EXPECT_EQ("quiet", (it++)->long_name);
  EXPECT_EQ('S', (it++)->short_name);
  EXPECT_EQ("line-numbers", (it++)->long_name);
  EXPECT_EQ("IGNORE-CASE", (it++)->long_name);
  EXPECT_EQ(copy.end(), it);

  EXPECT_EQ("prog", copy.program_cmd());
  auto arg = copy.program_args().begin();
  EXPECT_EQ(1, copy.program_args().size());
  EXPECT_EQ("clear-screen", *arg++);
  EXPECT_EQ(copy.program_args().end(), arg);
}

TEST_F(OptionParserTest, OptionsWithArgsEq) {
  parse(lg_parser, {"prog", "--max-back-scroll=12", "-ep=42",
        "-P=custom prompt", "--buffer=10", "--color=red"});

  EXPECT_EQ(6, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("max-back-scroll", it->long_name);
  EXPECT_EQ("12", it->argument);
  ++it;

  EXPECT_EQ("quit-at-eof", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("42", it->argument);
  ++it;

  EXPECT_EQ('P', it->short_name);
  EXPECT_EQ("custom prompt", it->argument);
  ++it;

  EXPECT_EQ("buffer", it->long_name);
  EXPECT_EQ("10", it->argument);
  ++it;

  EXPECT_EQ("color", it->long_name);
  EXPECT_EQ("red", it->argument);
  ++it;

  EXPECT_EQ(lg_parser.end(), it);

  it = lg_parser.find("buffer");
  EXPECT_EQ("buffer", it->long_name);
  EXPECT_EQ("10", it->argument);
  it = lg_parser.find("boffer");
  EXPECT_EQ(lg_parser.end(), it);

  it = lg_parser.find('e');
  EXPECT_EQ("quit-at-eof", it->long_name);
  it = lg_parser.find('i');
  EXPECT_EQ(lg_parser.end(), it);
}

TEST_F(OptionParserTest, OptionsWithArgsNoSpace) {
  parse(lg_parser, {"prog", "-h12", "-ep42",
        "-Pcustom", "-b10", "-ptest"});

  EXPECT_EQ(6, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("max-back-scroll", it->long_name);
  EXPECT_EQ("12", it->argument);
  ++it;

  EXPECT_EQ("quit-at-eof", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("42", it->argument);
  ++it;

  EXPECT_EQ('P', it->short_name);
  EXPECT_EQ("custom", it->argument);
  ++it;

  EXPECT_EQ("buffer", it->long_name);
  EXPECT_EQ("10", it->argument);
  ++it;

  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("test", it->argument);
  ++it;

  EXPECT_EQ(lg_parser.end(), it);
}

TEST_F(OptionParserTest, OptionsWithArgsSep) {
  parse(lg_parser, {"prog", "--max-back-scroll", "12", "-ep", "42",
        "-P", "custom prompt", "--buffer=", "10", "--color", "red"});

  EXPECT_EQ(6, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("max-back-scroll", it->long_name);
  EXPECT_EQ("12", it->argument);
  ++it;

  EXPECT_EQ("quit-at-eof", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("42", it->argument);
  ++it;

  EXPECT_EQ('P', it->short_name);
  EXPECT_EQ("custom prompt", it->argument);
  ++it;

  EXPECT_EQ("buffer", it->long_name);
  EXPECT_EQ("10", it->argument);
  ++it;

  EXPECT_EQ("color", it->long_name);
  EXPECT_EQ("red", it->argument);
  ++it;

  EXPECT_EQ(lg_parser.end(), it);
}

TEST_F(OptionParserTest, OptionsWithHyphenArg) {
  parse(lg_parser, {"prog", "-p", "-"});

  EXPECT_EQ(1, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("-", it->argument);
  ++it;
  EXPECT_EQ(lg_parser.end(), it);

  parse(lg_parser, {"prog", "--pattern", "-", "-e"});

  EXPECT_EQ(2, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  it = lg_parser.begin();
  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("-", it->argument);
  ++it;
  EXPECT_EQ("quit-at-eof", it->long_name);
  ++it;
  EXPECT_EQ(lg_parser.end(), it);
}

TEST_F(OptionParserTest, OptionsWithOptionalArgs) {
  parse(lg_parser, {"prog", "--max-back-scroll", "12", "-ep", "42",
        "-P", "--buffer", "10", "-qt", "--color", "red"});

  EXPECT_EQ(8, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto it = lg_parser.begin();
  EXPECT_EQ("max-back-scroll", it->long_name);
  EXPECT_EQ("12", it->argument);
  ++it;

  EXPECT_EQ("quit-at-eof", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("pattern", it->long_name);
  EXPECT_EQ("42", it->argument);
  ++it;

  EXPECT_EQ('P', it->short_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("buffer", it->long_name);
  EXPECT_EQ("10", it->argument);
  ++it;

  EXPECT_EQ("quiet", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("tag", it->long_name);
  EXPECT_EQ("", it->argument);
  ++it;

  EXPECT_EQ("color", it->long_name);
  EXPECT_EQ("red", it->argument);
  ++it;

  EXPECT_EQ(lg_parser.end(), it);
}

TEST_F(OptionParserTest, EndOfOptions) {
  parse(lg_parser, {"prog", "--max-back-scroll", "12", "--", "-ep", "42",
        "-P", "--buffer", "10", "--tag", "--color", "red"});

  EXPECT_EQ(1, lg_parser.size());
  EXPECT_EQ(false, lg_parser.empty());
  auto opt = lg_parser.begin();
  EXPECT_EQ("max-back-scroll", opt->long_name);
  EXPECT_EQ("12", opt->argument);
  ++opt;
  EXPECT_EQ(opt, lg_parser.end());

  EXPECT_EQ(8, lg_parser.program_args().size());
  auto it = lg_parser.program_args().begin();
  EXPECT_EQ("-ep", *it++);
  EXPECT_EQ("42", *it++);
  EXPECT_EQ("-P", *it++);
  EXPECT_EQ("--buffer", *it++);
  EXPECT_EQ("10", *it++);
  EXPECT_EQ("--tag", *it++);
  EXPECT_EQ("--color", *it++);
  EXPECT_EQ("red", *it++);
  EXPECT_EQ(it, lg_parser.program_args().end());
}

TEST_F(OptionParserTest, BadOptions) {
  EXPECT_THROW(parse(lg_parser, {"prog", "-Bcdjei"}), BadOption);
  EXPECT_THROW(parse(lg_parser, {"prog", "--error"}), BadOption);
  EXPECT_THROW(parse(lg_parser, {"prog", "-eid", "--color=red", "--throw",
          "--window=16"}), BadOption);
}

TEST_F(OptionParserTest, BadOptionArgs) {
  EXPECT_THROW(parse(lg_parser, {"prog", "-c=red", "--pattern", "-q"}),
               BadOptionArgument);
  EXPECT_THROW(parse(lg_parser, {"prog", "-c=red", "--pattern", "--",
          "three"}), BadOptionArgument);
  EXPECT_THROW(parse(lg_parser, {"prog", "--pattern"}), BadOptionArgument);
  EXPECT_NO_THROW(parse(lg_parser, {"prog", "--tag"}));
  EXPECT_THROW(parse(lg_parser, {"prog", "-cinp"}), BadOptionArgument);
  EXPECT_THROW(parse(lg_parser, {"prog", "-cinp", "--", "hello"}),
               BadOptionArgument);
  EXPECT_THROW(parse(lg_parser, {"prog", "-cinp="}), BadOptionArgument);
  EXPECT_THROW(parse(lg_parser, {"prog", "--pattern="}), BadOptionArgument);
}

TEST(ArgConversion, ArgToInt) {
  Option o = {'n', "number", "42"};
  EXPECT_EQ(42, o.arg_to_int());

  o.argument = "-57";
  EXPECT_EQ(-57, o.arg_to_int());

  o.argument = "0";
  EXPECT_EQ(0, o.arg_to_int());

  o.argument = "  37  ";
  EXPECT_EQ(37, o.arg_to_int());

  o.argument = "37f";
  EXPECT_THROW(o.arg_to_int(), BadOptionArgument);

  o.argument = "42.57";
  EXPECT_THROW(o.arg_to_int(), BadOptionArgument);
}

TEST(ArgConversion, ArgToUnsigned) {
  Option o = {'n', "number", "42"};
  EXPECT_EQ(42, o.arg_to_unsigned());

  o.argument = "-57";
  EXPECT_THROW(o.arg_to_unsigned(), BadOptionArgument);

  o.argument = "0";
  EXPECT_EQ(0, o.arg_to_unsigned());

  o.argument = "  37  ";
  EXPECT_EQ(37, o.arg_to_unsigned());

  o.argument = "37f";
  EXPECT_THROW(o.arg_to_unsigned(), BadOptionArgument);

  o.argument = "42.57";
  EXPECT_THROW(o.arg_to_unsigned(), BadOptionArgument);
}

TEST(ArgConversion, ArgToLong) {
  Option o = {'n', "number", "42"};
  EXPECT_EQ(42L, o.arg_to_long());

  o.argument = "-57";
  EXPECT_EQ(-57L, o.arg_to_long());

  o.argument = "0";
  EXPECT_EQ(0L, o.arg_to_long());

  o.argument = "  37  ";
  EXPECT_EQ(37L, o.arg_to_long());

  o.argument = "37f";
  EXPECT_THROW(o.arg_to_long(), BadOptionArgument);

  o.argument = "42.57";
  EXPECT_THROW(o.arg_to_long(), BadOptionArgument);
}

TEST(ArgConversion, ArgToDouble) {
  Option o = {'n', "number", "42"};
  EXPECT_EQ(42.0, o.arg_to_double());

  o.argument = "-57";
  EXPECT_EQ(-57.0, o.arg_to_double());

  o.argument = "0";
  EXPECT_EQ(0.0, o.arg_to_double());

  o.argument = "  37  ";
  EXPECT_EQ(37.0, o.arg_to_double());

  o.argument = "37.519";
  EXPECT_FLOAT_EQ(37.519, o.arg_to_double());

  o.argument = "-13.847";
  EXPECT_FLOAT_EQ(-13.847, o.arg_to_double());

  o.argument = "-2.3e4";
  EXPECT_FLOAT_EQ(-2.3e4, o.arg_to_double());

  o.argument = "37f";
  EXPECT_THROW(o.arg_to_double(), BadOptionArgument);
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
