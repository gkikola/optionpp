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
#include <string>
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
     * @throw std::invalid_argument if an invalid option is entered or
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
     * @throw std::invalid_argument if an invalid option is entered or
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
     * @throw std::invalid_argument if an invalid option is entered or
     *                              a mandatory argument is missing.
     * @see basic_parser_result
     */
    result_type parse(const string_type& cmd_line, bool ignore_first = false) const;

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
    typename result_type::item cur_item{};
    auto cur_option = m_options.end();
    bool expecting_arg{false};
    bool ignore_options{false};
    while (it != last) {
      const string_type& arg = *it;

      // If we are expecting a standalone option argument...
      if (expecting_arg) {
        expecting_arg = false;
        // ...see if an argument was provided...
        if (!utility::is_substr_at_pos(arg, traits_type::long_option_prefix())
            && !utility::is_substr_at_pos(arg, traits_type::short_option_prefix())) {
          cur_item.argument = arg;
          cur_item.original_text += traits_type::space_char();
          cur_item.original_text += arg;
          result.push_back(cur_item);
          cur_item = typename result_type::item{};
          cur_option = m_options.end();
        } else { // ...if not, make sure argument was optional
          if (cur_option->is_argument_required())
            throw std::invalid_argument{"[optionpp] missing mandatory argument"};
          result.push_back(cur_item);
          cur_item = typename result_type::item{};
          cur_option = m_options.end();
          continue; // Continue without incrementing 'it' in order to reevaluate current token
        }
      } else { // Not expecting a standalone option argument
        // See if this is the end-of-options marker
        if (!ignore_options && arg == traits_type::end_of_options()) {
          ignore_options = true;
        } else {
          // If options are being ignored, just treat this as a non-option argument
          if (ignore_options) {
            cur_item.original_text = arg;
            cur_item.is_option = false;
            result.push_back(cur_item);
            cur_item = typename result_type::item{};
          } else if (utility::is_substr_at_pos(arg, traits_type::long_option_prefix())) {
            // Long option was found
            // expecting_arg = read_long_option(arg, result, cur_item, cur_option);
          } else if (utility::is_substr_at_pos(arg, traits_type::short_option_prefix())) {
            // Short options found
            // expecting_arg = read_short_options(arg, result, cur_item, cur_option);
          } else { // This is not an option argument
            cur_item.original_text = arg;
            cur_item.is_option = false;
            result.push_back(cur_item);
            cur_item = typename result_type::item{};
          }
        }
      }

      ++it;
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

} // End namespace

#endif
