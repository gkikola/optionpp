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
 * @brief Header file for `basic_parser_result` class.
 */

#ifndef OPTIONPP_PARSER_RESULT_HPP
#define OPTIONPP_PARSER_RESULT_HPP

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace optionpp {

  /**
   * @brief Holds data that was parsed from the program command line.
   *
   * This is a container of `basic_parser_result::item` structures
   * that describe all the program options that were set on the
   * command line as well as all non-option arguments, all in the
   * order that they were originally specified.
   *
   * For example, if the user ran the program with the command
   * ```
   * myprogram -afn file1.txt file2.txt --verbose file3.txt
   * ```
   * then the corresponding `basic_parser_result` would hold seven
   * data items: `-a`, `-f`, `-n`, `file1.txt`, `file2.txt`,
   * `--verbose`, `file3.txt`, in that order.
   *
   * @tparam StringType
   * @parblock
   * @brief The type of string used by the `basic_parser`.
   *
   * Usually this is `std::string` (i.e. `std::basic_string<char>`),
   * which is used in the `parser_result` type alias, but other string
   * variants may be used instead.
   * @endparblock
   */
  template <typename StringType>
  class basic_parser_result {
  public:
    struct item;

    /**
     * @brief Type used for storing argument data.
     */
    using value_type = item;
    /**
     * @brief Type of string used for input/output.
     */
    using string_type = StringType;
    /**
     * @brief Type of character used in `string_type`.
     */
    using char_type = typename StringType::value_type;
    /**
     * @brief Type specifying character traits used in `string_type`.
     */
    using traits_type = typename StringType::traits_type;

    /**
     * @brief Type of container used to store the data items.
     */
    using container_type = std::vector<item>;
    /**
     * @brief Unsigned integer type (usually `std::size_t`) that can
     * hold container size.
     */
    using size_type = typename container_type::size_type;
    /**
     * @brief Plain iterator type.
     */
    using iterator = typename container_type::iterator;
    /**
     * @brief Constant iterator type.
     */
    using const_iterator = typename container_type::const_iterator;
    /**
     * @brief Reverse iterator type.
     */
    using reverse_iterator = typename container_type::reverse_iterator;
    /**
     * @brief Constant reverse iterator type.
     */
    using const_reverse_iterator = typename container_type::const_reverse_iterator;

    /**
     * @brief Holds data parsed from the command line.
     *
     * Each `item` instance represents either a program option or a
     * non-option argument that was passed on the command line.
     */
    struct item {
      /**
       * @brief Constructor.
       * @param original_text The original text used on the command line.
       * @param is_option True if the data represents a program option,
       *                  false for non-option arguments.
       * @param long_name The long name of the option (empty string if
       *                  this is not an option).
       * @param short_name The short name of the option (0 if this is not
       *                   an option).
       * @param argument The argument passed to the option, if any.
       */
      explicit item(const string_type& original_text,
                    bool is_option = false,
                    const string_type& long_name = string_type{},
                    char_type short_name = '\0',
                    const string_type& argument = string_type{})
        : original_text{original_text}, is_option{is_option},
          long_name{long_name}, short_name{short_name}, argument{argument} {}

      /**
       * @brief The original text used on the command line.
       *
       * For non-option arguments, the text is stored verbatim. If
       * this `item` represents an option, then this field will take
       * the form `-X` if the short name was used (where `X` is the
       * short name) or `--XXX` if the long name was used (where `XXX`
       * is the long name).
       *
       * In addition, if an argument was given with the option, then
       * the argument will be included after the option name.
       */
      string_type original_text;

      /**
       * @brief True if this `item` represents a program option, false
       * otherwise.
       *
       * This field will be false if the `item` is a non-option
       * argument (a separate command line argument that does not
       * begin with a hyphen).
       */
      bool is_option;

      /**
       * @brief The long name of the option which this `item`
       * represents.
       *
       * If `is_option` is false, this should be an empty string.
       */
      string_type long_name;

      /**
       * @brief The short name of the option which this `item`
       * represents.
       *
       * If `is_option` is false, this should be an empty string.
       */
      char_type short_name;

      /**
       * @brief The argument that was passed to the option, if any.
       *
       * If `is_option` is false, or if no argument was given, then
       * this should be an empty string.
       */
      string_type argument;
    };

    /**
     * @brief Default constructor.
     *
     * Constructs an empty `basic_parser_result`.
     */
    basic_parser_result() noexcept {}
    /**
     * @brief Construct from an initializer list.
     * @param il The `initializer_list` holding the parsed data.
     */
    basic_parser_result(const std::initializer_list<value_type>& il)
      : m_items{il} {}
    /**
     * @brief Construct from a sequence.
     * @tparam InputIt The type of iterator (usually deduced).
     * @param first Iterator pointing to the beginning of the `item` sequence.
     * @param last Iterator pointing to one past the end of the sequence.
     */
    template <typename InputIt>
    basic_parser_result(InputIt first, InputIt last) : m_items{first, last} {}

    /**
     * @brief Add an `item` to the back of the container.
     * @param item The parsed data item to add.
     */
    void push_back(const value_type& item) { m_items.push_back(item); }
    /**
     * @copydoc push_back
     */
    void push_back(value_type&& item) { m_items.push_back(std::move(item)); }

    /**
     * @brief Erase all data `item` objects currently stored.
     */
    void clear() noexcept { m_items.clear(); }

    /**
     * @brief Return the number of data items.
     *
     * This will be the total number of options and non-option
     * arguments that were specified.
     *
     * @return The number of option and non-option argument data
     *         items.
     */
    size_type size() const noexcept { return m_items.size(); }
    /**
     * @brief Return whether the container is empty.
     * @return True if the `item` container is empty, false otherwise.
     */
    bool empty() const noexcept { return m_items.empty(); }

    /**
     * @brief Return an `iterator` to the beginning of the container.
     * @return An `iterator` pointing to the first data `item`.
     */
    iterator begin() noexcept { return m_items.begin(); }
    /**
     * @copydoc cbegin
     */
    const_iterator begin() const noexcept { return cbegin(); }
    /**
     * @brief Return an `iterator` to the end of the container.
     * @return An `iterator` pointing to one past the last data `item`.
     */
    iterator end() noexcept { return m_items.end(); }
    /**
     * @copydoc cend
     */
    const_iterator end() const noexcept { return cend(); }

    /**
     * @brief Return a `const_iterator` to the beginning of the container.
     * @return A `const_iterator` pointing to the first data `item`.
     */
    const_iterator cbegin() const noexcept { return m_items.cbegin(); }
    /**
     * @brief Return a `const_iterator` to the end of the container.
     * @return A `const_iterator` pointing to one past the last `item`.
     */
    const_iterator cend() const noexcept { return m_items.cend(); }

    /**
     * @brief Return a `reverse_iterator` to the beginning.
     * @return A `reverse_iterator` pointing to the first `item` in
     *         the reversed sequence.
     */
    reverse_iterator rbegin() noexcept { return m_items.rbegin(); }
    /**
     * @copydoc crbegin
     */
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    /**
     * @brief Return a `reverse_iterator` to the end.
     * @return A `reverse_iterator` pointing to one past the last `item`
     *         in the reversed sequence.
     */
    reverse_iterator rend() noexcept { return m_items.rend(); }
    /**
     * @copydoc crend
     */
    const_reverse_iterator rend() const noexcept { return crend(); }

    /**
     * @brief Return a `const_reverse_iterator` to the beginning.
     * @return A `const_reverse_iterator` pointing to the first `item`
     *         in the reversed sequence.
     */
    const_reverse_iterator crbegin() const noexcept { return m_items.crbegin(); }
    /**
     * @brief Return a `const_reverse_iterator` to the end.
     * @return A `const_reverse_iterator` pointing to one past the last
     *         `item` in the reversed sequence.
     */
    const_reverse_iterator crend() const noexcept { return m_items.crend(); }

    /**
     * @brief Range-checked subscript.
     * @param index The index of the data `item` to return.
     * @return The parsed data `item` corresponding to the `index`.
     * @exception std::out_of_range Thrown if `index >= size()`.
     */
    value_type& at(size_type index) {
      if (index >= size())
        throw std::out_of_range("[optionpp] out of bounds parser_result access");
      return (*this)[index];
    }
    /**
     * @copydoc at
     */
    const value_type& at(size_type index) const {
      if (index >= size())
        throw std::out_of_range("[optionpp] out of bounds parser_result access");
      return (*this)[index];
    }

    /**
     * @brief Subscript operator.
     * @param index The index of the data `item` to return.
     * @return The parsed data `item` corresponding to the `index`.
     */
    value_type& operator[](size_type index) { return m_items[index]; }
    /**
     * @copydoc operator[]
     */
    const value_type& operator[](size_type index) const {
      return m_items[index];
    }

  private:
    container_type m_items; //< The internal container of `item` instances.
  };

  /**
   * @brief Type alias for typical usage of `basic_parser_result`.
   */
  using parser_result = basic_parser_result<std::string>;

} // End namespace

#endif
