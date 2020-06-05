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

#include "parser.hpp"

#include <limits>
#include <stdexcept>

using namespace optionpp;

void parser::set_custom_strings(const std::string& delims,
                                const std::string& short_prefix,
                                const std::string& long_prefix,
                                const std::string& end_indicator,
                                const std::string& equals) {
  if (!delims.empty())
    m_delims = delims;
  if (!short_prefix.empty())
    m_short_option_prefix = short_prefix;
  if (!long_prefix.empty())
    m_long_option_prefix = long_prefix;
  if (!end_indicator.empty())
    m_end_of_options = end_indicator;
  if (!equals.empty())
    m_equals = equals;
}

void parser::write_option_argument(const option& opt,
                                   const parser_result::item& item) const {
  if (!opt.has_bound_argument_variable())
    return;

  std::string::size_type pos = 0;
  const std::string& arg = item.argument;
  const std::string& opt_name = item.original_without_argument;
  const std::string& fn_name = "optionpp::parser::write_option_argument";

  try {
    switch (opt.argument_type()) {
    case option::uint_arg: {
      long long value = std::stoll(item.argument, &pos);
      if (pos != arg.size())
        throw std::invalid_argument{"invalid argument"};
      if (value < 0)
        throw parse_error{"argument for option '" + opt_name + "' must not be negative",
            fn_name, opt_name};
      else if (value > std::numeric_limits<unsigned>::max())
        throw std::out_of_range{"out of range"};
      opt.write_uint(static_cast<unsigned>(value));
      break;
    }
    case option::int_arg: {
      int value = std::stoi(item.argument, &pos);
      if (pos != arg.size())
        throw std::invalid_argument{"invalid argument"};
      opt.write_int(value);
      break;
    }
    case option::double_arg: {
      double value = std::stod(item.argument, &pos);
      if (pos != arg.size())
        throw std::invalid_argument{"invalid argument"};
      opt.write_double(value);
      break;
    }
    default:
    case option::string_arg:
      opt.write_string(arg);
      break;
    }
  } catch(const std::invalid_argument&) {
    switch (opt.argument_type()) {
    case option::uint_arg:
    case option::int_arg:
      throw parse_error{"argument for option '" + opt_name + "' must be an integer",
                          fn_name, opt_name};
    case option::double_arg:
      throw parse_error{"argument for option '" + opt_name + "' must be a number",
                          fn_name, opt_name};
    default:
      throw type_error{"type error in argument for option '" + opt_name + "'", fn_name};
    }
  } catch(const std::out_of_range&) {
    throw parse_error{"argument for option '" + opt_name + "' is out of range",
                        fn_name, opt_name};
  }
}

template <typename InputIt>
parser_result parser::parse(InputIt first, InputIt last, bool ignore_first) const {
  if (ignore_first && first != last)
    ++first;

  InputIt it{first};

  parser_result result{};
  cl_arg_type prev_type{cl_arg_type::non_option};
  while (it != last) {
    const std::string& arg{*it};

    // If we are expecting a standalone option argument...
    if (prev_type == cl_arg_type::arg_required
        || prev_type == cl_arg_type::arg_optional) {
      // ...see if an argument was provided...
      if (!is_end_indicator(arg) && !is_long_option(arg)
          && !is_short_option_group(arg)) {
        auto& arg_info = result.back();
        arg_info.argument = arg;
        arg_info.original_text.push_back(' ');
        arg_info.original_text += arg;
        prev_type = cl_arg_type::non_option;
        if (arg_info.opt_info)
          write_option_argument(*arg_info.opt_info, arg_info);
      } else { // ...if not, make sure argument was optional
        if (prev_type == cl_arg_type::arg_required) {
          const auto& opt_name = result.back().original_text;
          throw parse_error{"option '" + opt_name + "' requires an argument",
              "optionpp::parser::parse", opt_name};
        }
        prev_type = cl_arg_type::non_option;
        continue; // Continue without incrementing 'it' in order to reevaluate current token
      }
    } else if (prev_type == cl_arg_type::end_indicator) { // Ignore options
      typename parser_result::item arg_info;
      arg_info.original_text = arg;
      arg_info.is_option = false;
      result.push_back(std::move(arg_info));
    } else { // Regular argument
      parse_argument(arg, result, prev_type);
    }

    ++it;
  }

  // Make sure we don't still need a mandatory argument
  if (prev_type == cl_arg_type::arg_required) {
    const auto& opt_name = result.back().original_text;
    throw parse_error{"option '" + opt_name + "' requires an argument",
        "optionpp::parser::parse", opt_name};
  }

  return result;
}

parser_result parser::parse(int argc, const char* argv[], bool ignore_first) const {
  return parse(argv, argv + argc, ignore_first);
}

parser_result parser::parse(const std::string& cmd_line, bool ignore_first) const {
  std::vector<std::string> container;
  utility::split(cmd_line, std::back_inserter(container),
                 m_delims, "\"'", '\\');
  return parse(container.begin(), container.end(), ignore_first);
}

void parser::parse_argument(const std::string& argument,
                            parser_result& result, cl_arg_type& type) const {
  // Check for end-of-option marker
  if (is_end_indicator(argument)) {
    type = cl_arg_type::end_indicator;
    return;
  }

  // Split string into components
  std::string option_specifier;
  std::string option_argument;
  bool assignment_found = false;
  auto pos = argument.find(m_equals);
  if (pos == std::string::npos)
    option_specifier = argument;
  else {
    assignment_found = true;
    option_specifier = argument.substr(0, pos);
    pos += m_equals.size();
    option_argument = argument.substr(pos);

    // Check for bad syntax like -= and --=
    if (option_specifier == m_short_option_prefix
        || option_specifier == m_long_option_prefix) {
      option_specifier += m_equals;
      throw parse_error{"invalid option: '" + option_specifier + "'",
          "optionpp::parser::parse_argument", option_specifier};
    }
  }

  // Check option type
  parser_result::item arg_info;
  if (is_long_option(option_specifier)) {
    // Extract option name
    std::string option_name = option_specifier.substr(m_long_option_prefix.size());

    // Look up option info
    auto it = std::find_if(m_options.begin(), m_options.end(),
                           [&](const option& o) { return o.long_name() == option_name; });
    if (it == m_options.end())
      throw parse_error{"invalid option: '" + option_specifier + "'",
          "optionpp::parser::parse_argument", option_specifier};
    arg_info.opt_info = &(*it);

    // Does this option take an argument?
    if (!it->argument_name().empty()) {
      if (!assignment_found) { // No arg was found, caller should look for it
        if (it->is_argument_required())
          type = cl_arg_type::arg_required;
        else
          type = cl_arg_type::arg_optional;
      } else { // Found an argument
        type = cl_arg_type::no_arg; // Caller should not look for argument
        arg_info.argument = option_argument;
      }
    } else { // Does not take an argument
      if (assignment_found) // Found an argument where there should be none
        throw parse_error{"option '" + option_specifier + "' does not accept arguments",
                            "optionpp::parser::parse_argument", option_specifier};
      type = cl_arg_type::no_arg;
    }
    arg_info.original_text = argument;
    arg_info.original_without_argument = option_specifier;
    arg_info.is_option = true;
    arg_info.long_name = option_name;
    arg_info.short_name = it->short_name();
    if (assignment_found)
      write_option_argument(*it, arg_info);
    it->write_bool(true);
    result.push_back(std::move(arg_info));
  } else if (is_short_option_group(option_specifier)) { // Short options
    parse_short_option_group(option_specifier.substr(m_short_option_prefix.size()),
                             option_argument, assignment_found,
                             result, type);
  } else {
    // If we get here, this argument is not an option
    type = cl_arg_type::non_option;
    arg_info.original_text = argument;
    arg_info.is_option = false;
    result.push_back(std::move(arg_info));
  }
}

void parser::parse_short_option_group(const std::string& short_names,
                                      const std::string& argument, bool has_arg,
                                      parser_result& result, cl_arg_type& type) const {
  using sz_t = std::string::size_type;
  for (sz_t pos = 0; pos != short_names.size(); ++pos) {
    // Look up option info
    auto it = std::find_if(m_options.begin(), m_options.end(),
                           [&](const option& o) { return o.short_name() == short_names[pos]; });
    if (it == m_options.end()) {
      auto opt_name = m_short_option_prefix;
      opt_name.push_back(short_names[pos]);
      throw parse_error{"invalid option: '" + opt_name + "'",
          "optionpp::parser::parse_short_option_group", opt_name};
    }

    parser_result::item arg_info;
    arg_info.original_text = m_short_option_prefix;
    arg_info.original_text.push_back(short_names[pos]);
    arg_info.original_without_argument = arg_info.original_text;
    arg_info.is_option = true;
    arg_info.long_name = it->long_name();
    arg_info.short_name = short_names[pos];
    arg_info.opt_info = &(*it);
    it->write_bool(true);

    // Check if option takes an argument
    if (!it->argument_name().empty()) {
      if (pos + 1 < short_names.size()) {
        // This isn't the last option, so the rest of the string is an argument
        arg_info.argument = short_names.substr(pos + 1);
        if (has_arg) {
          // The assignment symbol is actually part of the argument
          arg_info.argument += m_equals;
          arg_info.argument += argument;
        }
        arg_info.original_text += arg_info.argument;
        write_option_argument(*it, arg_info);
        result.push_back(std::move(arg_info));
        type = cl_arg_type::no_arg;
        break;
      } else {
        // This is the last option and it needs an argument
        if (has_arg) {
          arg_info.original_text += m_equals;
          arg_info.original_text += argument;
          arg_info.argument = argument;
          write_option_argument(*it, arg_info);
          type = cl_arg_type::no_arg;
        } else if (it->is_argument_required()) {
          type = cl_arg_type::arg_required;
        } else {
          type = cl_arg_type::arg_optional;
        }
        result.push_back(std::move(arg_info));
        break;
      }
    }

    // If we make it here, then the current option does not take an argument
    if (pos + 1 == short_names.size() && has_arg) {
      auto opt_name = m_short_option_prefix;
      opt_name.push_back(short_names[pos]);
      throw parse_error{"option '" + opt_name + "' does not accept arguments",
          "optionpp::parser::parse_short_option_group", opt_name};
    }

    result.push_back(std::move(arg_info));
    type = cl_arg_type::no_arg;
    arg_info = parser_result::item{};
  } // End for loop
}
