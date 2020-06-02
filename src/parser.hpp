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
 * @brief Header file for `parser` class.
 */

#ifndef OPTIONPP_PARSER_HPP
#define OPTIONPP_PARSER_HPP

#include <initializer_list>
#include <string>
#include <vector>
#include "option.hpp"

/**
 * @brief Library namespace.
 *
 * This namespace encloses all Option++ library definitions.
 */
namespace optionpp {

  /**
   * @brief Parses program options.
   *
   * A `basic_parser` accepts program option information in the form
   * of `basic_option` objects, which may be passed to the constructor
   * or added after construction with the add_option method. After all
   * options have been specified, the `parse` method may then be
   * called with the command-line arguments that were passed to the
   * program.  This will produce a `basic_parser_result` containing
   * the parsed information.
   *
   * For more information about the argument parsing, refer to the
   * documentation for the `parse` method.
   *
   * @tparam StringType
   * @parblock
   * @brief The type of string to use for input and output.
   *
   * Usually this is `std::string` (i.e. `std::basic_string<char>`),
   * which is used in the `parser` type alias, but other string variants
   * may be used instead.
   * @endparblock
   * @see basic_option
   * @see basic_parser_result
   */
  template <typename StringType>
  class basic_parser {
  public:
    using string_type = StringType; //< Type of string used for input/output.
    using char_type = typename StringType::value_type; //< Type of character used in `string_type`.
    using traits_type = typename StringType::traits_type; //< Type specifying character traits used in `string_type`.
    using option_type = basic_option<StringType>; //< Type that stores option descriptions.

    /**
     * @brief Default constructor.
     *
     * No program options are accepted by default. Acceptable options
     * can later be specified using the `add_option` method.
     */
    basic_parser() noexcept {}
    /**
     * @brief Construct from an initializer list.
     *
     * Each `basic_option provided` in the list specifies an
     * acceptable program option that can be given on the command
     * line.
     *
     * @param il The `initializer_list` containing the acceptable
     *           program options.
     * @see basic_option
     */
    basic_parser(const std::initializer_list<option_type>& il)
      : m_options{il} {}
    /**
     * @brief Construct from a sequence.
     *
     * Each `basic_option` in the sequence specifies an acceptable
     * program option that can be given on the command line.
     *
     * @tparam InputIt The iterator type (usually deduced).
     * @param first The iterator pointing to the start of the
     *              sequence.
     * @param last The iterator pointing to one past the end of the
     *             sequence.
     */
    template <typename InputIt>
    basic_parser(InputIt first, InputIt last) : m_options{first, last} {}

    /**
     * @brief Add a valid program option.
     *
     * The given `basic_option` (if any) will specify information
     * about a program option that the user can set on the command
     * line, such as the option's long name, short name, description,
     * and so on.
     *
     * Note that the method returns a reference to the instance of the
     * `basic_option` that was inserted to allow chaining. For
     * example:
     * ```
     * opt_parser.add_option()
     *           .long_name("verbose")
     *           .short_name('v')
     *           .description("Show verbose output.");
     * ```
     *
     * @param opt The `basic_option` to add.
     * @return Reference to the inserted `basic_option`, for chaining.
     */
    option_type& add_option(const option_type& opt = option_type{}) {
      m_options.push_back(opt);
      return m_options.back();
    }

  private:
    using opt_container = std::vector<option_type>; //< Type used to hold `basic_option` objects.

    opt_container m_options; //< The container of `basic_option` objects.
  };

  using parser = basic_parser<std::string>; //< Type alias for typical usage of `basic_parser`.

} // End namespace

#endif
