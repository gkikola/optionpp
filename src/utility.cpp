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
 * @brief Source file for implementation of utility functions.
 */

#include <optionpp/utility.hpp>

#include <cctype>
#include <iterator>
#include <vector>

namespace optionpp {
  namespace utility {

    /**
     * @brief Determine if a character is whitespace.
     * @param c Character to check.
     * @return True if `c` is a whitespace character and false
     *         otherwise.
     */
    bool is_space(char c) {
      return std::isspace(static_cast<unsigned char>(c));
    }

    /**
     * @brief Performs word-wrapping for a single line of text.
     *
     * This is a helper function for `wrap_text`.
     *
     * @param str Text to wrap.
     * @param line_len Maximum desired line length, if any.
     * @param indent Number of spaces to indent each line.
     * @param first_line_indent Number of spaces to indent the
     *                          first line.
     * @return Resulting word-wrapped string.
     */
    std::string wrap_line(const std::string& str,
                          int line_len,
                          int indent,
                          int first_line_indent) {
      // Check for unlimited length
      if (line_len <= 0)
        return std::string(first_line_indent, ' ') + str;

      // Validate indentation
      if (indent < 0)
        indent = 0;
      else if (indent > line_len - 1)
        indent = line_len - 1;
      if (first_line_indent < 0)
        first_line_indent = 0;
      else if (first_line_indent > line_len - 1)
        first_line_indent = line_len - 1;

      std::string result;
      std::string::size_type pos{0};

      while (pos < str.size()) {
        int cur_indent = result.empty() ? first_line_indent : indent;
        auto start = pos;

        // After the first line, new lines should start at non-whitespace
        // characters
        if (!result.empty()) {
          while (start < str.size() && is_space(str[start]))
            ++start;
        }

        // Find ideal end point
        auto end = start + line_len - cur_indent;
        if (end > str.size())
          end = str.size();

        // We don't want to split in the middle of a word unless we don't
        // have a choice
        if (end < str.size()) {
          auto word_start = end;
          while (word_start > start && !is_space(str[word_start]))
            --word_start;

          if (word_start > start)
            end = word_start;
        }

        // Mark position of next line
        pos = end;

        // We don't want trailing whitespace
        while (end > start && is_space(str[end - 1]))
          --end;

        // Add line to result
        if (end > start) {
          if (!result.empty())
            result.push_back('\n');
          if (cur_indent > 0)
            result += std::string(cur_indent, ' ');
          result += str.substr(start, end - start);
        }
      }

      return result;
    }

    std::string wrap_text(const std::string& str,
                          int line_len,
                          int indent) {
      return wrap_text(str, line_len, indent, indent);
    }

    std::string wrap_text(const std::string& str,
                          int line_len,
                          int indent,
                          int first_line_indent) {
      std::vector<std::string> lines;
      split(str, std::back_inserter(lines), "\n", "", '\0', true);

      std::string result;
      for (const auto& line : lines) {
        if (!result.empty())
          result.push_back('\n');
        result += wrap_line(line, line_len, indent, first_line_indent);
        first_line_indent = indent;
      }

      return result;
    }

    bool is_substr_at_pos(const std::string& str, const std::string& substr,
                          typename std::string::size_type pos) noexcept {
      if (pos + substr.size() > str.size())
        return false;

      for (decltype(pos) i{0}; i < substr.size(); ++i) {
        if (str[pos + i] != substr[i])
          return false;
      }

      return true;
    }

  } // End namespace utility
} // End namespace optionpp
