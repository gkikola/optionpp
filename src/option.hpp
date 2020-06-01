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

#ifndef OPTIONPP_OPTION_HPP
#define OPTIONPP_OPTION_HPP

#include <string>

namespace optionpp {

  enum class argument_type { optional, required };

  template <typename StringType>
  class basic_option {
  public:
    using string_type = StringType;
    using char_type = typename StringType::value_type;
    using traits_type = typename StringType::traits_type;

    basic_option() noexcept(noexcept(string_type())) {}
    basic_option(char_type short_name) : m_short_name{short_name} {}
    basic_option(string_type long_name, char_type short_name = char_type{})
      : m_long_name{long_name}, m_short_name{short_name} {}

    basic_option& long_name(const string_type& name) {
      m_long_name = name;
      return *this;
    }
    const string_type& long_name() const noexcept { return m_long_name; }

    basic_option& short_name(char_type name) noexcept {
      m_short_name = name;
      return *this;
    }
    char_type short_name() const noexcept { return m_short_name; }

    inline basic_option& argument(const string_type& name,
                                  argument_type type = argument_type::optional);
    const string_type& argument_name() const noexcept { return m_arg_name; }
    bool is_argument_required() const noexcept { return m_arg_required; }

    basic_option& description(const string_type& desc) {
      m_desc = desc;
      return *this;
    }
    const string_type& description() const noexcept { return m_desc; }

    basic_option& group(const string_type& group_name) {
      m_group_name = group_name;
      return *this;
    }
    const string_type& group() const noexcept { return m_group_name; }

  private:
    string_type m_long_name;
    char_type m_short_name{};
    string_type m_arg_name;
    bool m_arg_required{};
    string_type m_desc;
    string_type m_group_name;
  };


  /* Implementation */

  template <typename S>
  basic_option<S>&
  basic_option<S>::argument(const string_type& name, argument_type type) {
    m_arg_name = name;
    m_arg_required = type == argument_type::required;

    return *this;
  }

  using option = basic_option<std::string>;

} // End namespace

#endif
