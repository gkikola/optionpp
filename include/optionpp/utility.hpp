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
 * @brief Header file for various utility functions.
 */

#ifndef OPTIONPP_UTILITY_HPP
#define OPTIONPP_UTILITY_HPP

#include <stdexcept>
#include <string>

namespace optionpp {

  /**
   * @brief Namespace for utility functions.
   */
  namespace utility {

    /**
     * @brief Split a string over delimiters into substrings.
     *
     * Splits a string into substring components using `delims` as
     * token delimiters. If `allow_empty` is false, then empty
     * components are skipped, unless enclosed in quotes.
     *
     * Delimiters appearing within quote characters (as specified by
     * the `quotes` parameter) are ignored. Within quotes, an escape
     * character can be used to escape a quote symbol.
     *
     * @tparam OutputIt Type of output iterator (typically deduced).
     * @param str The string to split.
     * @param dest An output iterator specifying where the tokens
     *             should be written.
     * @param delims String containing the characters to be used as
     *               delimiters.
     * @param quotes String containing the allowed quote characters.
     * @param escape_char Character to use as escape character.
     * @param allow_empty If true, consecutive delimiters will
     *                    produce empty substrings.
     */
    template <typename OutputIt>
    void split(const std::string& str, OutputIt dest,
               const std::string& delims = " \t\n\r",
               const std::string& quotes = "\"\'",
               char escape_char = '\\',
               bool allow_empty = false);

    /**
     * @brief Perform word-wrapping on a string.
     *
     * This function will take each line in the string and insert
     * additional newlines in order to limit the maximum line length.
     * Lines are only split between words unless there is no other
     * choice.
     *
     * To get an unlimited line length, set `line_len <= 0`.
     *
     * The text can also be indented a certain number of spaces. The
     * total line length includes the indentation.
     *
     * @param str Text to wrap.
     * @param line_len Maximum desired line length, if any.
     * @param indent Number of spaces to indent each line.
     * @return Resulting word-wrapped string.
     */
    std::string wrap_text(const std::string& str,
                          int line_len = 79,
                          int indent = 0);

    /**
     * @brief Perform word-wrapping on a string.
     *
     * This function will take each line in the string and insert
     * additional newlines in order to limit the maximum line length.
     * Lines are only split between words unless there is no other
     * choice.
     *
     * To get an unlimited line length, set `line_len <= 0`.
     *
     * The text can also be indented a certain number of spaces. The
     * total line length includes the indentation.
     *
     * @param str Text to wrap.
     * @param line_len Maximum desired line length, if any.
     * @param indent Number of spaces to indent each line after the
     *               first one.
     * @param first_line_indent Number of spaces to indent the first
     *                          line.
     * @return Resulting word-wrapped string.
     */
    std::string wrap_text(const std::string& str,
                          int line_len,
                          int indent,
                          int first_line_indent);

    /**
     * @brief Determine if a string occurs within another string at a
     * particular position.
     *
     * This simple function checks `str` to see if `substr` is
     * at the specified position. For example, the call
     * ```
     * is_substr_at_pos("Hello world", "wor", 6)
     * ```
     * should return true because the string "wor" does occur within
     * "Hello world" at index 6.
     *
     * @param str String in which substring is to be found.
     * @param substr Substring to match.
     * @param pos The position in `str` at which `substr` should be
     *            found.
     * @return True if `substr` occurs at index `pos` in `str`, and
     *         false otherwise.
     */
    bool is_substr_at_pos(const std::string& str, const std::string& substr,
                          std::string::size_type pos = 0) noexcept;

  } // End namespace

} // End namespace


/* Implementation */

template <typename OutputIt>
void optionpp::utility::split(const std::string& str, OutputIt dest,
                              const std::string& delims,
                              const std::string& quotes,
                              char escape_char,
                              bool allow_empty) {
  using size_type = decltype(str.size());
  size_type pos{0};
  bool escape_next{false};
  bool in_quotes{false};
  size_type quote_index{0};
  std::string cur_token;
  while (pos < str.size()) {
    if (in_quotes) {
      // Look for closing quote, unless we are escaping
      if (escape_next || str[pos] != quotes[quote_index]) {
        if (!escape_next && str[pos] == escape_char)
          escape_next = true;
        else {
          cur_token.push_back(str[pos]);
          escape_next = false;
        }
      } else { // Found closing quote
        in_quotes = false;
      }
    } else {
      // Look for delimiter
      if (escape_next || delims.find(str[pos]) == std::string::npos) {
        if (!escape_next && str[pos] == escape_char)
          escape_next = true;
        else if (escape_next) {
          cur_token.push_back(str[pos]);
          escape_next = false;
        } else {
          // Look for quote
          quote_index = quotes.find(str[pos]);
          if (quote_index != std::string::npos)
            in_quotes = true;
          else
            cur_token.push_back(str[pos]);
        }
      } else { // We hit a delimiter
        if (!cur_token.empty() || allow_empty)
          *dest++ = cur_token;
        cur_token.clear();
      }
    }

    ++pos;
  }

  // Do we have any characters leftover?
  if (!cur_token.empty() || allow_empty)
    *dest++ = cur_token;
}

#endif
