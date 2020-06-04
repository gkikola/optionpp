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

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "option.hpp"
#include "parser_result.hpp"
#include "utility.hpp"

/**
 * @brief Library namespace.
 *
 * This namespace encloses all Option++ library definitions.
 */
namespace optionpp {

  /**
   * @brief Parses program options.
   *
   * A `parser` accepts program option information in the form of
   * `option` objects, which may be passed to the constructor or added
   * after construction with the add_option method. After all options
   * have been specified, the `parse` method may then be called with
   * the command-line arguments that were passed to the program.  This
   * will produce a `parser_result` containing the parsed information.
   *
   * For more information about the argument parsing, refer to the
   * documentation for the `parse` method.
   *
   * @see option
   * @see parser_result
   */
  class parser {
  public:

    /**
     * @brief Exception class indicating an invalid option.
     */
    class parse_error : public error {
    public:
      /**
       * @brief Constructor.
       * @param msg String describing the error.
       * @param fn_name Name of the function that threw the exception.
       * @param option Name of the option that triggered the error (if
       *               any).
       */
      parse_error(const std::string msg, const std::string fn_name,
                  const std::string option = "")
        : error(msg, fn_name), m_option{option} {}

      /**
       * @brief Return option name.
       * @return Option that triggered the error, if any.
       */
      const std::string& option() const noexcept { return m_option; }

    private:
      std::string m_option; //< Option that triggered the error.
    };

    /**
     * @brief Default constructor.
     *
     * No program options are accepted by default. Acceptable options
     * can later be specified using the `add_option` method.
     */
    parser() noexcept {}
    /**
     * @brief Construct from an initializer list.
     *
     * Each `option provided` in the list specifies an acceptable
     * program option that can be given on the command line.
     *
     * @param il The `initializer_list` containing the acceptable
     *           program options.
     * @see option
     */
    parser(const std::initializer_list<option>& il)
      : m_options{il} {}
    /**
     * @brief Construct from a sequence.
     *
     * Each `option` in the sequence specifies an acceptable program
     * option that can be given on the command line.
     *
     * @tparam InputIt The iterator type (usually deduced).
     * @param first The iterator pointing to the start of the
     *              sequence.
     * @param last The iterator pointing to one past the end of the
     *             sequence.
     */
    template <typename InputIt>
    parser(InputIt first, InputIt last) : m_options{first, last} {}

    /**
     * @brief Add a valid program option.
     *
     * The given `option` (if any) will specify information about a
     * program option that the user can set on the command line, such
     * as the option's long name, short name, description, and so on.
     *
     * Note that the method returns a reference to the instance of the
     * `option` that was inserted to allow chaining. For example:
     * ```
     * opt_parser.add_option()
     *           .long_name("verbose")
     *           .short_name('v')
     *           .description("Show verbose output.");
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
     * @brief Parse command-line arguments.
     *
     * Accepts the usual arguments that are normally supplied to
     * `main`, scans them for program options (these will be arguments
     * starting with a hyphen or double-hyphen) and stores the options
     * together with the non-option arguments in a `parser_result`
     * object.
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
     * @return `parser_result` containing the parsed data.
     * @throw parse_error If an invalid option is entered or a
     *                    mandatory argument is missing.
     * @see parser_result
     */
    template <typename InputIt>
    parser_result parse(InputIt first, InputIt last, bool ignore_first = true) const;

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
     * @return `parser_result` containing the parsed data.
     * @throw parser_error If an invalid option is entered or a
     *                     mandatory argument is missing.
     * @see parser_result
     */
    parser_result parse(int argc, const char* argv[], bool ignore_first = true) const;

    /**
     * @brief Parse command-line arguments from a string.
     *
     * For full details, see the description of the
     * `parse(InputIt, InputIt, bool)` overload. This version of the
     * function will split the string over whitespace to tokenize the
     * input. Quotes can be used within the string to specify
     * arguments containing whitespace.  A backslash can be used to
     * start an escape sequence within an argument.
     *
     * @param cmd_line The command-line arguments to parse.
     * @param ignore_first If true, the first argument is ignored.
     * @return `parser_result` containing the parsed data.
     * @throw parse_error If an invalid option is entered or a
     *                    mandatory argument is missing.
     * @see parser_result
     */
    parser_result parse(const std::string& cmd_line, bool ignore_first = false) const;

    /**
     * @brief Change special strings used by the parser.
     *
     * With this method, you can set custom option prefixes and such.
     * For each parameter, a blank string indicates that the old
     * string should be kept.
     *
     * @param delims Whitespace delimiters used to separate arguments.
     * @param short_prefix Prefix that indicates a group of short
     *                     option names.
     * @param long_prefix Prefix that indicates a long option name.
     * @param end_indicator Mark that indicates that all remaining
     *                      arguments should be interpreted as
     *                      non-option arguments.
     * @param equals String that indicates an explicit option
     *               argument.
     */
    void set_custom_strings(const std::string& delims,
                            const std::string& short_prefix = "",
                            const std::string& long_prefix = "",
                            const std::string& end_indicator = "",
                            const std::string& equals = "");

  private:

    /**
     * @brief Determines whether an argument is an end-of-option
     *        marker.
     * @param argument Argument to check.
     * @return True if the argument is an end-of-option marker.
     */
    bool is_end_indicator(const std::string& argument) const noexcept {
      return argument == m_end_of_options;
    }

    /**
     * @brief Determines whether an argument is a long command-line
     *        option.
     * @param argument Argument to check.
     * @return True if the argument begins with a long option prefix.
     */
    bool is_long_option(const std::string& argument) const noexcept {
      return argument.size() > m_long_option_prefix.size()
        && utility::is_substr_at_pos(argument, m_long_option_prefix);
    }

    /**
     * @brief Determines whether an argument is a short command-line
     *        option group.
     * @param argument Argument to check.
     * @return True if the argument begins with a short option prefix.
     */
    bool is_short_option_group(const std::string& argument) const noexcept {
      return argument.size() > m_short_option_prefix.size()
        && utility::is_substr_at_pos(argument, m_short_option_prefix);
    }

    /**
     * @brief Determines whether an argument is a non-option argument.
     * @param argument Argument to check.
     * @return True if the argument is a non-option argument.
     */
    bool is_non_option(const std::string& argument) const noexcept {
      return !is_end_indicator(argument)
        && !is_long_option(argument)
        && !is_short_option_group(argument);
    }

    /**
     * @brief Represents the type of a command-line argument.
     */
    enum class cl_arg_type { non_option, //< If the argument is not an option.
                             end_indicator, //< If the argument is an end-of-options marker.
                             arg_required, //< If the argument ends with an option that needs a mandatory argument.
                             arg_optional, //< If the argument ends with an option that can take an optional argument.
                             no_arg //< If the argument ends with an option that does not take an argument (or an argument was already given).
    };

    /**
     * @brief Parse a command-line argument.
     * @param argument Argument to parse.
     * @param result Current `parser_result`. New entries will be added
     *               to the end.
     * @param type Will be set to the appropriate option type.
     * @throw parse_error Thrown if option is invalid or missing a
     *                    required argument.
     * @see cl_arg_type
     */
    void parse_argument(const std::string& argument,
                        parser_result& result, cl_arg_type& type) const;

    /**
     * @brief Parse a group of short options.
     * @param short_names String of short option name characters to parse.
     * @param argument Option argument that was provided, if any.
     * @param has_arg Should be true if an argument was found (even an
     *                empty one).
     * @param result Current `parser_result`. New entries will be added
     *               to the end.
     * @param type Will be set to the appropriate option type.
     * @throw parse_error Thrown if option is invalid or missing a
     *                    required argument.
     * @see cl_arg_type
     */
    void parse_short_option_group(const std::string& short_names,
                                  const std::string& argument, bool has_arg,
                                  parser_result& result, cl_arg_type& type) const;

    /**
     * @brief Type used to hold `option` objects.
     */
    using opt_container = std::vector<option>;
    opt_container m_options; //< The container of `option` objects.

    std::string m_delims{" \t\n\r"}; //< Delimiters used to separate command-line arguments.
    std::string m_short_option_prefix{"-"}; //< String that indicates a group of short option names.
    std::string m_long_option_prefix{"--"}; //< String that indicates a long option name.
    std::string m_end_of_options{"--"}; //< String that marks the end of the program options.
    std::string m_equals{"="}; //< String used to specify an explicit argument to an option.
  };

} // End namespace

#endif
