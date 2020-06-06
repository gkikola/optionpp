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
 * @brief Source file for `option` class implementation.
 */

#include <optionpp/option.hpp>

#include <optionpp/error.hpp>

using namespace optionpp;

option::option(const std::string& long_name, char short_name,
               const std::string& description,
               const std::string& arg_name, bool arg_required) :
  m_long_name{long_name}, m_short_name{short_name},
  m_desc{description}, m_arg_name{arg_name},
  m_arg_required{arg_required} {}

option& option::argument(const std::string& name, bool required) {
  m_arg_name = name;
  m_arg_required = required;

  return *this;
}

option& option::bind_bool(bool* var) noexcept {
  m_is_option_set = var;
  if (var)
    *var = false;
  return *this;
}

option& option::bind_string(std::string* var) noexcept {
  if (var && m_arg_name.empty())
    m_arg_name = "STRING";
  m_arg_type = string_arg;
  m_bound_variable = var;
  return *this;
}

option& option::bind_int(int* var) noexcept {
  if (var && m_arg_name.empty())
    m_arg_name = "INTEGER";
  m_arg_type = int_arg;
  m_bound_variable = var;
  return *this;
}

option& option::bind_uint(unsigned int* var) noexcept {
  if (var && m_arg_name.empty())
    m_arg_name = "INTEGER";
  m_arg_type = uint_arg;
  m_bound_variable = var;
  return *this;
}

option& option::bind_double(double* var) noexcept {
  if (var && m_arg_name.empty())
    m_arg_name = "NUMBER";
  m_arg_type = double_arg;
  m_bound_variable = var;
  return *this;
}

void option::write_bool(bool value) const noexcept {
  if (m_is_option_set)
    *m_is_option_set = value;
}

void option::write_string(const std::string& value) const {
  if (m_arg_type != string_arg || !m_bound_variable)
    throw type_error{"option '" + name() + "' does not accept a string argument",
                       "optionpp::option::write_string"};
  *static_cast<std::string*>(m_bound_variable) = value;
}

void option::write_int(int value) const {
  if (m_arg_type != int_arg || !m_bound_variable)
    throw type_error{"option '" + name() + "' does not accept an int argument",
                       "optionpp::option::write_int"};
  *static_cast<int*>(m_bound_variable) = value;
}

void option::write_uint(unsigned int value) const {
  if (m_arg_type != uint_arg || !m_bound_variable)
    throw type_error{"option '" + name() + "' does not accept an unsigned int argument",
                       "optionpp::option::write_uint"};
  *static_cast<unsigned int*>(m_bound_variable) = value;
}

void option::write_double(double value) const {
  if (m_arg_type != double_arg || !m_bound_variable)
    throw type_error{"option '" + name() + "' does not accept a double argument",
                       "optionpp::option::write_double"};
  *static_cast<double*>(m_bound_variable) = value;
}
