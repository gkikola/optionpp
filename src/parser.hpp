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
 * @brief Header file for `basic_parser` class.
 */

#ifndef OPTIONPP_PARSER_HPP
#define OPTIONPP_PARSER_HPP

#include <initializer_list>
#include <string>
#include <vector>
#include "option.hpp"
#include "parser_result.hpp"

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
     * @brief Type that stores option descriptions.
     */
    using option_type = basic_option<StringType>;
    /**
     * @brief Type that stores the parsed result data.
     */
    using result_type = basic_parser_result<StringType>;

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

    /**
     * @brief Parse command-line arguments.
     *
     * Accepts the usual arguments that are normally supplied to
     * `main`, scans them for program options (these will be arguments
     * starting with a hyphen or double-hyphen) and stores the options
     * together with the non-option arguments in a
     * `basic_parser_result` object.
     *
     * Each option can have a short name consisting of a single
     * character (following a hyphen), or a long name (following a
     * double-hyphen), or both. For example, an option called
     * `verbose` with a short name of `v` could be specified either as
     * `--verbose` or as `-v`.
     *
     * Multiple short names can be specified in a single argument. For
     * example `-abc` would specify three options with respective
     * short names `a`, `b`, and `c`.
     *
     * Some options may accept arguments. Arguments can be supplied in
     * several ways: for long names, the usual form is
     * `--long-name=argument` and for short names `-X argument`. The
     * parser will also accept `--long-name argument`, `-X=argument`,
     * and `-Xargument`.
     *
     * A double-hyphen (`--`) by itself can be used to indicate the
     * end of program options. Any remaining command line arguments
     * are then interpreted as non-option arguments. A single hyphen
     * (`-`) by itself is considered a non-option argument (this is
     * often used as a way to specify standard input instead of a
     * filename).
     *
     * @param first An iterator pointing to the first argument.
     * @param last An iterator pointing to one past the last argument.
     * @param ignore_first If true, the first argument (typically the
     *                     program filename) is ignored.
     * @return A `basic_parser_result` containing the parsed data.
     * @see basic_parser_result
     */
    template <typename InputIt>
    result_type parse(InputIt first, InputIt last, bool ignore_first = true);

    /**
     * @brief Parse command-line arguments.
     *
     * Accepts the usual arguments that are normally supplied to
     * `main`. For further details, see the description of the
     * `parse(InputIt, InputIt, bool)` overload.
     *
     * @param argc The number of arguments given on the command line.
     * @param argv All command-line arguments.
     * @param ignore_first If true, the first argument (typically the
     *                     program filename) is ignored.
     * @return A `basic_parser_result` containing the parsed data.
     * @see basic_parser_result
     */
    result_type parse(int argc, const char_type* argv[], bool ignore_first = true);

    /**
     * @brief Parse command-line arguments from a string.
     *
     * For full details, see the description of the
     * `parse(InputIt, InputIt, bool)` overload. This version of the
     * function will split the string over a set of delimiters to
     * tokenize the input. The specified quotation symbols can be used
     * within the string to specify arguments containing delimiters.
     * `escape_char` can be used to start an escape sequence within an
     * argument.
     *
     * @param cmd_line The command-line arguments to parse.
     * @param ignore_first If true, the first argument is ignored.
     * @param delims Delimiters to use for splitting.
     * @param quotes Quote characters to use for quoting arguments.
     * @param escape_char Character that starts escape sequences.
     * @return A `basic_parser_result` containing the parsed data.
     * @see basic_parser_result
     */
    result_type parse(const string_type& cmd_line, bool ignore_first,
                      const string_type& delims, const string_type& quotes,
                      char_type escape_char);

  private:
    /**
     * @brief Type used to hold `basic_option` objects.
     */
    using opt_container = std::vector<option_type>;

    opt_container m_options; //< The container of `basic_option` objects.
  };

  /**
   * @brief Type alias for typical usage of `basic_parser`.
   */
  using parser = basic_parser<std::string>;

} // End namespace

#endif
