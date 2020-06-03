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

  namespace utility {

    /**
     * @brief Split a string over delimiters into nonempty substrings.
     *
     * Splits a string into nonempty substring components using
     * `delims` as token delimiters (though a component can be
     * nonempty if it is quoted; see below).
     *
     * Delimiters appearing within quote characters (as specified by
     * the `quotes` parameter) are ignored. Within quotes, an escape
     * character can be used to escape a quote symbol.
     *
     * @tparam StringType The type of string (should typically be
     *                    deduced).
     * @tparam OutputIt Type of output iterator (typically deduced).
     * @param str The string to split.
     * @param dest An output iterator specifying where the tokens
     *             should be written.
     * @param delims String containing the characters to be used as
     *               delimiters.
     * @param quotes String containing the allowed quote characters.
     * @param escape_char Character to use as escape character.
     */
    template <typename StringType, typename OutputIt>
    void split(const StringType& str, OutputIt dest,
               const StringType& delims = " \t\n\r",
               const StringType& quotes = "\"\'",
               typename StringType::value_type escape_char = '\\');

    /**
     * @brief Split a string over delimiters into nonempty substrings.
     *
     * Splits a string into nonempty substring components using
     * `delims` as token delimiters. See the more general
     * `split(const StringType&, OutputIt, const StringType&, const StringType&)`
     * overload for further details.
     *
     * @tparam OutputIt Type of output iterator (typically deduced).
     * @param str The string to split.
     * @param dest An output iterator specifying where the tokens
     *             should be written.
     * @param delims String containing the characters to be used as
     *               delimiters.
     * @param quotes String containing the allowed quote characters.
     * @param escape_char Character to use as escape character.
     */
    template <typename OutputIt>
    void split(const char* str, OutputIt dest,
               const char* delims = " \t\r\n", const char* quotes = "\"\'",
               char escape_char = '\\') {
      split<std::string>(str, dest, delims, quotes, escape_char);
    }


    /* Implementation */

    template <typename StringType, typename OutputIt>
    void split(const StringType& str, OutputIt dest,
               const StringType& delims, const StringType& quotes,
               typename StringType::value_type escape_char) {
      using size_type = decltype(str.size());

      size_type pos{0};
      bool escape_next{false};
      bool in_quotes{false};
      size_type quote_index{0};
      StringType cur_token{};
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
          if (escape_next || delims.find(str[pos]) == StringType::npos) {
            if (!escape_next && str[pos] == escape_char)
              escape_next = true;
            else if (escape_next) {
              cur_token.push_back(str[pos]);
              escape_next = false;
            } else {
              // Look for quote
              quote_index = quotes.find(str[pos]);
              if (quote_index != StringType::npos)
                in_quotes = true;
              else
                cur_token.push_back(str[pos]);
            }
          } else { // We hit a delimiter
            if (!cur_token.empty())
              *dest++ = cur_token;
            cur_token.clear();
          }
        }

        ++pos;
      }

      // Do we have any characters leftover?
      if (!cur_token.empty())
        *dest++ = cur_token;
    }

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
    template <typename StringType>
    inline bool is_substr_at_pos(const StringType& str, const StringType& substr,
                                 typename StringType::size_type pos = 0) noexcept {
      if (pos + substr.size() > str.size())
        return false;

      for (decltype(pos) i = 0; i < substr.size(); ++i) {
        if (str[pos + i] != substr[i])
          return false;
      }

      return true;
    }

  } // End namespace

} // End namespace

#endif
