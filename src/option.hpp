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
 * @brief Header file for `basic_option` class.
 */

#ifndef OPTIONPP_OPTION_HPP
#define OPTIONPP_OPTION_HPP

#include <string>

namespace optionpp {

  /**
   * @brief Flags for option arguments.
   */
  enum class argument_type { optional, //< Argument is optional
                             required //< Argument is mandatory
  };

  /**
   * @brief Describes a valid program command-line option.
   *
   * A `basic_option` can hold information about a program option that
   * the user can pass through the command line. Among this information,
   * the most important are the long name and the short name.
   *
   * If an option has a long name, then the user can specify that option
   * using a double-hyphen. For example, `--version` would specify an option
   * with a long name of `version`.
   *
   * If an option has a short name, then the user can specify the
   * option with a single hyphen. For example `-v` would specify an
   * option with a short name of `v`. Short names are always a single
   * character, and can be combined in a single command-line
   * argument. For example, the command-line argument `-xvf` specifies
   * three different options: one with a short name of `x`, another
   * with short name `v`, and another with `f`.
   *
   * Program options can accept arguments. These can be mandatory or
   * optional. The argument name (set in the `name` parameter of the
   * `argument` method) is used in the help text and usage string.
   *
   * A description and a group name can be set as well. These are used
   * in generating the program help text.
   *
   * Note that many of the methods in the class return a reference to
   * the current instance. This allows for convenient chaining. For
   * example, to create an option with a long name of `help`, a short
   * name of `?`, and an appropriate description, one could write:
   * ```
   * basic_option help_opt{};
   * help_opt.long_name("help").short_name('?').description("Show help text.");
   * ```
   *
   * @tparam StringType
   * @parblock
   * @brief The type of string used for storing option information.
   *
   * Usually this is `std::string` (i.e. `std::basic_string<char>`),
   * which is used in the `option` type alias, but other string
   * variants may be used instead.
   * @endparblock
   */
  template <typename StringType>
  class basic_option {
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
     * @brief Default constructor.
     *
     * Constructs an option without initial information.
     */
    basic_option() noexcept(noexcept(string_type())) {}
    /**
     * @brief Construct an option with a given short name.
     *
     * Other fields are left empty.
     *
     * @param short_name The short name for the option.
     */
    basic_option(char_type short_name) : m_short_name{short_name} {}
    /**
     * @brief Construct an option with given names.
     *
     * The argument, description, and group fields are left empty.
     *
     * @param long_name The long name for the option.
     * @param short_name The short name for the option.
     */
    basic_option(string_type long_name, char_type short_name = char_type{})
      : m_long_name{long_name}, m_short_name{short_name} {}

    /**
     * @brief Set the option's long name.
     * @param name The long name to use.
     * @return Reference to the current instance (for chaining calls).
     */
    basic_option& long_name(const string_type& name) {
      m_long_name = name;
      return *this;
    }
    /**
     * @brief Retrieve the option's long name.
     * @return The long name for the option.
     */
    const string_type& long_name() const noexcept { return m_long_name; }

    /**
     * @brief Set the option's short name.
     * @param name The character to use as the short name.
     * @return Reference to the current instance (for chaining calls).
     */
    basic_option& short_name(char_type name) noexcept {
      m_short_name = name;
      return *this;
    }
    /**
     * @brief Retrieve the option's short name.
     * @return The single-character short name for the option.
     */
    char_type short_name() const noexcept { return m_short_name; }

    /**
     * @brief Set the option's argument information.
     *
     * The name is used in the program help text and usage string. The
     * type specifies whether the argument is optional
     * (`argument_type::optional`) or mandatory
     * (`argument_type::mandatory`).
     *
     * @param name Name of the argument (usually all uppercase).
     * @param type Whether the argument is optional or mandatory.
     * @return Reference to the current instance (for chaining calls).
     */
    inline basic_option& argument(const string_type& name,
                                  argument_type type = argument_type::optional);
    /**
     * @brief Retrieve the option's argument name.
     *
     * This is the name that is used in the help text.
     *
     * @return The name of the argument.
     */
    const string_type& argument_name() const noexcept { return m_arg_name; }
    /**
     * @brief Return true if the argument is mandatory.
     * @return True if the argument is required and false if it is optional.
     */
    bool is_argument_required() const noexcept { return m_arg_required; }

    /**
     * @brief Set the option description.
     *
     * This description is used in generating the program help text.
     *
     * @param desc Description of the option.
     * @return Reference to the current instance (for chaining calls).
     */
    basic_option& description(const string_type& desc) {
      m_desc = desc;
      return *this;
    }
    /**
     * @brief Retrieve the option description.
     * @return Option description, used in generating program help text.
     */
    const string_type& description() const noexcept { return m_desc; }

    /**
     * @brief Set the option group.
     *
     * Options that are in the same group will be grouped together in
     * the program's help text.
     *
     * @param group_name Name of the group to which the option belongs.
     * @return Reference to the current instance (for chaining calls).
     */
    basic_option& group(const string_type& group_name) {
      m_group_name = group_name;
      return *this;
    }
    /**
     * @brief Retrieve the option group.
     *
     * Options that are in the same group will be grouped together in
     * the program help text.
     *
     * @return Name of the group to which the option belongs, if any.
     */
    const string_type& group() const noexcept { return m_group_name; }

  private:
    string_type m_long_name; //< The long name
    char_type m_short_name{}; //< The short name
    string_type m_arg_name; //< The name of the argument (for help text)
    bool m_arg_required{}; //< True if argument is mandatory, false if optional
    string_type m_desc; //< Description of option (for help text)
    string_type m_group_name; //< Name of option group
  };


  /* Implementation */

  template <typename S>
  basic_option<S>&
  basic_option<S>::argument(const string_type& name, argument_type type) {
    m_arg_name = name;
    m_arg_required = type == argument_type::required;

    return *this;
  }

  /**
   * @brief Type alias for typical usage of `basic_option`.
   */
  using option = basic_option<std::string>;

} // End namespace

#endif
