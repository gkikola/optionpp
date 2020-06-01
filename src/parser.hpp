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

#ifndef OPTIONPP_PARSER_HPP
#define OPTIONPP_PARSER_HPP

#include <initializer_list>
#include <string>
#include <vector>
#include "option.hpp"

namespace optionpp {

  template <typename StringType>
  class basic_parser {
  public:
    using string_type = StringType;
    using char_type = typename StringType::value_type;
    using traits_type = typename StringType::traits_type;
    using option_type = basic_option<StringType>;

    basic_parser() noexcept {}
    basic_parser(const std::initializer_list<option_type>& il)
      : m_options{il} {}
    template <typename InputIt>
    basic_parser(InputIt first, InputIt last) : m_options{first, last} {}

    option_type& add_option(const option_type& opt = option_type{}) {
      m_options.push_back(opt);
      return m_options.back();
    }

  private:
    using opt_container = std::vector<option_type>;

    opt_container m_options;
  };

  using parser = basic_parser<std::string>;

} // End namespace

#endif
