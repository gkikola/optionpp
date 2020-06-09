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
#include <iosfwd>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <optionpp/option_group.hpp>
#include <optionpp/parser_result.hpp>
#include <optionpp/utility.hpp>

/**
 * @brief Library namespace.
 *
 * This namespace encloses all Option++ library definitions.
 */
namespace optionpp {

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
     * @brief Default constructor.
     *
     * No program options are accepted by default. Acceptable options
     * can later be specified using the `add_option` method.
     */
    parser() noexcept {}
    /**
     * @brief Construct from an initializer list.
     *
     * Each `option` provided in the list specifies an acceptable
     * program option that can be given on the command line.
     *
     * The options are added to the nameless default group.
     *
     * @param il The `initializer_list` containing the acceptable
     *           program options.
     * @see option
     */
    parser(const std::initializer_list<option>& il) {
      m_groups.emplace_back("", il.begin(), il.end());
    }
    /**
     * @brief Construct from a sequence.
     *
     * Each `option` in the sequence specifies an acceptable program
     * option that can be given on the command line.
     *
     * The options are added to the nameless default group.
     *
     * @tparam InputIt The iterator type (usually deduced).
     * @param first The iterator pointing to the start of the
     *              sequence.
     * @param last The iterator pointing to one past the end of the
     *             sequence.
     */
    template <typename InputIt>
    parser(InputIt first, InputIt last) { m_groups.emplace_back("", first, last); }

    /**
     * @brief Returns a reference to a particular group.
     *
     * The group is created if it does not exist.
     *
     * @param name Name of the group.
     * @return Reference to the group.
     */
    option_group& group(const std::string& name);

    /**
     * @brief Add a program option.
     *
     * The given `option` instance (if any) will specify information
     * about a program option that the user can set on the command
     * line, such as the option's long name, short name, description,
     * and so on.
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
    option& add_option(const option& opt = option{});

    /**
     * @brief Add a program option.
     *
     * @param long_name Long name for the option.
     * @param short_name Short name for the option.
     * @param description Option description (for help message).
     * @param arg_name Argument name, if any (usually uppercase)
     * @param arg_required Set to true if argument is mandatory.
     * @param group_name Name of group option should be added to.
     * @return Reference to the inserted `option`, for chaining.
     */
    option& add_option(const std::string& long_name,
                       char short_name = '\0',
                       const std::string& description = "",
                       const std::string& arg_name = "",
                       bool arg_required = false,
                       const std::string& group_name = "");

    /**
     * @brief Parse command-line arguments from a sequence of
     *        strings.
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
    parser_result parse(int argc, char* argv[], bool ignore_first = true) const;

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

    /**
     * @brief Sorts the groups by name.
     *
     * By default groups are kept in the order that they were added.
     */
    void sort_groups();

    /**
     * @brief Sorts all options by name.
     *
     * By default, options within each group are kept in the order
     * that they were added. This method will sort all the options in
     * each group so that they are ordered by name (either by their
     * long name, or by their short name if the long name doesn't
     * exist).
     */
    void sort_options();

    /**
     * @brief Subscript operator.
     *
     * Returns the specified option or creates it if it doesn't exist.
     *
     * @param long_name Long name for the option.
     * @return Matching option or newly created one if it didn't
     *         already exist.
     */
    option& operator[](const std::string& long_name);

    /**
     * @brief Subscript operator.
     *
     * Returns the specified option or creates it if it doesn't exist.
     *
     * @param short_name Short name for the option.
     * @return Matching option or newly created one if it didn't
     *         already exist.
     */
    option& operator[](char short_name);

    /**
     * @brief Print program help message.
     *
     * This will write all program options, organized by group, to the
     * given output stream. The additional parameters control the
     * formatting.
     *
     * The options are presented in the order that they were added to
     * each group; if desired, you can call `sort_options` first to
     * sort the options by name within each group.
     *
     * Option names and descriptions are displayed in columns, with
     * option names on the left and descriptions on the right. The
     * first line of a description will begin on the same line as the
     * option names, unless there's not enough room, in which case a
     * new line is started.
     *
     * The indentation levels are not cumulative: each parameter gives
     * the total level of indentation, counted from the leftmost
     * character of the line.
     *
     * @param os Output stream.
     * @param max_line_length Text will be wrapped so that each line
     *                        is at most this many characters.
     * @param group_indent Number of spaces to indent group names.
     * @param option_indent Number of spaces to indent option names.
     * @param desc_first_line_indent Number of spaces to indent first
     *                               line of each description.
     * @param desc_multiline_indent Number of spaces to indent
     *                              descriptions after the first line.
     * @return The output stream that was initially given.
     */
    std::ostream& print_help(std::ostream& os,
                             int max_line_length = 78,
                             int group_indent = 0,
                             int option_indent = 2,
                             int desc_first_line_indent = 30,
                             int desc_multiline_indent = 32) const;


  private:

    /**
     * @brief Type used to hold `option_group` objects.
     */
    using group_container = std::vector<option_group>;
    /**
     * @brief Iterator type for the group container.
     */
    using group_iterator = group_container::iterator;
    /**
     * @brief `const_iterator` type for the group container.
     */
    using group_const_iterator = group_container::const_iterator;
    /**
     * @brief Iterator type for iterating over options.
     */
    using option_iterator = option_group::iterator;
    /**
     * @brief `const_iterator` for iterating over options.
     */
    using option_const_iterator = option_group::const_iterator;

    /**
     * @brief Search for a group by name.
     * @param name Group name.
     * @return Iterator pointing to the group, or to the end if not
     *         found.
     */
    group_iterator find_group(const std::string& name);
    /**
     * @copydoc find_group
     */
    group_const_iterator find_group(const std::string& name) const;

    /**
     * @brief Search for an option by long name.
     * @param long_name Long name for the option.
     * @return Pointer to the option, or `nullptr` if not found.
     */
    option* find_option(const std::string& long_name);
    /**
     * @copydoc find_option
     */
    const option* find_option(const std::string& long_name) const;

    /**
     * @brief Search for an option by short name.
     * @param short_name Short name for the option.
     * @return Pointer to the option, or `nullptr` if not found.
     */
    option* find_option(char short_name);
    /**
     * @copydoc find_option
     */
    const option* find_option(char short_name) const;

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
     * @brief Write to an option's bound argument variable.
     *
     * The argument will be converted to the appropriate type. If it
     * cannot be converted, an exception is raised. If no variable was
     * bound to the option, then nothing is done.
     *
     * @param entry Object holding parsed result information for the
     *              option, including the argument to assign.
     */
    void write_option_argument(const parsed_entry& entry) const;

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

    group_container m_groups; //< The container of option groups.

    std::string m_delims{" \t\n\r"}; //< Delimiters used to separate command-line arguments.
    std::string m_short_option_prefix{"-"}; //< String that indicates a group of short option names.
    std::string m_long_option_prefix{"--"}; //< String that indicates a long option name.
    std::string m_end_of_options{"--"}; //< String that marks the end of the program options.
    std::string m_equals{"="}; //< String used to specify an explicit argument to an option.
  };

  /**
   * @brief Output operator.
   *
   * Writes program help text to an output stream using the default
   * options for printing. For finer control, use
   * `parser::print_help`.
   *
   * By default, options are presented in the same order that they
   * were inserted within each group. If desired, you can call
   * `parser::sort_options` first to sort by name.
   *
   * @param os Output stream to write output to.
   * @param parser Parser containing the program option information.
   * @return The given output stream.
   */
  std::ostream& operator<<(std::ostream& os, const parser& parser);

} // End namespace

/* Implementation */

// Doxygen seems to think that the parse implementation below is a new
// function, so we'll ask it to skip this part of the header
#ifndef DOXYGEN_SHOULD_SKIP_THIS

template <typename InputIt>
optionpp::parser_result
optionpp::parser::parse(InputIt first, InputIt last, bool ignore_first) const {
  if (ignore_first && first != last)
    ++first;

  InputIt it{first};

  parser_result result{};
  cl_arg_type prev_type{cl_arg_type::non_option};
  while (it != last) {
    const std::string& arg{*it};

    // If we are expecting a standalone option argument...
    if (prev_type == cl_arg_type::arg_required
        || prev_type == cl_arg_type::arg_optional) {
      // ...then this token should be a non-option; but if the
      // argument is required we'll interpret it that way regardless
      if (is_non_option(arg)
          || prev_type == cl_arg_type::arg_required) {
        auto& arg_info = result.back();
        arg_info.argument = arg;
        arg_info.original_text.push_back(' ');
        arg_info.original_text += arg;
        prev_type = cl_arg_type::non_option;
        if (arg_info.opt_info)
          write_option_argument(arg_info);
      } else { // Found an option, reset type and continue
        prev_type = cl_arg_type::non_option;
        continue; // Continue without incrementing 'it' in order to reevaluate current token
      }
    } else if (prev_type == cl_arg_type::end_indicator) { // Ignore options
      parsed_entry arg_info;
      arg_info.original_text = arg;
      arg_info.is_option = false;
      result.push_back(std::move(arg_info));
    } else { // Regular argument
      parse_argument(arg, result, prev_type);
    }

    ++it;
  }

  // Make sure we don't still need a mandatory argument
  if (prev_type == cl_arg_type::arg_required) {
    const auto& opt_name = result.back().original_text;
    throw parse_error{"option '" + opt_name + "' requires an argument",
        "optionpp::parser::parse", opt_name};
  }

  return result;
}

#endif // DOXYGEN_SHOULD_SKIP_THIS

#endif
