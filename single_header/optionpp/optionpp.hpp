/* Option++ -- read command-line program options
 * Copyright (C) 2017-2020 Greg Kikola.'
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

// Single-header generated 2020-06-07T22:52:55Z


#include <cstddef>
#include <initializer_list>
#include <iosfwd>
#include <iterator>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace optionpp {
  class error : public std::logic_error {
  public:
    error(const std::string& msg, const std::string& fn_name)
      : logic_error(msg), m_function{fn_name} {}
    const std::string& function() const { return m_function; }
  private:
    std::string m_function;
  };
  class out_of_range : public error {
  public:
    out_of_range(const std::string& msg, const std::string& fn_name)
      : error(msg, fn_name) {}
  };
  class bad_dereference : public error {
  public:
    bad_dereference(const std::string& msg,
                    const std::string& fn_name)
      : error(msg, fn_name) {}
  };
  class type_error : public error {
  public:
    type_error(const std::string& msg, const std::string& fn_name)
      : error(msg, fn_name) {}
  };
}


namespace optionpp {
  namespace utility {
    template <typename OutputIt>
    void split(const std::string& str, OutputIt dest,
               const std::string& delims = " \t\n\r",
               const std::string& quotes = "\"\'",
               char escape_char = '\\',
               bool allow_empty = false);
    std::string wrap_text(const std::string& str,
                          int line_len = 79,
                          int indent = 0);
    std::string wrap_text(const std::string& str,
                          int line_len,
                          int indent,
                          int first_line_indent);
    bool is_substr_at_pos(const std::string& str, const std::string& substr,
                          std::string::size_type pos = 0) noexcept;
  }
}
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
      if (escape_next || str[pos] != quotes[quote_index]) {
        if (!escape_next && str[pos] == escape_char)
          escape_next = true;
        else {
          cur_token.push_back(str[pos]);
          escape_next = false;
        }
      } else {
        in_quotes = false;
      }
    } else {
      if (escape_next || delims.find(str[pos]) == std::string::npos) {
        if (!escape_next && str[pos] == escape_char)
          escape_next = true;
        else if (escape_next) {
          cur_token.push_back(str[pos]);
          escape_next = false;
        } else {
          quote_index = quotes.find(str[pos]);
          if (quote_index != std::string::npos)
            in_quotes = true;
          else
            cur_token.push_back(str[pos]);
        }
      } else {
        if (!cur_token.empty() || allow_empty)
          *dest++ = cur_token;
        cur_token.clear();
      }
    }
    ++pos;
  }
  if (!cur_token.empty() || allow_empty)
    *dest++ = cur_token;
}


namespace optionpp {
  class option {
  public:
    enum arg_type { string_arg,
                    int_arg,
                    uint_arg,
                    double_arg
    };
    option() noexcept {}
    option(char short_name) : m_short_name{short_name} {}
    option(const std::string& long_name,
           char short_name = '\0',
           const std::string& description = "",
           const std::string& arg_name = "",
           bool arg_required = false);
    option& name(const std::string& long_name, char short_name = '\0') {
      m_long_name = long_name;
      m_short_name = short_name;
      return *this;
    }
    std::string name() const noexcept {
      if (!m_long_name.empty())
        return m_long_name;
      else if (m_short_name != '\0')
        return std::string{m_short_name};
      else
        return "";
    }
    option& long_name(const std::string& name) {
      m_long_name = name;
      return *this;
    }
    const std::string& long_name() const noexcept { return m_long_name; }
    option& short_name(char name) noexcept {
      m_short_name = name;
      return *this;
    }
    char short_name() const noexcept { return m_short_name; }
    option& argument(const std::string& name,
                     bool required = true);
    const std::string& argument_name() const noexcept { return m_arg_name; }
    bool is_argument_required() const noexcept { return m_arg_required; }
    arg_type argument_type() const noexcept { return m_arg_type; }
    option& bind_bool(bool* var) noexcept;
    option& bind_string(std::string* var) noexcept;
    option& bind_int(int* var) noexcept;
    option& bind_uint(unsigned int* var) noexcept;
    option& bind_double(double* var) noexcept;
    bool has_bound_argument_variable() const noexcept { return m_bound_variable; }
    void write_bool(bool value) const noexcept;
    void write_string(const std::string& value) const;
    void write_int(int value) const;
    void write_uint(unsigned int value) const;
    void write_double(double value) const;
    option& description(const std::string& desc) {
      m_desc = desc;
      return *this;
    }
    const std::string& description() const noexcept { return m_desc; }
  private:
    std::string m_long_name;
    char m_short_name{'\0'};
    std::string m_desc;
    std::string m_arg_name;
    bool m_arg_required{false};
    arg_type m_arg_type{string_arg};
    bool* m_is_option_set = nullptr;
    void* m_bound_variable = nullptr;
  };
}


namespace optionpp {
  class option_group {
  public:
    using value_type = option;
    using container_type = std::vector<option>;
    using size_type = container_type::size_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using reverse_iterator = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;
    option_group() noexcept {}
    option_group(const std::string& name) : m_name{name} {}
    template <typename InputIt>
    option_group(const std::string& name,
                 InputIt first, InputIt last)
      : m_name{name}, m_options{first, last} {}
    const std::string& name() const noexcept { return m_name; }
    option& add_option(const option& opt = option{}) {
      m_options.push_back(opt);
      return m_options.back();
    }
    option& add_option(const std::string& long_name,
                       char short_name = '\0',
                       const std::string& description = "",
                       const std::string& arg_name = "",
                       bool arg_required = false);
    size_type size() const noexcept { return m_options.size(); }
    bool empty() const noexcept { return m_options.empty(); }
    iterator begin() noexcept { return m_options.begin(); }
    const_iterator begin() const noexcept { return cbegin(); }
    iterator end() noexcept { return m_options.end(); }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cbegin() const noexcept { return m_options.cbegin(); }
    const_iterator cend() const noexcept { return m_options.cend(); }
    reverse_iterator rbegin() noexcept { return m_options.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    reverse_iterator rend() noexcept { return m_options.rend(); }
    const_reverse_iterator rend() const noexcept { return crend(); }
    const_reverse_iterator crbegin() const noexcept { return m_options.crbegin(); }
    const_reverse_iterator crend() const noexcept { return m_options.crend(); }
    iterator find(const std::string& long_name);
    const_iterator find(const std::string& long_name) const;
    iterator find(char short_name);
    const_iterator find(char short_name) const;
    void sort();
    option& operator[](const std::string long_name);
    option& operator[](char short_name);
  private:
    std::string m_name;
    container_type m_options;
  };
}


namespace optionpp {
  struct parsed_entry {
    parsed_entry() noexcept {};
    explicit parsed_entry(const std::string& original_text,
                          bool is_option = false,
                          const std::string& long_name = "",
                          char short_name = '\0',
                          const std::string& argument = "")
      : original_text{original_text}, is_option{is_option},
        long_name{long_name}, short_name{short_name}, argument{argument} {}
    std::string original_text;
    std::string original_without_argument;
    bool is_option{false};
    std::string long_name;
    char short_name{'\0'};
    std::string argument;
    const option* opt_info{nullptr};
  };
  class parser_result {
  public:
    using value_type = parsed_entry;
    using container_type = std::vector<value_type>;
    using size_type = container_type::size_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using reverse_iterator = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;
    parser_result() noexcept {}
    parser_result(const std::initializer_list<value_type>& il)
      : m_entries{il} {}
    template <typename InputIt>
    parser_result(InputIt first, InputIt last) : m_entries{first, last} {}
    void push_back(const value_type& entry) { m_entries.push_back(entry); }
    void push_back(value_type&& entry) { m_entries.push_back(std::move(entry)); }
    void clear() noexcept { m_entries.clear(); }
    size_type size() const noexcept { return m_entries.size(); }
    bool empty() const noexcept { return m_entries.empty(); }
    iterator begin() noexcept { return m_entries.begin(); }
    const_iterator begin() const noexcept { return cbegin(); }
    iterator end() noexcept { return m_entries.end(); }
    const_iterator end() const noexcept { return cend(); }
    const_iterator cbegin() const noexcept { return m_entries.cbegin(); }
    const_iterator cend() const noexcept { return m_entries.cend(); }
    reverse_iterator rbegin() noexcept { return m_entries.rbegin(); }
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    reverse_iterator rend() noexcept { return m_entries.rend(); }
    const_reverse_iterator rend() const noexcept { return crend(); }
    const_reverse_iterator crbegin() const noexcept { return m_entries.crbegin(); }
    const_reverse_iterator crend() const noexcept { return m_entries.crend(); }
    value_type& at(size_type index) {
      if (index >= size())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return (*this)[index];
    }
    const value_type& at(size_type index) const {
      if (index >= size())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return (*this)[index];
    }
    value_type& operator[](size_type index) { return m_entries[index]; }
    const value_type& operator[](size_type index) const {
      return m_entries[index];
    }
    value_type& back() {
      if (empty())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::back");
      return m_entries.back();
    }
    const value_type& back() const {
      if (empty())
        throw out_of_range("out of bounds parser_result access",
                           "optionpp::parser_result::at");
      return m_entries.back();
    }
    bool is_option_set(const std::string& long_name) const noexcept;
    bool is_option_set(char short_name) const noexcept;
    std::string get_argument(std::string long_name) const noexcept;
    std::string get_argument(char short_name) const noexcept;
  private:
    container_type m_entries;
  };
}


namespace optionpp {
  template <typename T, typename Ptr, typename Ref, bool IsOption>
  class result_iterator : public std::iterator<std::bidirectional_iterator_tag,
                                               T, std::ptrdiff_t,
                                               Ptr, Ref> {
  public:
    result_iterator() noexcept : m_result{nullptr}, m_index{} {}
    result_iterator(T& result)
      : m_result{&result}, m_index{0} {
        if (!result.empty() && result[0].is_option != IsOption)
          ++(*this);
      }
    Ref operator*() const;
    Ptr operator->() const { return &**this; }
    result_iterator& operator++();
    result_iterator operator++(int) {
      result_iterator<T, Ptr, Ref, IsOption> copy{*this};
      ++(*this);
      return copy;
    }
    result_iterator& operator--();
    result_iterator operator--(int) {
      result_iterator<T, Ptr, Ref, IsOption> copy{*this};
      --(*this);
      return copy;
    }
    bool operator==(const result_iterator<T, Ptr, Ref, IsOption>& other) const;
    inline bool operator!=(const result_iterator<T, Ptr, Ref, IsOption>& other) const {
      return !(*this == other);
    }
  private:
    T* m_result;
    typename T::size_type m_index;
  };
  template <typename T,
            typename Ptr, typename Ref, bool IsOption>
  inline result_iterator<T, Ptr, Ref, IsOption>
  begin(result_iterator<T, Ptr, Ref, IsOption> it) noexcept { return it; }
  template <typename T,
            typename Ptr, typename Ref, bool IsOption>
  inline result_iterator<T, Ptr, Ref, IsOption>
  end(const result_iterator<T, Ptr, Ref, IsOption>&) noexcept {
    return result_iterator<T, Ptr, Ref, IsOption>{};
  }
  using option_iterator = result_iterator<parser_result,
                                          parsed_entry*,
                                          parsed_entry&,
                                          true>;
  using option_const_iterator = result_iterator<const parser_result,
                                                const parsed_entry*,
                                                const parsed_entry&,
                                                true>;
  using non_option_iterator = result_iterator<parser_result,
                                              parsed_entry*,
                                              parsed_entry&,
                                              false>;
  using non_option_const_iterator = result_iterator<const parser_result,
                                                    const parsed_entry*,
                                                    const parsed_entry&,
                                                    false>;
}
template <typename T,
          typename Ptr, typename Ref, bool IsOption>
Ref optionpp::result_iterator<T, Ptr, Ref, IsOption>::operator*() const {
  if (!m_result)
    throw bad_dereference{"tried to dereference a nullptr",
                          "optionpp::non_option_iterator::operator*"};
  if (m_index == m_result->size())
    throw bad_dereference{"tried to dereference past-the-end iterator",
        "optionpp::non_option_iterator::operator*"};
  return (*m_result)[m_index];
}
template <typename T,
          typename Ptr, typename Ref, bool IsOption>
auto optionpp::result_iterator<T, Ptr, Ref, IsOption>::operator++()
  -> result_iterator<T, Ptr, Ref, IsOption>& {
  if (m_result) {
    do {
      ++m_index;
    } while (m_index < m_result->size()
             && (*m_result)[m_index].is_option != IsOption);
  }
  return *this;
}
template <typename T,
          typename Ptr, typename Ref, bool IsOption>
auto optionpp::result_iterator<T, Ptr, Ref, IsOption>::operator--()
  -> result_iterator<T, Ptr, Ref, IsOption>& {
  if (m_result) {
    do {
      if (m_index == 0)
        throw out_of_range{"out of bounds parser_result access",
                           "optionpp::non_option_iterator::operator--"};
      --m_index;
    } while ((*m_result)[m_index].is_option != IsOption);
  }
  return *this;
}
template <typename T,
          typename Ptr, typename Ref, bool IsOption>
bool optionpp::result_iterator<T, Ptr, Ref, IsOption>
::operator==(const result_iterator<T, Ptr, Ref, IsOption>& other) const {
  bool left_default = !m_result;
  bool right_default = !other.m_result;
  bool left_at_end = left_default || m_result->size() == m_index;
  bool right_at_end = right_default
    || other.m_result->size() == other.m_index;
  if (!left_default && !right_default)
    return m_index == other.m_index && m_result == other.m_result;
  else if (left_at_end && right_at_end)
    return true;
  else
    return false;
}


namespace optionpp {
  class parse_error : public error {
  public:
    parse_error(const std::string msg, const std::string fn_name,
                const std::string option = "")
      : error(msg, fn_name), m_option{option} {}
    const std::string& option() const noexcept { return m_option; }
  private:
    std::string m_option;
  };
  class parser {
  public:
    parser() noexcept {}
    parser(const std::initializer_list<option>& il) {
      m_groups.emplace_back("", il.begin(), il.end());
    }
    template <typename InputIt>
    parser(InputIt first, InputIt last) { m_groups.emplace_back("", first, last); }
    option_group& group(const std::string& name);
    option& add_option(const option& opt = option{});
    option& add_option(const std::string& long_name,
                       char short_name = '\0',
                       const std::string& description = "",
                       const std::string& arg_name = "",
                       bool arg_required = false,
                       const std::string& group_name = "");
    template <typename InputIt>
    parser_result parse(InputIt first, InputIt last, bool ignore_first = true) const;
    parser_result parse(int argc, char* argv[], bool ignore_first = true) const;
    parser_result parse(const std::string& cmd_line, bool ignore_first = false) const;
    void set_custom_strings(const std::string& delims,
                            const std::string& short_prefix = "",
                            const std::string& long_prefix = "",
                            const std::string& end_indicator = "",
                            const std::string& equals = "");
    void sort_groups();
    void sort_options();
    option& operator[](const std::string& long_name);
    option& operator[](char short_name);
    std::ostream& print_help(std::ostream& os,
                             int max_line_length = 78,
                             int group_indent = 0,
                             int option_indent = 2,
                             int desc_first_line_indent = 30,
                             int desc_multiline_indent = 32) const;
  private:
    using group_container = std::vector<option_group>;
    using group_iterator = group_container::iterator;
    using group_const_iterator = group_container::const_iterator;
    using option_iterator = option_group::iterator;
    using option_const_iterator = option_group::const_iterator;
    group_iterator find_group(const std::string& name);
    group_const_iterator find_group(const std::string& name) const;
    option* find_option(const std::string& long_name);
    const option* find_option(const std::string& long_name) const;
    option* find_option(char short_name);
    const option* find_option(char short_name) const;
    bool is_end_indicator(const std::string& argument) const noexcept {
      return argument == m_end_of_options;
    }
    bool is_long_option(const std::string& argument) const noexcept {
      return argument.size() > m_long_option_prefix.size()
        && utility::is_substr_at_pos(argument, m_long_option_prefix);
    }
    bool is_short_option_group(const std::string& argument) const noexcept {
      return argument.size() > m_short_option_prefix.size()
        && utility::is_substr_at_pos(argument, m_short_option_prefix);
    }
    bool is_non_option(const std::string& argument) const noexcept {
      return !is_end_indicator(argument)
        && !is_long_option(argument)
        && !is_short_option_group(argument);
    }
    void write_option_argument(const parsed_entry& entry) const;
    enum class cl_arg_type { non_option,
                             end_indicator,
                             arg_required,
                             arg_optional,
                             no_arg
    };
    void parse_argument(const std::string& argument,
                        parser_result& result, cl_arg_type& type) const;
    void parse_short_option_group(const std::string& short_names,
                                  const std::string& argument, bool has_arg,
                                  parser_result& result, cl_arg_type& type) const;
    group_container m_groups;
    std::string m_delims{" \t\n\r"};
    std::string m_short_option_prefix{"-"};
    std::string m_long_option_prefix{"--"};
    std::string m_end_of_options{"--"};
    std::string m_equals{"="};
  };
  std::ostream& operator<<(std::ostream& os, const parser& parser);
}
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
    if (prev_type == cl_arg_type::arg_required
        || prev_type == cl_arg_type::arg_optional) {
      if (is_non_option(arg)
          || prev_type == cl_arg_type::arg_required) {
        auto& arg_info = result.back();
        arg_info.argument = arg;
        arg_info.original_text.push_back(' ');
        arg_info.original_text += arg;
        prev_type = cl_arg_type::non_option;
        if (arg_info.opt_info)
          write_option_argument(arg_info);
      } else {
        prev_type = cl_arg_type::non_option;
        continue;
      }
    } else if (prev_type == cl_arg_type::end_indicator) {
      parsed_entry arg_info;
      arg_info.original_text = arg;
      arg_info.is_option = false;
      result.push_back(std::move(arg_info));
    } else {
      parse_argument(arg, result, prev_type);
    }
    ++it;
  }
  if (prev_type == cl_arg_type::arg_required) {
    const auto& opt_name = result.back().original_text;
    throw parse_error{"option '" + opt_name + "' requires an argument",
        "optionpp::parser::parse", opt_name};
  }
  return result;
}



#ifdef OPTIONPP_MAIN


#include <algorithm>
#include <cctype>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <vector>

namespace optionpp {
  namespace utility {
    bool is_space(char c) {
      return std::isspace(static_cast<unsigned char>(c));
    }
    std::string wrap_line(const std::string& str,
                          int line_len,
                          int indent,
                          int first_line_indent) {
      if (line_len <= 0)
        return std::string(first_line_indent, ' ') + str;
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
        if (!result.empty()) {
          while (start < str.size() && is_space(str[start]))
            ++start;
        }
        auto end = start + line_len - cur_indent;
        if (end > str.size())
          end = str.size();
        if (end < str.size()) {
          auto word_start = end;
          while (word_start > start && !is_space(str[word_start]))
            --word_start;
          if (word_start > start)
            end = word_start;
        }
        pos = end;
        while (end > start && is_space(str[end - 1]))
          --end;
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
  }
}

namespace optionpp {
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
    if (var && m_arg_name.empty()) {
      m_arg_name = "STRING";
      m_arg_required = true;
    }
    m_arg_type = string_arg;
    m_bound_variable = var;
    return *this;
  }
  option& option::bind_int(int* var) noexcept {
    if (var && m_arg_name.empty()) {
      m_arg_name = "INTEGER";
      m_arg_required = true;
    }
    m_arg_type = int_arg;
    m_bound_variable = var;
    return *this;
  }
  option& option::bind_uint(unsigned int* var) noexcept {
    if (var && m_arg_name.empty()) {
      m_arg_name = "INTEGER";
      m_arg_required = true;
    }
    m_arg_type = uint_arg;
    m_bound_variable = var;
    return *this;
  }
  option& option::bind_double(double* var) noexcept {
    if (var && m_arg_name.empty()) {
      m_arg_name = "NUMBER";
      m_arg_required = true;
    }
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
}

namespace optionpp {
  option& option_group::add_option(const std::string& long_name,
                                   char short_name,
                                   const std::string& description,
                                   const std::string& arg_name,
                                   bool arg_required) {
    m_options.emplace_back(long_name, short_name, description,
                           arg_name, arg_required);
    return m_options.back();
  }
  option& option_group::operator[](const std::string long_name) {
    auto it = find(long_name);
    if (it == end())
      return add_option().long_name(long_name);
    else
      return *it;
  }
  option& option_group::operator[](char short_name) {
    auto it = find(short_name);
    if (it == end())
      return add_option().short_name(short_name);
    else
      return *it;
  }
  auto option_group::find(const std::string& long_name) -> iterator {
    return std::find_if(m_options.begin(), m_options.end(),
                        [&](const option& o) { return o.long_name() == long_name; });
  }
  auto option_group::find(const std::string& long_name) const -> const_iterator {
    return std::find_if(m_options.begin(), m_options.end(),
                        [&](const option& o) { return o.long_name() == long_name; });
  }
  auto option_group::find(char short_name) -> iterator {
    return std::find_if(m_options.begin(), m_options.end(),
                        [&](const option& o) { return o.short_name() == short_name; });
  }
  auto option_group::find(char short_name) const -> const_iterator {
    return std::find_if(m_options.begin(), m_options.end(),
                        [&](const option& o) { return o.short_name() == short_name; });
  }
  void option_group::sort() {
    std::sort(m_options.begin(), m_options.end(),
              [](const option& a, const option& b) {
                return a.name() < b.name();
              });
  }
}

namespace optionpp {
  bool parser_result::is_option_set(const std::string& long_name) const noexcept {
    if (long_name.empty())
      return false;
    else
      return std::any_of(begin(), end(),
                         [&](const parsed_entry& i) {
                           return i.is_option && i.long_name == long_name;
                         });
  }
  bool parser_result::is_option_set(char short_name) const noexcept {
    if (short_name == '\0')
      return false;
    else
      return std::any_of(begin(), end(),
                         [&](const parsed_entry& i) {
                           return i.is_option && i.short_name == short_name;
                         });
  }
  std::string parser_result::get_argument(std::string long_name) const noexcept {
    if (long_name == "")
      return "";
    auto it = std::find_if(rbegin(), rend(),
                           [&](const parsed_entry& i) {
                             return i.is_option && i.long_name == long_name;
                           });
    if (it != rend())
      return it->argument;
    else
      return "";
  }
  std::string parser_result::get_argument(char short_name) const noexcept {
    if (short_name == '\0')
      return "";
    auto it = std::find_if(rbegin(), rend(),
                           [=](const parsed_entry& i) {
                             return i.is_option && i.short_name == short_name;
                           });
    if (it != rend())
      return it->argument;
    else
      return "";
  }
}


namespace optionpp {
  option& parser::add_option(const option& opt) {
    auto it = find_group("");
    if (it == m_groups.end()) {
      m_groups.emplace_back("");
      return m_groups.back().add_option(opt);
    } else {
      return it->add_option(opt);
    }
  }
  option& parser::add_option(const std::string& long_name,
                             char short_name,
                             const std::string& description,
                             const std::string& arg_name,
                             bool arg_required,
                             const std::string& group_name) {
    return group(group_name).add_option(long_name, short_name)
      .description(description).argument(arg_name, arg_required);
  }
  option_group& parser::group(const std::string& name) {
    auto it = std::find_if(m_groups.rbegin(), m_groups.rend(),
                           [&](const option_group& g) {
                             return g.name() == name;
                           });
    if (it == m_groups.rend()) {
      m_groups.emplace_back(name);
      return m_groups.back();
    } else {
      return *it;
    }
  }
  void parser::set_custom_strings(const std::string& delims,
                                  const std::string& short_prefix,
                                  const std::string& long_prefix,
                                  const std::string& end_indicator,
                                  const std::string& equals) {
    if (!delims.empty())
      m_delims = delims;
    if (!short_prefix.empty())
      m_short_option_prefix = short_prefix;
    if (!long_prefix.empty())
      m_long_option_prefix = long_prefix;
    if (!end_indicator.empty())
      m_end_of_options = end_indicator;
    if (!equals.empty())
      m_equals = equals;
  }
  void parser::sort_groups() {
    std::sort(m_groups.begin(), m_groups.end(),
              [](const option_group& a, const option_group& b) {
                return a.name() < b.name();
              });
  }
  void parser::sort_options() {
    std::for_each(m_groups.begin(), m_groups.end(),
                  [](option_group& g) { g.sort(); });
  }
  option& parser::operator[](const std::string& long_name) {
    option* opt = find_option(long_name);
    if (opt)
      return *opt;
    else
      return add_option().long_name(long_name);
  }
  option& parser::operator[](char short_name) {
    option* opt = find_option(short_name);
    if (opt)
      return *opt;
    else
      return add_option().short_name(short_name);
  }
  std::ostream& parser::print_help(std::ostream& os,
                                   int max_line_length,
                                   int group_indent,
                                   int option_indent,
                                   int desc_first_line_indent,
                                   int desc_multiline_indent) const {
    bool first = true;
    for (const auto& group : m_groups) {
      if (group.empty())
        continue;
      if (first)
        first = false;
      else
        os << "\n\n";
      if (!group.name().empty()) {
        os << utility::wrap_text(group.name(), max_line_length, group_indent)
           << "\n";
      }
      bool first_opt = true;
      for (const auto& opt : group) {
        if (first_opt)
          first_opt = false;
        else
          os << "\n";
        std::string usage(option_indent, ' ');
        if (opt.short_name() != '\0') {
          usage += m_short_option_prefix;
          usage += opt.short_name();
          if (!opt.long_name().empty())
            usage += ", ";
        } else {
          usage += std::string(m_short_option_prefix.size() + 3, ' ');
        }
        if (!opt.long_name().empty()) {
          usage += m_long_option_prefix;
          usage += opt.long_name();
        }
        if (!opt.argument_name().empty()) {
          if (opt.is_argument_required())
            usage += m_equals + opt.argument_name();
          else
            usage += "[" + m_equals + opt.argument_name() + "]";
        }
        int spacing = desc_first_line_indent - usage.size();
        if (spacing <= 1) {
          os << utility::wrap_text(usage, max_line_length);
          if (!opt.description().empty()) {
            os << "\n" << utility::wrap_text(opt.description(),
                                             max_line_length,
                                             desc_multiline_indent,
                                             desc_first_line_indent);
          }
        } else {
          if (!opt.description().empty()) {
            usage += std::string(spacing, ' ');
            usage += opt.description();
          }
          os << utility::wrap_text(usage, max_line_length,
                                   desc_multiline_indent, 0);
        }
      }
    }
    return os;
  }
  auto parser::find_group(const std::string& name) -> group_iterator {
    return std::find_if(m_groups.begin(), m_groups.end(),
                        [&](const option_group& g) {
                          return g.name() == name;
                        });
  }
  auto parser::find_group(const std::string& name) const -> group_const_iterator {
    return std::find_if(m_groups.begin(), m_groups.end(),
                        [&](const option_group& g) {
                          return g.name() == name;
                        });
  }
  option* parser::find_option(const std::string& long_name) {
    for (auto& group : m_groups) {
      auto it = group.find(long_name);
      if (it != group.end())
        return &(*it);
    }
    return nullptr;
  }
  const option* parser::find_option(const std::string& long_name) const {
    for (const auto& group : m_groups) {
      auto it = group.find(long_name);
      if (it != group.end())
        return &(*it);
    }
    return nullptr;
  }
  option* parser::find_option(char short_name) {
    for (auto& group : m_groups) {
      auto it = group.find(short_name);
      if (it != group.end())
        return &(*it);
    }
    return nullptr;
  }
  const option* parser::find_option(char short_name) const {
    for (const auto& group : m_groups) {
      auto it = group.find(short_name);
      if (it != group.end())
        return &(*it);
    }
    return nullptr;
  }
  parser_result parser::parse(int argc, char* argv[], bool ignore_first) const {
    return parse(argv, argv + argc, ignore_first);
  }
  parser_result parser::parse(const std::string& cmd_line, bool ignore_first) const {
    std::vector<std::string> container;
    utility::split(cmd_line, std::back_inserter(container),
                   m_delims, "\"'", '\\');
    return parse(container.begin(), container.end(), ignore_first);
  }
  void parser::write_option_argument(const parsed_entry& entry) const {
    if (!entry.opt_info)
      return;
    const option& opt = *entry.opt_info;
    if (!opt.has_bound_argument_variable())
      return;
    std::string::size_type pos = 0;
    const std::string& arg = entry.argument;
    const std::string& opt_name = entry.original_without_argument;
    const std::string& fn_name = "optionpp::parser::write_option_argument";
    try {
      switch (opt.argument_type()) {
      case option::uint_arg: {
        long long value = std::stoll(entry.argument, &pos);
        if (pos != arg.size())
          throw std::invalid_argument{"invalid argument"};
        if (value < 0)
          throw parse_error{"argument for option '" + opt_name + "' must not be negative",
              fn_name, opt_name};
        else if (value > std::numeric_limits<unsigned>::max())
          throw std::out_of_range{"out of range"};
        opt.write_uint(static_cast<unsigned>(value));
        break;
      }
      case option::int_arg: {
        int value = std::stoi(entry.argument, &pos);
        if (pos != arg.size())
          throw std::invalid_argument{"invalid argument"};
        opt.write_int(value);
        break;
      }
      case option::double_arg: {
        double value = std::stod(entry.argument, &pos);
        if (pos != arg.size())
          throw std::invalid_argument{"invalid argument"};
        opt.write_double(value);
        break;
      }
      default:
      case option::string_arg:
        opt.write_string(arg);
        break;
      }
    } catch(const std::invalid_argument&) {
      switch (opt.argument_type()) {
      case option::uint_arg:
      case option::int_arg:
        throw parse_error{"argument for option '" + opt_name + "' must be an integer",
            fn_name, opt_name};
      case option::double_arg:
        throw parse_error{"argument for option '" + opt_name + "' must be a number",
            fn_name, opt_name};
      default:
        throw type_error{"type error in argument for option '" + opt_name + "'", fn_name};
      }
    } catch(const std::out_of_range&) {
      throw parse_error{"argument for option '" + opt_name + "' is out of range",
          fn_name, opt_name};
    }
  }
  void parser::parse_argument(const std::string& argument,
                              parser_result& result, cl_arg_type& type) const {
    if (is_end_indicator(argument)) {
      type = cl_arg_type::end_indicator;
      return;
    }
    std::string option_specifier;
    std::string option_argument;
    bool assignment_found = false;
    auto pos = argument.find(m_equals);
    if (pos == std::string::npos)
      option_specifier = argument;
    else {
      assignment_found = true;
      option_specifier = argument.substr(0, pos);
      pos += m_equals.size();
      option_argument = argument.substr(pos);
      if (option_specifier == m_short_option_prefix
          || option_specifier == m_long_option_prefix) {
        option_specifier += m_equals;
        throw parse_error{"invalid option: '" + option_specifier + "'",
            "optionpp::parser::parse_argument", option_specifier};
      }
    }
    parsed_entry arg_info;
    if (is_long_option(option_specifier)) {
      std::string option_name = option_specifier.substr(m_long_option_prefix.size());
      const option* opt = find_option(option_name);
      if (!opt)
        throw parse_error{"invalid option: '" + option_specifier + "'",
            "optionpp::parser::parse_argument", option_specifier};
      arg_info.opt_info = &(*opt);
      if (!opt->argument_name().empty()) {
        if (!assignment_found) {
          if (opt->is_argument_required())
            type = cl_arg_type::arg_required;
          else
            type = cl_arg_type::arg_optional;
        } else {
          type = cl_arg_type::no_arg;
          arg_info.argument = option_argument;
        }
      } else {
        if (assignment_found)
          throw parse_error{"option '" + option_specifier + "' does not accept arguments",
              "optionpp::parser::parse_argument", option_specifier};
        type = cl_arg_type::no_arg;
      }
      arg_info.original_text = argument;
      arg_info.original_without_argument = option_specifier;
      arg_info.is_option = true;
      arg_info.long_name = option_name;
      arg_info.short_name = opt->short_name();
      if (assignment_found)
        write_option_argument(arg_info);
      opt->write_bool(true);
      result.push_back(std::move(arg_info));
    } else if (is_short_option_group(option_specifier)) {
      parse_short_option_group(option_specifier.substr(m_short_option_prefix.size()),
                               option_argument, assignment_found,
                               result, type);
    } else {
      type = cl_arg_type::non_option;
      arg_info.original_text = argument;
      arg_info.is_option = false;
      result.push_back(std::move(arg_info));
    }
  }
  void parser::parse_short_option_group(const std::string& short_names,
                                        const std::string& argument, bool has_arg,
                                        parser_result& result, cl_arg_type& type) const {
    using sz_t = std::string::size_type;
    for (sz_t pos = 0; pos != short_names.size(); ++pos) {
      const option* opt = find_option(short_names[pos]);
      if (!opt) {
        auto opt_name = m_short_option_prefix;
        opt_name.push_back(short_names[pos]);
        throw parse_error{"invalid option: '" + opt_name + "'",
            "optionpp::parser::parse_short_option_group", opt_name};
      }
      parsed_entry arg_info;
      arg_info.original_text = m_short_option_prefix;
      arg_info.original_text.push_back(short_names[pos]);
      arg_info.original_without_argument = arg_info.original_text;
      arg_info.is_option = true;
      arg_info.long_name = opt->long_name();
      arg_info.short_name = short_names[pos];
      arg_info.opt_info = &(*opt);
      opt->write_bool(true);
      if (!opt->argument_name().empty()) {
        if (pos + 1 < short_names.size()) {
          arg_info.argument = short_names.substr(pos + 1);
          if (has_arg) {
            arg_info.argument += m_equals;
            arg_info.argument += argument;
          }
          arg_info.original_text += arg_info.argument;
          write_option_argument(arg_info);
          result.push_back(std::move(arg_info));
          type = cl_arg_type::no_arg;
          break;
        } else {
          if (has_arg) {
            arg_info.original_text += m_equals;
            arg_info.original_text += argument;
            arg_info.argument = argument;
            write_option_argument(arg_info);
            type = cl_arg_type::no_arg;
          } else if (opt->is_argument_required()) {
            type = cl_arg_type::arg_required;
          } else {
            type = cl_arg_type::arg_optional;
          }
          result.push_back(std::move(arg_info));
          break;
        }
      }
      if (pos + 1 == short_names.size() && has_arg) {
        auto opt_name = m_short_option_prefix;
        opt_name.push_back(short_names[pos]);
        throw parse_error{"option '" + opt_name + "' does not accept arguments",
            "optionpp::parser::parse_short_option_group", opt_name};
      }
      result.push_back(std::move(arg_info));
      type = cl_arg_type::no_arg;
      arg_info = parsed_entry{};
    }
  }
  std::ostream& operator<<(std::ostream& os, const parser& opt_parser) {
    return opt_parser.print_help(os);
  }
}


#endif
