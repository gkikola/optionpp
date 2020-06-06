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
 * @brief Header file for `option_group` class.
 */

#ifndef OPTIONPP_OPTION_GROUP_HPP
#define OPTIONPP_OPTION_GROUP_HPP

#include <string>
#include <utility>
#include <vector>
#include "option.hpp"

namespace optionpp {

  /**
   * @brief Holds a group of program options.
   *
   * Groups can be used to keep options organized when displaying
   * the application's help message.
   */
  class option_group {
  public:

    /**
     * @brief Type used to hold option information.
     */
    using value_type = option;
    /**
     * @brief Type of container used to hold the options.
     */
    using container_type = std::vector<option>;
    /**
     * @brief Type used to represent the size of the container.
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
     */
    option_group() noexcept {}
    /**
     * @brief Construct empty named group.
     * @param name Name of the group.
     */
    option_group(const std::string& name) : m_name{name} {}
    /**
     * @brief Construct from an initializer list.
     *
     * Each `option` provided in the list specifies a program option
     * that can be given on the command line or elsewhere.
     *
     * @param name Group name.
     * @param il The `initializer_list` containing the program
     *           options.
     * @see option
     */
    option_group(const std::string& name,
                 const std::initializer_list<option>& il)
      : m_name{name}, m_options{il} {}
    /**
     * @brief Construct from a sequence.
     *
     * Each `option` in the sequence specifies a program option that
     * can be given on the command line.
     *
     * @tparam InputIt The iterator type (usually deduced).
     * @param name Group name.
     * @param first The iterator pointing to the start of the
     *              sequence.
     * @param last The iterator pointing to one past the end of the
     *             sequence.
     */
    template <typename InputIt>
    option_group(const std::string& name,
                 InputIt first, InputIt last)
      : m_name{name}, m_options{first, last} {}

    /**
     * @brief Returns the name of the group.
     * @return Group name.
     */
    const std::string& name() const noexcept { return m_name; }

    /**
     * @brief Add a program option to the group.
     *
     * Note that the method returns a reference to the instance of the
     * `option` that was inserted to allow chaining. For example:
     * ```
     * group.add_option()
     *      .long_name("verbose")
     *      .short_name('v')
     *      .description("Show verbose output.");
     * ```
     *
     * @param opt The `option` to add.
     * @return Reference to the inserted `option`, for chaining.
     */
    option& add_option(const option& opt = option{}) {
      m_options.push_back(opt);
      return m_options.back();
    }
    /**
     * @brief Construct and add a program option to the group.
     * @param long_name Long name for the option.
     * @param short_name Short name for the option.
     * @param description Description of the option.
     * @param arg_name Name of the option's argument.
     * @param arg_required Should be true if the option's argument is
     *                     mandatory.
     * @return Reference to the inserted `option`, for chaining.
     */
    option& add_option(const std::string& long_name,
                       char short_name = '\0',
                       const std::string& description = "",
                       const std::string& arg_name = "",
                       bool arg_required = false);

    /**
     * @brief Return the number of options in the group.
     * @return Size of option container.
     */
    size_type size() const noexcept { return m_options.size(); }
    /**
     * @brief Return whether the container is empty.
     * @return True if the `option` container is empty, false otherwise.
     */
    bool empty() const noexcept { return m_options.empty(); }

    /**
     * @brief Return an `iterator` to the first option in the group.
     * @return An `iterator` pointing to the first `option`.
     */
    iterator begin() noexcept { return m_options.begin(); }
    /**
     * @copydoc cbegin
     */
    const_iterator begin() const noexcept { return cbegin(); }
    /**
     * @brief Return an `iterator` to one beyond the last option in
     *        the group.
     * @return An `iterator` pointing to one past the last `option`.
     */
    iterator end() noexcept { return m_options.end(); }
    /**
     * @copydoc cend
     */
    const_iterator end() const noexcept { return cend(); }

    /**
     * @brief Return a `const_iterator` to the first `option`.
     * @return A `const_iterator` pointing to the first `option`.
     */
    const_iterator cbegin() const noexcept { return m_options.cbegin(); }
    /**
     * @brief Return a `const_iterator` to the end of the container.
     * @return A `const_iterator` pointing to one past the last
     *         `option`.
     */
    const_iterator cend() const noexcept { return m_options.cend(); }

    /**
     * @brief Return a `reverse_iterator` to the beginning.
     * @return A `reverse_iterator` pointing to the first `option` in
     *         the reversed sequence.
     */
    reverse_iterator rbegin() noexcept { return m_options.rbegin(); }
    /**
     * @copydoc crbegin
     */
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    /**
     * @brief Return a `reverse_iterator` to the end.
     * @return A `reverse_iterator` pointing to one past the last
     *         `option` in the reversed sequence.
     */
    reverse_iterator rend() noexcept { return m_options.rend(); }
    /**
     * @copydoc crend
     */
    const_reverse_iterator rend() const noexcept { return crend(); }

    /**
     * @brief Return a `const_reverse_iterator` to the beginning.
     * @return A `const_reverse_iterator` pointing to the first
     *         `option` in the reversed sequence.
     */
    const_reverse_iterator crbegin() const noexcept { return m_options.crbegin(); }
    /**
     * @brief Return a `const_reverse_iterator` to the end.
     * @return A `const_reverse_iterator` pointing to one past the last
     *         `option` in the reversed sequence.
     */
    const_reverse_iterator crend() const noexcept { return m_options.crend(); }

    /**
     * @brief Search for an option in the group.
     *
     * Looks up an option by its long name and returns an iterator
     * pointing to the `option` instance.
     *
     * @param long_name Long name of the option.
     * @return Iterator pointing to the option or `end` if not found.
     */
    iterator find(const std::string& long_name);

    /**
     * @brief Search for an option in the group.
     *
     * Looks up an option by its long name and returns an iterator
     * pointing to the `option` instance.
     *
     * @param long_name Long name of the option.
     * @return Iterator pointing to the option or `end` if not found.
     */
    const_iterator find(const std::string& long_name) const;

    /**
     * @brief Search for an option in the group.
     *
     * Looks up an option by its short name and returns an iterator
     * pointing to the `option` instance.
     *
     * @param short_name Short name of the option.
     * @return Iterator pointing to the option or `end` if not found.
     */
    iterator find(char short_name);

    /**
     * @brief Search for an option in the group.
     *
     * Looks up an option by its short name and returns an iterator
     * pointing to the `option` instance.
     *
     * @param short_name Short name of the option.
     * @return Iterator pointing to the option or `end` if not found.
     */
    const_iterator find(char short_name) const;

    /**
     * @brief Sorts the options by name.
     *
     * By default, options are kept in the same order as they were
     * added. Calling this method will sort them by name (that is, for
     * each option, if both long and short names exist, then the long
     * name is used for comparison).
     */
    void sort();

  private:
    std::string m_name; //< Group name.
    container_type m_options; //< Collection of program options.
  };

} // End namespace

#endif
