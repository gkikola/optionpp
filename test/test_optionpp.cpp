#include <cstring>
#include <iostream>
#include "gtest/gtest.h"
#include "../src/optionpp.hpp"

class OptionParserTest : public ::testing::Test {
public:
  OptionParserTest();
protected:
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
      {'P', "", "PROMPT", "use custom prompt"},
      {'q', "quiet", "", "quiet mode, do not ring terminal bell"},
      {'s', "", "", "squeeze consecutive blank lines into one"},
      {'S', "", "", "chop long lines"},
      {'t', "tag", "TAG", "edit file containing tag TAG"},
      {'u', "underline-special", "", "underline special characters"},
      {'z', "window", "N", "change default scrolling window to N lines"}
    });
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
  char* argv[2] = { new char[5], nullptr };
  std::strcpy(argv[0], "prog");

  sm_parser.parse(1, argv);
  EXPECT_EQ(1, sm_parser.program_args().size());
  EXPECT_EQ(0, sm_parser.size());
  EXPECT_EQ(true, sm_parser.empty());
  EXPECT_EQ(sm_parser.end(), sm_parser.begin());
  
  delete[] argv[0];
}

TEST_F(OptionParserTest, NoOptions) {
  char* argv[6] = { new char[12], new char[12], new char[12],
                    new char[12], new char[12], nullptr };
  std::strcpy(argv[0], "prog");
  std::strcpy(argv[1], "arg1");
  std::strcpy(argv[2], "arg2");
  std::strcpy(argv[3], "arg3");
  std::strcpy(argv[4], "arg4");
  
  sm_parser.parse(5, argv);
  EXPECT_EQ(0, sm_parser.size());
  EXPECT_EQ(true, sm_parser.empty());
  EXPECT_EQ(sm_parser.end(), sm_parser.begin());

  EXPECT_EQ(5, sm_parser.program_args().size());
  auto it = sm_parser.program_args().begin();
  EXPECT_EQ("prog", *it++);
  EXPECT_EQ("arg1", *it++);
  EXPECT_EQ("arg2", *it++);
  EXPECT_EQ("arg3", *it++);
  EXPECT_EQ("arg4", *it++);
  EXPECT_EQ(sm_parser.program_args().end(), it);
  
  for (int i = 0; i < 5; ++i)
    delete[] argv[i];
}

TEST_F(OptionParserTest, Options) {
  char* argv[6] = { new char[12], new char[12], new char[12],
                    new char[12], new char[12], nullptr };
  std::strcpy(argv[0], "prog");
  std::strcpy(argv[1], "-BiuqS");
  std::strcpy(argv[2], "--line-numbers");
  std::strcpy(argv[3], "clear-screen");
  std::strcpy(argv[4], "-I");

  lg_parser.parse(5, argv);
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

  auto arg = lg_parser.program_args().begin();
  EXPECT_EQ(2, lg_parser.program_args().size());
  EXPECT_EQ("prog", *arg++);
  EXPECT_EQ("clear-screen", *arg++);
  EXPECT_EQ(lg_parser.program_args().end(), arg);
  
  for (int i = 0; i < 5; ++i)
    delete[] argv[i];
}

TEST_F(OptionParserTest, OptionsWithArgsEq) {
  char* argv[7] = { new char[12], new char[12], new char[12],
                    new char[12], new char[12], new char[12],
                    nullptr };
  std::strcpy(argv[0], "prog");
  std::strcpy(argv[1], "--max-back-scroll=12");
  std::strcpy(argv[2], "-ep=42");
  std::strcpy(argv[3], "-P=\"custom prompt\"");
  std::strcpy(argv[4], "--buffer=10");
  std::strcpy(argv[5], "--color=red");

  lg_parser.parse(5, argv);
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
  
  for (int i = 0; i < 6; ++i)
    delete[] argv[i];
}

TEST_F(OptionParserTest, OptionsWithArgsSep) {
  char* argv[12] = { new char[12], new char[12], new char[12],
                     new char[12], new char[12], new char[12],
                     new char[12], new char[12], new char[12],
                     new char[12], new char[12], nullptr };
  std::strcpy(argv[0], "prog");
  std::strcpy(argv[1], "--max-back-scroll");
  std::strcpy(argv[2], "12");
  std::strcpy(argv[3], "-ep");
  std::strcpy(argv[4], "42");
  std::strcpy(argv[5], "-P");
  std::strcpy(argv[6], "custom prompt");
  std::strcpy(argv[7], "--buffer");
  std::strcpy(argv[8], "10");
  std::strcpy(argv[9], "--color");
  std::strcpy(argv[10], "red");

  lg_parser.parse(11, argv);
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
  
  for (int i = 0; i < 6; ++i)
    delete[] argv[i];
}

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
