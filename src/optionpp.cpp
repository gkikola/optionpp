/* optionpp -- read command-line program options
   Copyright (C) 2017 Gregory Kikola.

   This file is part of option++.

   option++ is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   option++ is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with option++.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Written by Gregory Kikola <gkikola@gmail.com>. */

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <sstream>
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

OptionParser::iterator OptionParser::find(char short_name)
{
  std::find_if(begin(), end(),
               [short_name](const Option& o)
               { return o.short_name == short_name; });
}

OptionParser::const_iterator OptionParser::find(char short_name) const
{
  std::find_if(cbegin(), cend(),
               [short_name](const Option& o)
               { return o.short_name == short_name; });
}

OptionParser::iterator OptionParser::find(const std::string& long_name)
{
  std::find_if(begin(), end(),
               [&long_name](const Option& o)
               { return o.long_name == long_name; });
}

OptionParser::const_iterator
OptionParser::find(const std::string& long_name) const
{
  std::find_if(cbegin(), cend(),
               [&long_name](const Option& o)
               { return o.long_name == long_name; });
}

std::ostream& OptionParser::print_usage(std::ostream& out,
                                        unsigned tab_stop,
                                        unsigned term_width)
{
  for (const OptionDesc& o : m_opts) {
    bool has_short = o.short_name;
    bool has_long = o.long_name != "";
    bool has_arg = o.argument_name != "";
    bool arg_optional = o.arg_optional;
    std::string entry = "  ";

    //print short name
    if (has_short) {
      entry += '-';
      entry += o.short_name;
      if (has_long)
        entry += ", ";
      else
        entry += "  ";
    }
    else
      entry += "    ";

    //print long name
    if (has_long) {
      entry += "--" + o.long_name;

      if (has_arg) {
        if (arg_optional)
          entry += '[';
        entry += "=" + o.argument_name;
        if (arg_optional)
          entry += ']';
      }
    } else if (has_arg) {
      if (arg_optional)
        entry += '[';
      entry += o.argument_name;
      if (arg_optional)
        entry += ']';
    }

    //print spaces up to tab stop
    if (entry.length() >= tab_stop - 1)
      entry += '\n' + std::string(tab_stop - 1, ' ');
    else
      entry += std::string(tab_stop - 1 - entry.length(), ' ');

    out << entry;

    //print description
    std::istringstream in(o.description);
    std::string word;
    unsigned length = 0;
    while (in >> word) {
      //insert space before each word
      if (length != 0) {
        out << " ";
        ++length;
      }

      //ensure we haven't gone past terminal width
      length += word.length();
      if (tab_stop + length > term_width) {
        out << "\n" << std::string(tab_stop + 1, ' ');
        length = word.length() + 2;
      }

      out << word;
    }

    out << std::endl;
  }

  return out;
}

void OptionParser::usage(std::string& opt_usage_str,
                         unsigned tab_stop, unsigned term_width)
{
  std::ostringstream oss;
  print_usage(oss, tab_stop, term_width);
  opt_usage_str = oss.str();
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

      if (argv[i][0] == '-' && argv[i][1]) { //no argument given
        if (!m_allow_bad_args && m_opts_read.back().desc
            && !m_opts_read.back().desc->arg_optional)
          throw BadOptionArgument("expected argument for option "
                                  + m_last_option_read);
      } else { //read the argument
        m_opts_read.back().argument = argv[i];
        continue;
      }
    }
    
    if (!opts_done && argv[i][0] == '-' && argv[i][1]) {
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
    if (!m_allow_bad_args && m_opts_read.back().desc
        && !m_opts_read.back().desc->arg_optional)
      throw BadOptionArgument("expected argument for option "
                              + m_last_option_read);
  }
}

bool OptionParser::read_short_opts(const std::string& argstr)
{
  bool expecting_arg = false;
  bool arg_optional = false;

  for (std::size_t i = 0; i < argstr.size(); ++i) {
    if (argstr[i] == '=') {
      if (expecting_arg || m_allow_bad_args) {
        if (m_opts_read.empty())
          throw BadOptionArgument("expected option before argument");

        //add argument to last option read
        std::string arg = argstr.substr(i + 1);
        m_opts_read.back().argument = arg;

        //if there was no argument read, expect it later
        expecting_arg = arg.empty();
        break;
      } else {
          if (m_last_option_read.empty())
            throw BadOptionArgument("expected option before argument");
          else
            throw BadOptionArgument("unexpected argument for option "
                                    + m_last_option_read);
      }
    } else { //argstr[i] != '='
      if (expecting_arg && !arg_optional && !m_allow_bad_args) {
        assert(!m_opts_read.empty());
        throw BadOptionArgument("expected argument for option "
                                + m_last_option_read);
      }
      
      if (OptionDesc* opt_desc = lookup(argstr[i])) {
        Option opt = { opt_desc->short_name, opt_desc->long_name };
        opt.desc = opt_desc;
        m_opts_read.push_back(opt);
        m_last_option_read = std::string("-") + argstr[i];
        expecting_arg = !opt_desc->argument_name.empty();
        arg_optional = opt_desc->arg_optional;
      } else if (!m_allow_bad_opts) {
        throw BadOption("unexpected option " + m_last_option_read);
      } else { //allowing bad options
        Option opt = { argstr[i] };
        m_opts_read.push_back(opt);
        m_last_option_read = std::string("-") + argstr[i];
      }
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
  } else if (!m_allow_bad_opts) {
    throw BadOption("unexpected option " + m_last_option_read);
  } else { //allowing bad options
    Option opt = { 0, long_name, arg };
    m_opts_read.push_back(opt);
    m_last_option_read = "--" + long_name;
    if (arg.empty() && n != std::string::npos)
      expecting_arg = true;
  }
  
  return expecting_arg;
}

bool operator<(const OptionDesc& o1, const OptionDesc& o2)
{
  std::string s1, s2;

  //group ordering should go 0, 1, 2, ..., n, -1, -2, -3, ...
  if (o1.group != o2.group) {
    if (o1.group >= 0 && o2.group >= 0) //both nonnegative
      return o1.group < o2.group;
    else if (o1.group >= 0 && o2.group < 0)
      return true;
    else if (o1.group < 0 && o2.group >= 0)
      return false;
    else //both negative
      return o1.group > o2.group;
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

bool operator<=(const OptionDesc& o1, const OptionDesc& o2)
{
  return o1 < o2 || o1 == o2;
}

bool operator>(const OptionDesc& o1, const OptionDesc& o2)
{
  return !(o1 <= o2);
}

bool operator>=(const OptionDesc& o1, const OptionDesc& o2)
{
  return !(o1 < o2);
}

bool operator==(const OptionDesc& o1, const OptionDesc& o2)
{
  return o1.short_name == o2.short_name
    && o1.long_name == o2.long_name
    && o1.argument_name == o2.argument_name
    && o1.description == o2.description
    && o1.group == o2.group
    && o1.arg_optional == o2.arg_optional;
}

bool operator!=(const OptionDesc& o1, const OptionDesc& o2)
{
  return !(o1 == o2);
}
