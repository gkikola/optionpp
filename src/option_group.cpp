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
 * @brief Source file for `option_group` class implementation.
 */

#include "option_group.hpp"

#include <algorithm>

using namespace optionpp;

option& option_group::add_option(const std::string& long_name,
                                 char short_name,
                                 const std::string& description,
                                 const std::string& arg_name,
                                 bool arg_required) {
  m_options.emplace_back(long_name, short_name, description,
                         arg_name, arg_required);
  return m_options.back();
}

auto option_group::find(const std::string& long_name) -> iterator {
  return std::find_if(m_options.begin(), m_options.end(),
                      [&](const option& o) { return o.long_name() == long_name; });
}

auto option_group::find(const std::string& long_name) const -> const_iterator {
  return std::find_if(m_options.begin(), m_options.end(),
                      [&](const option& o) { return o.long_name() == long_name; });
}

auto option_group::find(char short_name) -> iterator {
  return std::find_if(m_options.begin(), m_options.end(),
                      [&](const option& o) { return o.short_name() == short_name; });
}

auto option_group::find(char short_name) const -> const_iterator {
  return std::find_if(m_options.begin(), m_options.end(),
                      [&](const option& o) { return o.short_name() == short_name; });
}

void option_group::sort() {
  std::sort(m_options.begin(), m_options.end(),
            [](const option& a, const option& b) {
              return a.name() < b.name();
            });
}
