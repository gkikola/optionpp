#include <algorithm>
#include <cassert>
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

void OptionParser::parse(int argc, char* argv[])
{
  m_opts_read.clear();
  m_prog_args.clear();
  m_prog_args.push_back(argv[0]);
  m_last_option_read = "";

  bool opts_done = false;
  bool reading_arg = false;
  
  for (size_t i = 1; (i != argc) && argv[i]; ++i) {
    if (reading_arg) {
      reading_arg = false;

      if (m_opts_read.empty())
        throw BadOptionArgument("expected option before argument");

      if (argv[i][0] == '-') { //no argument given
        if (!m_opts_read.back().desc->arg_optional)
          throw BadOptionArgument("expected argument for option "
                                  + m_last_option_read);
      } else { //read the argument
        m_opts_read.back().argument = argv[i];
        continue;
      }
    }
    
    if (!opts_done && argv[i][0] == '-') {
      if (argv[i][1] == '-') { //long option
        std::string lopt = argv[i] + 2;
        if (lopt.empty()) //--: no more options
          opts_done = true;
        else
          reading_arg = read_long_opt(lopt);
      } else //short option
        reading_arg = read_short_opts(argv[i] + 1);
    } else { //not an option
      m_prog_args.push_back(argv[i]);
    }
  }

  //end of program arguments, so we shouldn't be expecting any option args
  if (reading_arg) {
    if (!m_opts_read.back().desc->arg_optional)
      throw BadOptionArgument("expected argument for option "
                              + m_last_option_read);
  }
}

bool OptionParser::read_short_opts(const std::string& argstr)
{
  bool expecting_arg = false;
  bool arg_optional = false;

  for (auto c : argstr) {
    if (c == '=') {
      if (expecting_arg) {
        if (m_opts_read.empty())
          throw BadOptionArgument("expected option before argument");

        //add argument to last option read
        auto n = argstr.find('=');
        assert(n != std::string::npos);
        m_opts_read.back().argument = argstr.substr(n + 1);

        expecting_arg = false;
        break;
      } else {
        if (m_allow_bad_args)
          return true;
        else
          if (m_last_option_read.empty())
            throw BadOptionArgument("expected option before argument");
          else
            throw BadOptionArgument("unexpected argument for option "
                                    + m_last_option_read);
      }
    } else { //c != '='
      if (expecting_arg && !arg_optional) {
        assert(!m_opts_read.empty());
        throw BadOptionArgument("expected argument for option "
                                + m_last_option_read);
      }
      
      if (OptionDesc* opt_desc = lookup(c)) {
        Option opt = { opt_desc->short_name, opt_desc->long_name };
        opt.desc = opt_desc;
        m_opts_read.push_back(opt);
        m_last_option_read = std::string("-") + c;
        expecting_arg = !opt_desc->argument_name.empty();
        arg_optional = opt_desc->arg_optional;
      } else
        throw BadOption("unexpected option " + m_last_option_read);
    }
  }
  
  return expecting_arg;
}

bool OptionParser::read_long_opt(const std::string& argstr)
{
  std::string long_name, arg;
  std::string::size_type n = 0;
  if ((n = argstr.find('=')) != std::string::npos) {
    long_name = argstr.substr(0, n);
    arg = argstr.substr(n + 1);
  } else
    long_name = argstr;

  bool expecting_arg = false;
  if (OptionDesc* opt_desc = lookup(long_name)) {
    Option opt = { opt_desc->short_name, opt_desc->long_name, arg };
    opt.desc = opt_desc;
    m_opts_read.push_back(opt);
    m_last_option_read = "--" + opt_desc->long_name;
    expecting_arg = !opt_desc->argument_name.empty() && arg.empty();
  } else
    throw BadOption("unexpected option " + m_last_option_read);
  
  return expecting_arg;
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
