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

#include "utility.hpp"

using namespace optionpp;

bool utility::is_substr_at_pos(const std::string& str, const std::string& substr,
                               typename std::string::size_type pos) noexcept {
  if (pos + substr.size() > str.size())
    return false;

  for (decltype(pos) i{0}; i < substr.size(); ++i) {
    if (str[pos + i] != substr[i])
      return false;
  }

  return true;
}
