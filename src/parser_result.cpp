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

/**
 * @file
 * @brief Source file for `parser_result` class implementation.
 */

#include <optionpp/parser_result.hpp>

#include <algorithm>
#include <optionpp/error.hpp>

using namespace optionpp;

bool parser_result::is_option_set(const std::string& long_name) const noexcept {
  if (long_name.empty())
    return false;
  else
    return std::any_of(begin(), end(),
                       [&](const parsed_entry& i) {
                         return i.is_option && i.long_name == long_name;
                       });
}

bool parser_result::is_option_set(char short_name) const noexcept {
  if (short_name == '\0')
    return false;
  else
    return std::any_of(begin(), end(),
                       [&](const parsed_entry& i) {
                         return i.is_option && i.short_name == short_name;
                       });
}

std::string parser_result::get_argument(std::string long_name) const noexcept {
  if (long_name == "")
    return "";

  auto it = std::find_if(rbegin(), rend(),
                         [&](const parsed_entry& i) {
                           return i.is_option && i.long_name == long_name;
                         });
  if (it != rend())
    return it->argument;
  else
    return "";
}

std::string parser_result::get_argument(char short_name) const noexcept {
  if (short_name == '\0')
    return "";

  auto it = std::find_if(rbegin(), rend(),
                         [=](const parsed_entry& i) {
                           return i.is_option && i.short_name == short_name;
                         });
  if (it != rend())
    return it->argument;
  else
    return "";
}
