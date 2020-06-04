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
   * @brief Provides string information and values to be used with
   * `basic_parser`.
   *
   * By providing a specialization of this template and using it as
   * the `StringTraits` template parameter on `basic_option`, one may
   * change the strings that are used by the parser. For example, this
   * allows the ability to change the option prefixes, the whitespace
   * delimiters, and so on.
   *
   * Specializations for `std::string` and
   * `std::basic_string<wchar_t>` are provided. To provide your own
   * specializations, define the appropriate static member functions.
   * See the default specializations for examples.
   */
  template <typename StringType>
  struct string_traits {};

  /**
   * @brief Default specialization of `string_traits` for
   * `std::string`.
   */
  template<>
  struct string_traits<std::string> {
    /**
     * @brief Type of string used by the static member functions.
     */
    using string_type = std::string;
    /**
     * @brief Type of character used by `string_type`.
     */
    using char_type = char;
    /**
     * @brief Return the space character.
     * @return Whitespace character.
     */
    static char_type space_char() noexcept { return ' '; }
    /**
     * @brief Return the newline sequence.
     * @return String that indicates a newline.
     */
    static string_type newline() noexcept { return "\n"; }
    /**
     * @brief Return whitespace characters.
     * @return String containing characters that should be
     *         considered as whitespace.
     */
    static string_type whitespace() noexcept { return " \t\n\r"; }
    /**
     * @brief Return quote characters.
     * @return String containing characters used to quote
     *         command line arguments.
     */
    static string_type quotes() noexcept { return "\"'"; }
    /**
     * @brief Return escape character.
     * @return Character that begins an escape sequence.
     */
    static char_type escape_char() noexcept { return '\\'; }
    /**
     * @brief Return prefix for long option names.
     * @return Prefix used for long option names.
     */
    static string_type long_option_prefix() noexcept { return "--"; }
    /**
     * @brief Return prefix for short option names.
     * @return Prefix used for short option names.
     */
    static string_type short_option_prefix() noexcept { return "-"; }
    /**
     * @brief Return end-of-options marker.
     * @return String which specifies that all remaining command
     *         line arguments are non-option arguments.
     */
    static string_type end_of_options() noexcept { return "--"; }
    /**
     * @brief Return assignment symbol.
     * @return String used to assign an argument to an option.
     */
    static string_type assignment() noexcept { return "="; }
    /**
     * @brief Format an error message indicating an invalid option.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type invalid_option_msg(const string_type& option) {
      return "invalid option: '" + option + "'";
    }
    /**
     * @brief Format an error message indicating a missing mandatory
     *        argument.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type missing_argument_msg(const string_type& option) {
      return "option '" + option + "' requires an argument";
    }
    /**
     * @brief Format an error message indicating that an unexpected
     *        argument was encountered.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type unexpected_argument_msg(const string_type& option) {
      return "option '" + option + "' does not accept arguments";
    }
  };

  /**
   * @brief Default specialization of `string_traits` for
   * `std::basic_string<wchar_t>`.
   */
  template<>
  struct string_traits<std::basic_string<wchar_t>> {
    /**
     * @brief Type of string used by the static member functions.
     */
    using string_type = std::basic_string<wchar_t>;
    /**
     * @brief Type of character used by `string_type`.
     */
    using char_type = wchar_t;
    /**
     * @brief Return the space character.
     * @return Whitespace character.
     */
    static char_type space_char() noexcept { return L' '; }
    /**
     * @brief Return the newline sequence.
     * @return String that indicates a newline.
     */
    static string_type newline() noexcept { return L"\n"; }
    /**
     * @brief Return whitespace characters.
     * @return String containing characters that should be
     *         considered as whitespace.
     */
    static string_type whitespace() noexcept { return L" \t\n\r"; }
    /**
     * @brief Return quote characters.
     * @return String containing characters used to quote
     *         command line arguments.
     */
    static string_type quotes() noexcept { return L"\"'"; }
    /**
     * @brief Return escape character.
     * @return Character that begins an escape sequence.
     */
    static char_type escape_char() noexcept { return L'\\'; }
    /**
     * @brief Return prefix for long option names.
     * @return Prefix used for long option names.
     */
    static string_type long_option_prefix() noexcept { return L"--"; }
    /**
     * @brief Return prefix for short option names.
     * @return Prefix used for short option names.
     */
    static string_type short_option_prefix() noexcept { return L"-"; }
    /**
     * @brief Return end-of-options marker.
     * @return String which specifies that all remaining command
     *         line arguments are non-option arguments.
     */
    static string_type end_of_options() noexcept { return L"--"; }
    /**
     * @brief Return assignment symbol.
     * @return String used to assign an argument to an option.
     */
    static string_type assignment() noexcept { return L"="; }
    /**
     * @brief Format an error message indicating an invalid option.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type invalid_option_msg(const string_type& option) {
      return L"invalid option: '" + option + L"'";
    }
    /**
     * @brief Format an error message indicating a missing mandatory
     *        argument.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type missing_argument_msg(const string_type& option) {
      return L"option '" + option + L"' requires an argument";
    }
    /**
     * @brief Format an error message indicating that an unexpected
     *        argument was encountered.
     * @param option Name of the option.
     * @return Formatted error message.
     */
    static string_type unexpected_argument_msg(const string_type& option) {
      return L"option '" + option + L"' does not accept arguments";
    }
  };

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
   * @tparam StringTraits
   * @parblock
   * @brief The type used to supply string information.
   *
   * Changing this parameter from its default allows you to customize
   * the strings that are used by the parser, or to allow you to use a
   * string class other than `std::string`. See `string_traits` for
   * further details.
   * @endparblock
   * @see basic_option
   * @see basic_parser_result
   * @see string_traits
   */
  template <typename StringType,
            typename StringTraits = string_traits<StringType>>
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
     * @brief Type specifying string traits.
     */
    using traits_type = StringTraits;
    /**
     * @brief Type that stores option descriptions.
     */
    using option_type = basic_option<StringType>;
    /**
     * @brief Type that stores the parsed result data.
     */
    using result_type = basic_parser_result<StringType>;

    /**
     * @brief Exception class indicating an invalid option.
     */
    class parse_error : public error {
    public:
      /**
       * @brief Constructor.
       * @param what_str C-style string describing the error.
       * @param fn_name Name of the function that threw the exception.
       * @param message Formatted message string.
       * @param option Name of the option that triggered the error (if
       *               any).
       */
      parse_error(const char* what_str, const char* fn_name,
                  const string_type& message,
                  const string_type& option = string_type{})
        : error(what_str, fn_name), m_message{message}, m_option{option} {}

      /**
       * @brief Return formatted message string.
       * @return Formatted message string describing the error.
       */
      const string_type& message() const noexcept { return m_message; }

      /**
       * @brief Return option name.
       * @return Option that triggered the error, if any.
       */
      const string_type& option() const noexcept { return m_option; }

    private:
      string_type m_message; //< Formatted message.
      string_type m_option; //< Option that triggered the error.
    };

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
     * @return `basic_parser_result` containing the parsed data.
     * @throw std::invalid_argument If an invalid option is entered or
     *                              a mandatory argument is missing.
     * @see basic_parser_result
     */
    template <typename InputIt>
    result_type parse(InputIt first, InputIt last, bool ignore_first = true) const;

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
     * @return `basic_parser_result` containing the parsed data.
     * @throw std::invalid_argument If an invalid option is entered or
     *                              a mandatory argument is missing.
     * @see basic_parser_result
     */
    result_type parse(int argc, const char_type* argv[], bool ignore_first = true) const;

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
     * @return `basic_parser_result` containing the parsed data.
     * @throw std::invalid_argument If an invalid option is entered or
     *                              a mandatory argument is missing.
     * @see basic_parser_result
     */
    result_type parse(const string_type& cmd_line, bool ignore_first = false) const;

  private:

    /**
     * @brief Determines whether an argument is an end-of-option
     *        marker.
     * @param argument Argument to check.
     * @return True if the argument is an end-of-option marker.
     */
    static bool is_end_indicator(const string_type& argument) noexcept {
      return argument == traits_type::end_of_options();
    }

    /**
     * @brief Determines whether an argument is a long command-line
     *        option.
     * @param argument Argument to check.
     * @return True if the argument begins with a long option prefix.
     */
    static bool is_long_option(const string_type& argument) noexcept {
      auto prefix = traits_type::long_option_prefix();
      return argument.size() > prefix.size() && utility::is_substr_at_pos(argument, prefix);
    }

    /**
     * @brief Determines whether an argument is a short command-line
     *        option group.
     * @param argument Argument to check.
     * @return True if the argument begins with a short option prefix.
     */
    static bool is_short_option_group(const string_type& argument) noexcept {
      auto prefix = traits_type::short_option_prefix();
      return argument.size() > prefix.size() && utility::is_substr_at_pos(argument, prefix);
    }

    /**
     * @brief Determines whether an argument is a non-option argument.
     * @param argument Argument to check.
     * @return True if the argument is a non-option argument.
     */
    static bool is_non_option(const string_type& argument) noexcept {
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
     * @param result Current `parse_result`. New entries will be added
     *               to the end.
     * @param type Will be set to the appropriate option type.
     * @throw std::invalid_argument Thrown if option is invalid or
     *                              missing a required argument.
     * @see cl_arg_type
     */
    void parse_argument(const string_type& argument,
                        result_type& result, cl_arg_type& type) const;

    /**
     * @brief Parse a group of short options.
     * @param short_names String of short option name characters to parse.
     * @param argument Option argument that was provided, if any.
     * @param has_arg Should be true if an argument was found (even an
     *                empty one).
     * @param result Current `parse_result`. New entries will be added
     *               to the end.
     * @param type Will be set to the appropriate option type.
     * @throw std::invalid_argument Thrown if option is invalid or
     *                              missing a required argument.
     * @see cl_arg_type
     */
    void parse_short_option_group(const string_type& short_names,
                                  const string_type& argument, bool has_arg,
                                  result_type& result, cl_arg_type& type) const;

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


  /* Implementation */

  /*
   * Doxygen has trouble with the trailing return types here, so we'll
   * set it to ignore these. The actual docs are in the class
   * declaration.
   */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

  template <typename StringType, typename StringTraits>
  template <typename InputIt>
  auto basic_parser<StringType, StringTraits>::parse(InputIt first, InputIt last,
                                                     bool ignore_first) const -> result_type {
    if (ignore_first && first != last)
      ++first;

    InputIt it{first};

    result_type result{};
    cl_arg_type prev_type{cl_arg_type::non_option};
    while (it != last) {
      const string_type& arg{*it};

      // If we are expecting a standalone option argument...
      if (prev_type == cl_arg_type::arg_required
          || prev_type == cl_arg_type::arg_optional) {
        // ...see if an argument was provided...
        if (!is_end_indicator(arg) && !is_long_option(arg)
            && !is_short_option_group(arg)) {
          auto& arg_info = result.back();
          arg_info.argument = arg;
          arg_info.original_text += traits_type::space_char();
          arg_info.original_text += arg;
          prev_type = cl_arg_type::non_option;
        } else { // ...if not, make sure argument was optional
          if (prev_type == cl_arg_type::arg_required) {
            const auto& opt_name = result.back().original_text;
            throw parse_error{"missing mandatory argument",
                "optionpp::basic_parser::parse",
                traits_type::missing_argument_msg(opt_name), opt_name};
          }
          prev_type = cl_arg_type::non_option;
          continue; // Continue without incrementing 'it' in order to reevaluate current token
        }
      } else if (prev_type == cl_arg_type::end_indicator) { // Ignore options
        typename result_type::item arg_info;
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
      throw parse_error{"missing mandatory argument",
          "optionpp::basic_parser::parse",
          traits_type::missing_argument_msg(opt_name), opt_name};
    }

    return result;
  }

  template <typename StringType, typename StringTraits>
  auto basic_parser<StringType, StringTraits>::parse(int argc, const char_type* argv[],
                                                     bool ignore_first) const -> result_type {
    return parse(argv, argv + argc, ignore_first);
  }

  template <typename StringType, typename StringTraits>
  auto basic_parser<StringType, StringTraits>::parse(const string_type& cmd_line,
                                                     bool ignore_first) const -> result_type {
    std::vector<string_type> container;
    utility::split(cmd_line, std::back_inserter(container),
                   StringTraits::whitespace(),
                   StringTraits::quotes(),
                   StringTraits::escape_char());
    return parse(container.begin(), container.end(), ignore_first);
  }

#endif // DOXYGEN_SHOULD_SKIP_THIS

  template <typename StringType, typename StringTraits>
  void basic_parser<StringType, StringTraits>::parse_argument(const string_type& argument,
                                                              result_type& result,
                                                              cl_arg_type& type) const {
    // Check for end-of-option marker
    if (is_end_indicator(argument)) {
      type = cl_arg_type::end_indicator;
      return;
    }

    // Split string into components
    string_type option_specifier;
    string_type option_argument;
    bool assignment_found = false;
    auto pos = argument.find(traits_type::assignment());
    if (pos == string_type::npos)
      option_specifier = argument;
    else {
      assignment_found = true;
      option_specifier = argument.substr(0, pos);
      pos += traits_type::assignment().size();
      option_argument = argument.substr(pos);

      // Check for bad syntax like -= and --=
      if (option_specifier == traits_type::short_option_prefix()
          || option_specifier == traits_type::long_option_prefix()) {
        option_specifier += traits_type::assignment();
        throw parse_error{"invalid option",
            "optionpp::basic_parser::parse_argument",
            traits_type::invalid_option_msg(option_specifier),
            option_specifier};
      }
    }

    // Check option type
    typename result_type::item opt_info;
    if (is_long_option(option_specifier)) {
      // Extract option name
      string_type option_name = option_specifier.substr(traits_type::long_option_prefix().size());

      // Look up option info
      auto it = std::find_if(m_options.begin(), m_options.end(),
                             [&](const option_type& o) { return o.long_name() == option_name; });
      if (it == m_options.end())
        throw parse_error{"invalid option",
            "optionpp::basic_parser::parse_argument",
            traits_type::invalid_option_msg(option_specifier), option_specifier};

      // Does this option take an argument?
      if (!it->argument_name().empty()) {
        if (!assignment_found) { // No arg was found, caller should look for it
          if (it->is_argument_required())
            type = cl_arg_type::arg_required;
          else
            type = cl_arg_type::arg_optional;
        } else { // Found an argument
          type = cl_arg_type::no_arg; // Caller should not look for argument
          opt_info.argument = option_argument;
        }
      } else { // Does not take an argument
        if (assignment_found) // Found an argument where there should be none
          throw parse_error{"unexpected argument",
                              "optionpp::basic_parser::parse_argument",
                              traits_type::unexpected_argument_msg(option_specifier),
                              option_specifier};
        type = cl_arg_type::no_arg;
      }
      opt_info.original_text = argument;
      opt_info.is_option = true;
      opt_info.long_name = option_name;
      opt_info.short_name = it->short_name();
      result.push_back(std::move(opt_info));
    } else if (is_short_option_group(option_specifier)) { // Short options
      parse_short_option_group(option_specifier.substr(traits_type::short_option_prefix().size()),
                               option_argument, assignment_found,
                               result, type);
    } else {
      // If we get here, this argument is not an option
      type = cl_arg_type::non_option;
      opt_info.original_text = argument;
      opt_info.is_option = false;
      result.push_back(std::move(opt_info));
    }
  }

  template <typename StringType, typename StringTraits>
  void basic_parser<StringType, StringTraits>
  ::parse_short_option_group(const string_type& short_names,
                             const string_type& argument, bool has_arg,
                             result_type& result, cl_arg_type& type) const {
    for (decltype(short_names.size()) pos = 0; pos != short_names.size(); ++pos) {
      // Look up option info
      auto it = std::find_if(m_options.begin(), m_options.end(),
                             [&](const option_type& o) { return o.short_name() == short_names[pos]; });
      if (it == m_options.end()) {
        auto opt_name = traits_type::short_option_prefix();
        opt_name.push_back(short_names[pos]);
        throw parse_error{"invalid option",
            "optionpp::basic_parser::parse_short_option_group",
            traits_type::invalid_option_msg(opt_name), opt_name};
      }

      typename result_type::item opt_info;
      opt_info.original_text = traits_type::short_option_prefix();
      opt_info.original_text.push_back(short_names[pos]);
      opt_info.is_option = true;
      opt_info.long_name = it->long_name();
      opt_info.short_name = short_names[pos];

      // Check if option takes an argument
      if (!it->argument_name().empty()) {
        if (pos + 1 < short_names.size()) {
          // This isn't the last option, so the rest of the string is an argument
          opt_info.argument = short_names.substr(pos + 1);
          if (has_arg) {
            // The assignment symbol is actually part of the argument
            opt_info.argument += traits_type::assignment();
            opt_info.argument += argument;
          }
          opt_info.original_text += opt_info.argument;
          result.push_back(std::move(opt_info));
          type = cl_arg_type::no_arg;
          break;
        } else {
          // This is the last option and it needs an argument
          if (has_arg) {
            opt_info.original_text += traits_type::assignment();
            opt_info.original_text += argument;
            opt_info.argument = argument;
            type = cl_arg_type::no_arg;
          } else if (it->is_argument_required()) {
            type = cl_arg_type::arg_required;
          } else {
            type = cl_arg_type::arg_optional;
          }
          result.push_back(std::move(opt_info));
          break;
        }
      }

      // If we make it here, then the current option does not take an argument
      if (pos + 1 == short_names.size() && has_arg) {
        auto opt_name = traits_type::short_option_prefix();
        opt_name.push_back(short_names[pos]);
        throw parse_error{"option does not accept arguments",
            "optionpp::basic_parser::parse_short_option_group",
            traits_type::unexpected_argument_msg(opt_name), opt_name};
      }

      result.push_back(std::move(opt_info));
      type = cl_arg_type::no_arg;
      opt_info = typename result_type::item{};
    } // End for loop
  }

} // End namespace

#endif
