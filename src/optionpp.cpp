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

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include "optionpp.hpp"

void optionpp::OptionParser::add(char sname,
                                 const std::string& lname,
                                 const std::string& aname,
                                 const std::string& desc,
                                 int group)
{
  OptionDesc opt = { sname, lname, aname, desc, group };
  m_opts.push_back(opt);
}

void optionpp::OptionParser::add(std::initializer_list<OptionDesc> opts)
{
  m_opts.insert(m_opts.end(), opts.begin(), opts.end());
}

optionpp::OptionDesc*
optionpp::OptionParser::lookup(char short_name)
{
  //convert to const, use const lookup, then convert back
  return const_cast<OptionDesc*>
    (const_cast<const OptionParser*>(this)->lookup(short_name));
}

const optionpp::OptionDesc*
optionpp::OptionParser::lookup(char short_name) const
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

optionpp::OptionDesc*
optionpp::OptionParser::lookup(const std::string& long_name)
{
  //convert to const, use const lookup, then convert back
  return const_cast<OptionDesc*>
    (const_cast<const OptionParser*>(this)->lookup(long_name));
}

const optionpp::OptionDesc*
optionpp::OptionParser::lookup(const std::string& long_name) const
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

optionpp::OptionParser::iterator
optionpp::OptionParser::find(char short_name)
{
  return std::find_if(begin(), end(),
                      [short_name](const Option& o)
                      { return o.short_name == short_name; });
}

optionpp::OptionParser::const_iterator
optionpp::OptionParser::find(char short_name) const
{
  return std::find_if(cbegin(), cend(),
                      [short_name](const Option& o)
                      { return o.short_name == short_name; });
}

optionpp::OptionParser::iterator
optionpp::OptionParser::find(const std::string& long_name)
{
  return std::find_if(begin(), end(),
                      [&long_name](const Option& o)
                      { return o.long_name == long_name; });
}

optionpp::OptionParser::const_iterator
optionpp::OptionParser::find(const std::string& long_name) const
{
  return std::find_if(cbegin(), cend(),
                      [&long_name](const Option& o)
                      { return o.long_name == long_name; });
}

std::ostream&
optionpp::OptionParser::print_usage(std::ostream& out,
                                    unsigned tab_stop,
                                    unsigned term_width) const
{
  //sort option descriptions by option name
  desc_set opts = m_opts;
  std::stable_sort(opts.begin(), opts.end());

  for (const OptionDesc& o : opts) {
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

void
optionpp::OptionParser::usage(std::string& opt_usage_str,
                              unsigned tab_stop,
                              unsigned term_width) const
{
  std::ostringstream oss;
  print_usage(oss, tab_stop, term_width);
  opt_usage_str = oss.str();
}

void optionpp::OptionParser::parse(int argc, char* argv[])
{
  m_opts_read.clear();
  m_prog_args.clear();
  m_prog_cmd = argv[0];
  m_last_option_read = "";

  bool opts_done = false;
  bool reading_arg = false;

  for (size_t i = 1;
       (i != static_cast<size_t>(argc)) && argv[i];
       ++i) {
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

bool
optionpp::OptionParser::read_short_opts(const std::string& argstr)
{
  bool expecting_arg = false;

  for (std::size_t i = 0; i < argstr.size(); ++i) {
    if (argstr[i] == '=') {
      if (!expecting_arg && !m_allow_bad_args) {
        if (m_opts_read.empty())
          throw BadOptionArgument("expected option before argument");
        else
          throw BadOptionArgument("unexpected argument for option "
                                  + m_last_option_read);
      } else
        expecting_arg = true;
    } else { //argstr[i] != '='
      char name = argstr[i];
      m_last_option_read = std::string("-") + name;

      if (OptionDesc* opt_desc = lookup(name)) {
        Option opt =
          {
            opt_desc->short_name, opt_desc->long_name,
            "", nullptr
          };
        opt.desc = opt_desc;
        m_opts_read.push_back(opt);
        expecting_arg = !opt_desc->argument_name.empty();
      } else if (!m_allow_bad_opts) {
        throw BadOption("unexpected option " + m_last_option_read);
      } else { //allowing bad options
        Option opt = { argstr[i], "", "", nullptr };
        m_opts_read.push_back(opt);
      }
    }

    if (expecting_arg) {
      if (i + 1 < argstr.size() && argstr[i + 1] == '=')
        ++i;
      if (m_opts_read.empty())
        throw BadOptionArgument("expected option before argument");

      //add argument to last option read
      std::string arg = argstr.substr(i + 1);
      m_opts_read.back().argument = arg;

      //if there was no argument read, expect it later
      expecting_arg = arg.empty();
      break;
    }
  }

  return expecting_arg;
}

bool
optionpp::OptionParser::read_long_opt(const std::string& argstr)
{
  std::string long_name, arg;
  std::string::size_type n = 0;
  if ((n = argstr.find('=')) != std::string::npos) {
    long_name = argstr.substr(0, n);
    arg = argstr.substr(n + 1);
  } else
    long_name = argstr;

  m_last_option_read = "--" + long_name;

  bool expecting_arg = false;
  if (OptionDesc* opt_desc = lookup(long_name)) {
    Option opt = { opt_desc->short_name, opt_desc->long_name, arg };
    opt.desc = opt_desc;
    m_opts_read.push_back(opt);
    expecting_arg = !opt_desc->argument_name.empty() && arg.empty();
  } else if (!m_allow_bad_opts) {
    throw BadOption("unexpected option " + m_last_option_read);
  } else { //allowing bad options
    Option opt = { 0, long_name, arg };
    m_opts_read.push_back(opt);
    if (arg.empty() && n != std::string::npos)
      expecting_arg = true;
  }

  return expecting_arg;
}

int optionpp::Option::arg_to_int() const
{
  try {
    std::size_t pos = 0;
    int ret = std::stoi(argument, &pos);
    validate_arg(pos);

    return ret;
  } catch (const std::invalid_argument& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " must be an integer");
  } catch (const std::out_of_range& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " is out of range");
  }
}

unsigned optionpp::Option::arg_to_unsigned() const
{
  try {
    std::size_t pos = 0;
    int val = std::stoi(argument, &pos);
    validate_arg(pos);

    if (val < 0)
      throw BadOptionArgument("argument for option " + option_name()
                              + " must not be negative");

    return static_cast<unsigned>(val);
  } catch (const std::invalid_argument& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " must be a nonnegative integer");
  } catch (const std::out_of_range& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " is out of range");
  }
}

long optionpp::Option::arg_to_long() const
{
  try {
    std::size_t pos = 0;
    long ret = std::stol(argument, &pos);
    validate_arg(pos);

    return ret;
  } catch (const std::invalid_argument& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " must be an integer");
  } catch (const std::out_of_range& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " is out of range");
  }
}

double optionpp::Option::arg_to_double() const
{
  try {
    std::size_t pos = 0;
    double ret = std::stod(argument, &pos);
    validate_arg(pos);

    return ret;
  } catch (const std::invalid_argument& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " must be a number");
  } catch (const std::out_of_range& e) {
    throw BadOptionArgument("argument for option " + option_name()
                            + " is out of range");
  }
}

std::string optionpp::Option::option_name() const
{
  std::string ret;
  if (!long_name.empty() && short_name)
    return "--" + long_name + " (-" + short_name + ")";
  else if (!long_name.empty())
    return "--" + long_name;
  else
    return "-" + std::string(1, short_name);
}

void optionpp::Option::validate_arg(std::size_t end_pos) const
{
  while (end_pos < argument.length()) {
    if (argument[end_pos] != ' ')
      throw std::invalid_argument("validate_arg");
    ++end_pos;
  }
}

bool optionpp::operator<(const OptionDesc& o1,
                         const OptionDesc& o2)
{
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
    std::string name1, name2;

    //same group, compare short name and long name, in that order
    if (o1.short_name)
      name1 = o1.short_name;
    else
      name1 = o1.long_name;

    if (o2.short_name)
      name2 = o2.short_name;
    else
      name2 = o2.long_name;

    auto comp = [](char c1, char c2)
      { return std::toupper(c1) < std::toupper(c2); };
    return std::lexicographical_compare(name1.begin(), name1.end(),
                                        name2.begin(), name2.end(),
                                        comp);
  }
}

bool optionpp::operator<=(const OptionDesc& o1,
                          const OptionDesc& o2)
{
  return (o1 < o2) || (o1 == o2);
}

bool optionpp::operator>(const OptionDesc& o1,
                         const OptionDesc& o2)
{
  return !(o1 <= o2);
}

bool optionpp::operator>=(const OptionDesc& o1,
                          const OptionDesc& o2)
{
  return !(o1 < o2);
}

bool optionpp::operator==(const OptionDesc& o1,
                          const OptionDesc& o2)
{
  return o1.short_name == o2.short_name
    && o1.long_name == o2.long_name
    && o1.argument_name == o2.argument_name
    && o1.description == o2.description
    && o1.group == o2.group
    && o1.arg_optional == o2.arg_optional;
}

bool optionpp::operator!=(const OptionDesc& o1,
                          const OptionDesc& o2)
{
  return !(o1 == o2);
}
