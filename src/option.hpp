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
 * @brief Header file for `option` class.
 */

#ifndef OPTIONPP_OPTION_HPP
#define OPTIONPP_OPTION_HPP

#include <string>

namespace optionpp {

  /**
   * @brief Describes a valid program command-line option.
   *
   * A `option` can hold information about a program option that the
   * user can pass through the command line. Among this information,
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
   * option help_opt{};
   * help_opt.long_name("help").short_name('?').description("Show help text.");
   * ```
   */
  class option {
  public:

    /**
     * @brief Holds the possible argument types.
     */
    enum arg_type { string_arg, //< Indicates a string argument.
                    int_arg, //< Indicates an integer argument.
                    uint_arg, //< Indicates an unsigned int argument.
                    double_arg //< Indicates a floating-point argument.
    };

    /**
     * @brief Default constructor.
     *
     * Constructs an option without initial information.
     */
    option() noexcept {}
    /**
     * @brief Construct an option with a given short name.
     *
     * Other fields are left empty.
     *
     * @param short_name The short name for the option.
     */
    option(char short_name) : m_short_name{short_name} {}
    /**
     * @brief Construct an option with given names.
     *
     * The argument, description, and group fields are left empty.
     *
     * @param long_name The long name for the option.
     * @param short_name The short name for the option.
     */
    option(std::string long_name, char short_name = '\0')
      : m_long_name{long_name}, m_short_name{short_name} {}

    /**
     * @brief Sets the long and short name for the option.
     * @param long_name Long name form for the option.
     * @param short_name Single-character short option name.
     * @return Reference to the current instance (for chaining calls).
     */
    option& name(const std::string& long_name, char short_name = '\0') {
      m_long_name = long_name;
      m_short_name = short_name;
      return *this;
    }

    /**
     * @brief Returns a name for the option.
     *
     * This will be the long name if it exists, otherwise the short
     * name is used. If neither name is set, returns an empty string.
     *
     * @return Option name.
     */
    std::string name() const noexcept {
      if (!m_long_name.empty())
        return m_long_name;
      else if (m_short_name != '\0')
        return std::string{m_short_name};
      else
        return "";
    }

    /**
     * @brief Set the option's long name.
     * @param name The long name to use.
     * @return Reference to the current instance (for chaining calls).
     */
    option& long_name(const std::string& name) {
      m_long_name = name;
      return *this;
    }
    /**
     * @brief Retrieve the option's long name.
     * @return The long name for the option.
     */
    const std::string& long_name() const noexcept { return m_long_name; }

    /**
     * @brief Set the option's short name.
     * @param name The character to use as the short name.
     * @return Reference to the current instance (for chaining calls).
     */
    option& short_name(char name) noexcept {
      m_short_name = name;
      return *this;
    }
    /**
     * @brief Retrieve the option's short name.
     * @return The single-character short name for the option.
     */
    char short_name() const noexcept { return m_short_name; }

    /**
     * @brief Set the option's argument information.
     *
     * The name is used in the program help text and usage string.
     *
     * @param name Name of the argument (usually all uppercase).
     * @param required True if the option is mandatory, false if it
     *                 is optional.
     * @return Reference to the current instance (for chaining calls).
     */
    option& argument(const std::string& name,
                     bool required = true);
    /**
     * @brief Retrieve the option's argument name.
     *
     * This is the name that is used in the help text.
     *
     * @return The name of the argument.
     */
    const std::string& argument_name() const noexcept { return m_arg_name; }
    /**
     * @brief Return true if the argument is mandatory.
     * @return True if the argument is required and false if it is optional.
     */
    bool is_argument_required() const noexcept { return m_arg_required; }
    /**
     * @brief Return the type of argument.
     * @return Expected argument type.
     */
    arg_type argument_type() const noexcept { return m_arg_type; }

    /**
     * @brief Designates a location to store whether the option was
     *        set.
     *
     * If the option is encountered on the command line, the bound
     * value is set to true. Otherwise, it is set to false.
     *
     * @param var Address of boolean to set.
     * @return Reference to the current instance (for chaining calls).
     */
    option& bind_bool(bool* var) noexcept;
    /**
     * @brief Designates that the option should take a string argument
     *        which should be stored in `*var`.
     * @param var Address of string to receive argument value.
     * @return Reference to the current instance (for chaining calls).
     */
    option& bind_string(std::string* var) noexcept;
    /**
     * @brief Designates that the option should take an integer
     *        argument which should be stored in `*var`.
     *
     * If a non-integer argument is given, then the `parser` will
     * throw a `parse_error` exception.
     *
     * @param var Address of integer to receive argument value.
     * @return Reference to the current instance (for chaining calls).
     */
    option& bind_int(int* var) noexcept;
    /**
     * @brief Designates that the option should take an unsigned
     *        integer argument which should be stored in `*var`.
     *
     * If a non-integer argument is given, or if a negative integer is
     * encountered, then the `parser` will throw a `parse_error`
     * exception.
     *
     * @param var Address of unsigned int to receive argument value.
     * @return Reference to the current instance (for chaining calls).
     */
    option& bind_uint(unsigned int* var) noexcept;
    /**
     * @brief Designates that the option should take a
     * double-precision floating point argument which should be stored
     * in `*var`.
     *
     * If a non-numeric argument is given, then the `parser` will
     * throw a `parse_error` exception.
     *
     * @param var Address of double to receive argument value.
     * @return Reference to the current instance (for chaining calls).
     */
    option& bind_double(double* var) noexcept;
    /**
     * @brief Returns true if a variable has been bound to the
     *        option's argument.
     *
     * Note that binding a boolean value to the option does not affect
     * the return value of this method.
     *
     * @return True if a variable is bound, false otherwise.
     */
    bool has_bound_argument_variable() const noexcept { return m_bound_variable; }
    /**
     * @brief Writes to the bound boolean variable that was specified
     * in `bind_bool`.
     *
     * It is safe to call this function even when no boolean variable
     * is currently bound. Doing so is effectively a no-op.
     *
     * @param value Value to write to the bound bool variable.
     */
    void write_bool(bool value) const noexcept;
    /**
     * @brief Writes to the bound string variable that was specified
     * in `bind_string`.
     *
     * This method should not be called unless a string variable was
     * previously bound. You can use the `argument_type` method to
     * check what type of argument the option expects.
     *
     * @throw type_error If no string variable was bound.
     * @param value Value to write to the bound string variable.
     */
    void write_string(const std::string& value) const;
    /**
     * @brief Writes to the bound integer variable that was specified
     * in `bind_int`.
     *
     * This method should not be called unless an int variable was
     * previously bound. You can use the `argument_type` method to
     * check what type of argument the option expects.
     *
     * @throw type_error If no int variable was bound.
     * @param value Value to write to the bound int variable.
     */
    void write_int(int value) const;
    /**
     * @brief Writes to the bound unsigned integer variable that was
     * specified in `bind_uint`.
     *
     * This method should not be called unless an unsigned int
     * variable was previously bound. You can use the `argument_type`
     * method to check what type of argument the option expects.
     *
     * @throw type_error If no unsigned int variable was bound.
     * @param value Value to write to the bound unsigned int variable.
     */
    void write_uint(unsigned int value) const;
    /**
     * @brief Writes to the bound double variable that was specified
     * in `bind_double`.
     *
     * This method should not be called unless a double variable was
     * previously bound. You can use the `argument_type` method to
     * check what type of argument the option expects.
     *
     * @throw type_error If no double variable was bound.
     * @param value Value to write to the bound double variable.
     */
    void write_double(double value) const;

    /**
     * @brief Set the option description.
     *
     * This description is used in generating the program help text.
     *
     * @param desc Description of the option.
     * @return Reference to the current instance (for chaining calls).
     */
    option& description(const std::string& desc) {
      m_desc = desc;
      return *this;
    }
    /**
     * @brief Retrieve the option description.
     * @return Option description, used in generating program help text.
     */
    const std::string& description() const noexcept { return m_desc; }

    /**
     * @brief Set the option group.
     *
     * Options that are in the same group will be grouped together in
     * the program's help text.
     *
     * @param group_name Name of the group to which the option belongs.
     * @return Reference to the current instance (for chaining calls).
     */
    option& group(const std::string& group_name) {
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
    const std::string& group() const noexcept { return m_group_name; }

  private:
    std::string m_long_name; //< The long name.
    char m_short_name{'\0'}; //< The short name.
    std::string m_desc; //< Description of option (for help text).
    std::string m_group_name; //< Name of option group.

    std::string m_arg_name; //< The name of the argument (for help text).
    bool m_arg_required{false}; //< True if argument is mandatory, false if optional.
    arg_type m_arg_type{string_arg}; //< Type of argument that is expected.
    bool* m_is_option_set = nullptr; //< Pointer to value to hold whether the option was set.
    void* m_bound_variable = nullptr; //< Pointer to hold argument value.
  };

} // End namespace

#endif
