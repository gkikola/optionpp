#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>
#include "optionpp.hpp"

void OptionParser::add(char sname, const std::string& lname,
         const std::string& aname, const std::string& desc,
         int group)
{
  Option opt = { sname, lname, aname, desc, group };
  m_opts.push_back(opt);
}

void OptionParser::add(std::initializer_list<Option> opts)
{
  m_opts.insert(m_opts.end(), opts.begin(), opts.end());
}

bool OptionParser::parse(int argc, char* argv[])
{
  bool reading_arg = false;

  for (size_t i = 1; (i != argc) && argv[i]; ++i) {
    std::string cur;
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') //long option
        cur = argv[i] + 2;
      else //short option
        cur = argv[i] + 1;

      //do stuff
    } else { //not an option
      continue;
    }
  }

  return false;
}

bool operator < (const Option& o1, const Option& o2)
{
  std::string s1, s2;

  if (o1.short_name)
    s1 = o1.short_name;
  else
    s1 = o1.long_name;

  if (o2.short_name)
    s2 = o2.short_name;
  else
    s2 = o2.long_name;

  return s1 < s2;
}
