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
 * @brief Header file for exception classes.
 */

#ifndef OPTIONPP_ERROR_HPP
#define OPTIONPP_ERROR_HPP

#include <stdexcept>
#include <string>

namespace optionpp {

  /**
   * @brief Base class for library exceptions.
   */
  class error : public std::logic_error {
  public:
    /**
     * @brief Constructor.
     * @param msg Description of the error.
     * @param fn_name Name of the function in which error occurred.
     */
    error(const std::string& msg, const std::string& fn_name)
      : logic_error(msg), m_function{fn_name} {}

    /**
     * @brief Return the name of function that threw the exception.
     * @return Name of function that threw the exception.
     */
    const std::string& function() const { return m_function; }

  private:
    std::string m_function; //< Function in which error occurred.
  };

  /**
   * @brief Exception indicating out of range access.
   */
  class out_of_range : public error {
  public:
    /**
     * @brief Constructor.
     * @param msg Description of the error.
     * @param fn_name Name of the function in which error occurred.
     */
    out_of_range(const std::string& msg, const std::string& fn_name)
      : error(msg, fn_name) {}
  };

  /**
   * @brief Exception indicating an invalid iterator was dereferenced.
   */
  class bad_dereference : public error {
  public:
    /**
     * @brief Constructor.
     * @param msg Description of the error.
     * @param fn_name Name of the function in which error occurred.
     */
    bad_dereference(const std::string& msg,
                    const std::string& fn_name)
      : error(msg, fn_name) {}
  };

  /**
   * @brief Exception indicating a type error.
   */
  class type_error : public error {
  public:
    /**
     * @brief Constructor.
     * @param msg Description of the error.
     * @param fn_name Name of the function in which error occurred.
     */
    type_error(const std::string& msg, const std::string& fn_name)
      : error(msg, fn_name) {}
  };

} // End namespace

#endif
