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

#ifndef OPTIONPP_PARSER_RESULT_HPP
#define OPTIONPP_PARSER_RESULT_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace optionpp {

  template <typename StringType>
  class basic_parser_result {
  public:
    struct item;

    using value_type = item;
    using string_type = StringType;
    using char_type = typename StringType::value_type;
    using traits_type = typename StringType::traits_type;

    using container_type = std::vector<item>;
    using size_type = typename container_type::size_type;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

    struct item {
      string_type original_text{};
      bool is_option{};
      string_type long_name{};
      char_type short_name{};
      string_type argument{};
    };

    basic_parser_result() noexcept {}
    basic_parser_result(const std::initializer_list<value_type>& il)
      : m_items{il} {}
    template <typename InputIt>
    basic_parser_result(InputIt first, InputIt last) : m_items{first, last} {}

    void push_back(const value_type& item) { m_items.push_back(item); }
    void push_back(value_type&& item) { m_items.push_back(std::move(item)); }

    void clear() noexcept { m_items.clear(); }

    size_type size() const noexcept { return m_items.size(); }
    bool empty() const noexcept { return m_items.empty(); }

    iterator begin() noexcept { return m_items.begin(); }
    const_iterator begin() const noexcept { return cbegin(); }
    iterator end() noexcept { return m_items.end(); }
    const_iterator end() const noexcept { return cend(); }

    const_iterator cbegin() const noexcept { return m_items.cbegin(); }
    const_iterator cend() const noexcept { return m_items.cend(); }

    reverse_iterator rbegin() noexcept { return m_items.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    reverse_iterator rend() noexcept { return m_items.rend(); }
    const_reverse_iterator rend() const noexcept { return crend(); }

    const_reverse_iterator crbegin() const noexcept { return m_items.crbegin(); }
    const_reverse_iterator crend() const noexcept { return m_items.crend(); }

  private:
    container_type m_items;
  };

  using parser_result = basic_parser_result<std::string>;

} // End namespace

#endif
