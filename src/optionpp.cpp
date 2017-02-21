#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <string>
#include <vector>
#include "optionpp.hpp"

void OptionParser::add(char sname, const std::string& lname,
         const std::string& aname, const std::string& desc,
         int group)
{
  OptionDesc opt = { sname, lname, aname, desc, group };
  m_opts.insert(opt);
}

void OptionParser::add(std::initializer_list<OptionDesc> opts)
{
  m_opts.insert(opts.begin(), opts.end());
}

OptionDesc* OptionParser::lookup(char short_name)
{
  //convert to const, use const lookup, then convert back
  return const_cast<OptionDesc*>
    (const_cast<const OptionParser*>(this)->lookup(short_name));
}

const OptionDesc* OptionParser::lookup(char short_name) const
{
  decltype(m_opts.cbegin()) it
    = std::find_if(m_opts.cbegin(), m_opts.cend(),
                   [short_name](const OptionDesc& d)
                   { return d.short_name == short_name; });

  if (it == m_opts.cend())
    return nullptr;
  else
    return &(*it);
}

OptionDesc* OptionParser::lookup(const std::string& long_name)
{
  //convert to const, use const lookup, then convert back
  return const_cast<OptionDesc*>
    (const_cast<const OptionParser*>(this)->lookup(long_name));
}

const OptionDesc* OptionParser::lookup(const std::string& long_name) const
{
  decltype(m_opts.cbegin()) it
    = std::find_if(m_opts.cbegin(), m_opts.cend(),
                   [&long_name](const OptionDesc& d)
                   { return d.long_name == long_name; });

  if (it == m_opts.cend())
    return nullptr;
  else
    return &(*it);
}

bool OptionParser::parse(int argc, char* argv[])
{
  m_opts_read.clear();
  m_prog_args.clear();
  m_prog_args.push_back(argv[0]);
  m_last_option_read = "";
  
  bool read_arg = false;
  
  for (size_t i = 1; (i != argc) && argv[i]; ++i) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == '-') { //long option
        read_long_arg(argv[i] + 2);
      } else //short option
        read_short_args(argv[i] + 1);
    } else { //not an option
      m_prog_args.push_back(argv[i]);
    }
  }

  return false;
}

bool OptionParser::read_short_args(const std::string& argstr)
{
  bool expecting_arg = false;

  for (auto c : argstr) {
    if (c == '=') {
      if (expecting_arg) return true;
      else {
        if (m_allow_bad_args)
          return true;
        else
          throw BadOption("unexpected argument for option "
                          + m_last_option_read);
      }
    } else { //c != '='
      if (OptionDesc* opt_desc = lookup(c)) {
        Option opt = { opt_desc->short_name, opt_desc->long_name };
        m_opts_read.push_back(opt);
        m_last_option_read = c;
      }      
    }
  }
  
  return false;
}

bool OptionParser::read_long_arg(const std::string& argstr)
{
  std::string long_name, arg;
  std::string::size_type n = 0;
  if ((n = argstr.find('=')) != std::string::npos) {
    long_name = argstr.substr(0, n);
    arg = argstr.substr(n + 1);
  } else
    long_name = argstr;

  if (OptionDesc* opt_desc = lookup(long_name)) {
    Option opt = { opt_desc->short_name, opt_desc->long_name, arg };
    m_opts_read.push_back(opt);
    m_last_option_read = opt_desc->long_name;
  }
  
  return false;
}

bool operator < (const OptionDesc& o1, const OptionDesc& o2)
{
  std::string s1, s2;

  //group ordering should go 0, 1, 2, ..., n, -1, -2, -3, ...
  if (o1.group != o2.group) {
    if (o1.group >= 0 && o1.group < o2.group)
      return true;
    else if (o1.group < 0 && o2.group >= 0)
      return true;
    else if (o2.group < 0 && o2.group < o1.group)
      return true;
    else
      return false;
  } else {
    //same group, compare short name and long name, in that order
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
}
