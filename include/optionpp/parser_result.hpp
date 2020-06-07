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
 * @brief Header file for `parser_result` class.
 */

#ifndef OPTIONPP_PARSER_RESULT_HPP
#define OPTIONPP_PARSER_RESULT_HPP

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <optionpp/error.hpp>
#include <optionpp/option.hpp>

namespace optionpp {

  /**
   * @brief Holds data parsed from the command line.
   *
   * Each `parsed_entry` instance represents either a program option
   * or a non-option argument that was passed on the command line.
   */
  struct parsed_entry {
    /**
     * @brief Default constructor.
     */
    parsed_entry() noexcept {};

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
    explicit parsed_entry(const std::string& original_text,
                          bool is_option = false,
                          const std::string& long_name = "",
                          char short_name = '\0',
                          const std::string& argument = "")
      : original_text{original_text}, is_option{is_option},
        long_name{long_name}, short_name{short_name}, argument{argument} {}

    /**
     * @brief The original text used on the command line.
     *
     * For non-option arguments, the text is stored verbatim. If this
     * `parsed_entry` represents an option, then this field will take
     * the form `-X` if the short name was used (where `X` is the
     * short name) or `--XXX` if the long name was used (where `XXX`
     * is the long name).
     *
     * In addition, if an argument was given with the option, then
     * the argument will be included after the option name.
     */
    std::string original_text;

    /**
     * @brief The original text used on the command line but without
     * any option argument.
     *
     * For example, if the user entered an option `--width=32`, then
     * this field should be set to `"--width"` whereas
     * `original_text` would contain the full string.
     */
    std::string original_without_argument;

    /**
     * @brief True if this `parsed_entry` represents a program option,
     * false otherwise.
     *
     * This field will be false if the `parsed_entry` is a non-option
     * argument (a separate command line argument that does not begin
     * with a hyphen, or one that occurs after the end-of-options
     * marker `--`).
     */
    bool is_option{false};

    /**
     * @brief The long name of the option which this `parsed_entry`
     * represents.
     *
     * If `is_option` is false, this should be an empty string.
     */
    std::string long_name;

    /**
     * @brief The short name of the option which this `parsed_entry`
     * represents.
     *
     * If `is_option` is false, this should be a null character.
     */
    char short_name{'\0'};

    /**
     * @brief The argument that was passed to the option, if any.
     *
     * If `is_option` is false, or if no argument was given, then
     * this should be an empty string.
     */
    std::string argument;

    /**
     * @brief Pointer to the `option` instance representing this
     * option, if any.
     */
    const option* opt_info{nullptr};
  };

  /**
   * @brief Holds data that was parsed from the program command line.
   *
   * This is a container of `parsed_entry` instances that describe all
   * the program options that were set on the command line as well as
   * all non-option arguments, all in the order in which they were
   * originally specified.
   *
   * For example, if the user ran the program with the command
   * ```
   * myprogram -afn file1.txt file2.txt --verbose file3.txt
   * ```
   * then the corresponding `parser_result` would hold seven data
   * entries: `-a`, `-f`, `-n`, `file1.txt`, `file2.txt`, `--verbose`,
   * `file3.txt`, in that order.
   */
  class parser_result {
  public:

    /**
     * @brief Type used for storing argument data.
     */
    using value_type = parsed_entry;
    /**
     * @brief Type of container used to store the data entries.
     */
    using container_type = std::vector<value_type>;
    /**
     * @brief Unsigned integer type (usually `std::size_t`) that can
     * hold container size.
     */
    using size_type = container_type::size_type;
    /**
     * @brief Plain iterator type.
     */
    using iterator = container_type::iterator;
    /**
     * @brief Constant iterator type.
     */
    using const_iterator = container_type::const_iterator;
    /**
     * @brief Reverse iterator type.
     */
    using reverse_iterator = container_type::reverse_iterator;
    /**
     * @brief Constant reverse iterator type.
     */
    using const_reverse_iterator = container_type::const_reverse_iterator;

    /**
     * @brief Default constructor.
     *
     * Constructs an empty `parser_result`.
     */
    parser_result() noexcept {}
    /**
     * @brief Construct from an initializer list.
     * @param il The `initializer_list` holding the parsed data.
     */
    parser_result(const std::initializer_list<value_type>& il)
      : m_entries{il} {}
    /**
     * @brief Construct from a sequence.
     * @tparam InputIt The type of iterator (usually deduced).
     * @param first Iterator pointing to the beginning of the sequence.
     * @param last Iterator pointing to one past the end of the sequence.
     */
    template <typename InputIt>
    parser_result(InputIt first, InputIt last) : m_entries{first, last} {}

    /**
     * @brief Add a `parsed_entry` to the back of the container.
     * @param entry The parsed data entry to add.
     */
    void push_back(const value_type& entry) { m_entries.push_back(entry); }
    /**
     * @copydoc push_back
     */
    void push_back(value_type&& entry) { m_entries.push_back(std::move(entry)); }

    /**
     * @brief Erase all data entries currently stored.
     */
    void clear() noexcept { m_entries.clear(); }

    /**
     * @brief Return the number of data entries.
     *
     * This will be the total number of options and non-option
     * arguments that were specified.
     *
     * @return The number of option and non-option argument data
     *         entries.
     */
    size_type size() const noexcept { return m_entries.size(); }
    /**
     * @brief Return whether the container is empty.
     * @return True if the entry container is empty, false otherwise.
     */
    bool empty() const noexcept { return m_entries.empty(); }

    /**
     * @brief Return an `iterator` to the beginning of the container.
     * @return An `iterator` pointing to the first entry.
     */
    iterator begin() noexcept { return m_entries.begin(); }
    /**
     * @copydoc cbegin
     */
    const_iterator begin() const noexcept { return cbegin(); }
    /**
     * @brief Return an `iterator` to the end of the container.
     * @return An `iterator` pointing to one past the last entry.
     */
    iterator end() noexcept { return m_entries.end(); }
    /**
     * @copydoc cend
     */
    const_iterator end() const noexcept { return cend(); }

    /**
     * @brief Return a `const_iterator` to the beginning of the container.
     * @return A `const_iterator` pointing to the first entry.
     */
    const_iterator cbegin() const noexcept { return m_entries.cbegin(); }
    /**
     * @brief Return a `const_iterator` to the end of the container.
     * @return A `const_iterator` pointing to one past the last entry.
     */
    const_iterator cend() const noexcept { return m_entries.cend(); }

    /**
     * @brief Return a `reverse_iterator` to the beginning.
     * @return A `reverse_iterator` pointing to the first entry in the
     *         reversed sequence.
     */
    reverse_iterator rbegin() noexcept { return m_entries.rbegin(); }
    /**
     * @copydoc crbegin
     */
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    /**
     * @brief Return a `reverse_iterator` to the end.
     * @return A `reverse_iterator` pointing to one past the last
     *         entry in the reversed sequence.
     */
    reverse_iterator rend() noexcept { return m_entries.rend(); }
    /**
     * @copydoc crend
     */
    const_reverse_iterator rend() const noexcept { return crend(); }

    /**
     * @brief Return a `const_reverse_iterator` to the beginning.
     * @return A `const_reverse_iterator` pointing to the first entry
     *         in the reversed sequence.
     */
    const_reverse_iterator crbegin() const noexcept { return m_entries.crbegin(); }
    /**
     * @brief Return a `const_reverse_iterator` to the end.
     * @return A `const_reverse_iterator` pointing to one past the last
     *         entry in the reversed sequence.
     */
    const_reverse_iterator crend() const noexcept { return m_entries.crend(); }

    /**
     * @brief Range-checked subscript.
     * @param index The index of the `parsed_entry` to return.
     * @return The parsed data entry corresponding to the `index`.
     * @throw out_of_range Thrown if `index >= size()`.
     */
    value_type& at(size_type index) {
      if (index >= size())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return (*this)[index];
    }
    /**
     * @copydoc at
     */
    const value_type& at(size_type index) const {
      if (index >= size())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return (*this)[index];
    }

    /**
     * @brief Subscript operator.
     * @param index The index of the data entry to return.
     * @return The `parsed_entry` corresponding to the `index`.
     */
    value_type& operator[](size_type index) { return m_entries[index]; }
    /**
     * @copydoc operator[]
     */
    const value_type& operator[](size_type index) const {
      return m_entries[index];
    }

    /**
     * @brief Access last entry in the container.
     * @throw out_of_range If container is empty.
     * @return Reference to last `parsed_entry`.
     */
    value_type& back() {
      if (empty())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::back");
      return m_entries.back();
    }

    /**
     * @copydoc back
     */
    const value_type& back() const {
      if (empty())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return m_entries.back();
    }

    /**
     * @brief Returns whether the specified option is set.
     * @param long_name The long name for the option.
     * @return True if the option was present on the command-line,
     *         and false otherwise.
     */
    bool is_option_set(const std::string& long_name) const noexcept;
    /**
     * @brief Returns whether the specified option is set.
     * @param short_name The short name for the option.
     * @return True if the option was present on the command-line,
     *         and false otherwise.
     */
    bool is_option_set(char short_name) const noexcept;

    /**
     * @brief Get the argument for the specified option.
     *
     * If no argument was given, an empty string is returned. If
     * multiple arguments were given, the last is returned.
     *
     * @param long_name The long name for the option.
     * @return The argument given to the option.
     */
    std::string get_argument(std::string long_name) const noexcept;
    /**
     * @brief Get the argument for the specified option.
     *
     * If no argument was given, an empty string is returned. If
     * multiple arguments were given, the last is returned.
     *
     * @param short_name The short name for the option.
     * @return The argument given to the option.
     */
    std::string get_argument(char short_name) const noexcept;

  private:
    container_type m_entries; //< The internal container of `parsed_entry` instances.
  };

  /**
   * @brief Iterator over non-option program arguments.
   */
  class non_option_iterator : public std::iterator<std::bidirectional_iterator_tag,
                                                   parsed_entry> {
    friend bool operator==(const non_option_iterator& a,
                           const non_option_iterator& b);
  public:
    /**
     * @brief Default constructor.
     */
    non_option_iterator() noexcept : m_result{nullptr}, m_index{} {}
    /**
     * @brief Constructor.
     * @param result Parser result to iterate over.
     */
    non_option_iterator(parser_result& result);

    /**
     * @brief Dereference operator.
     * @return Reference to the pointed-to entry.
     */
    parsed_entry& operator*() const { return (*m_result)[m_index]; }

    /**
     * @brief Member access operator.
     * @return Reference to the pointed-to entry.
     */
    parsed_entry* operator->() const { return &**this; }

    /**
     * @brief Prefix increment operator.
     * @return The incremented iterator pointing to the next non-option
     *         argument.
     */
    non_option_iterator& operator++();

    /**
     * @brief Postfix increment operator.
     * @return A copy of the unincremented iterator.
     */
    non_option_iterator operator++(int);

    /**
     * @brief Prefix decrement operator.
     * @throw out_of_range If the iterator already points to the
     *                     beginning.
     * @return The decremented iterator pointing to the previous non-option
     *         argument.
     */
    non_option_iterator& operator--();

    /**
     * @brief Postfix decrement operator.
     * @throw out_of_range If the iterator already points to the
     *                     beginning.
     * @return A copy of the undecremented iterator.
     */
    non_option_iterator operator--(int);

  private:
    parser_result* m_result; //< `parser_result` being iterated over.
    parser_result::size_type m_index; //< Current entry index.
  };

  /**
   * @brief `non_option_iterator` comparison operator.
   * @param a Left operand to compare.
   * @param b Right operand to compare.
   * @return True if a and b point to the same member of the same
   *         `parser_result`.
   */
  bool operator==(const non_option_iterator& a,
                  const non_option_iterator& b);

  /**
   * @brief `non_option_iterator` comparison operator.
   * @param a Left operand to compare.
   * @param b Right operand to compare.
   * @return False if a and b point to the same member of the same
   *         `parser_result`.
   */
  inline bool operator!=(const non_option_iterator& a,
                         const non_option_iterator& b) { return !(a == b); }

  /**
   * @brief `begin` function for range for loop support.
   * @param it A `non_option_iterator`.
   * @return The unchanged iterator `it`
   */
  inline non_option_iterator begin(non_option_iterator it) noexcept { return it; }

  /**
   * @brief `end` function for range for loop support.
   * @return The end iterator.
   */
  inline non_option_iterator end(const non_option_iterator&) noexcept {
    return non_option_iterator{};
  }

} // End namespace

#endif
