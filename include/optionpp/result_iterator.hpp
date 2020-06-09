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
 * @brief Header file for `parser_result` class implementation.
 */

#ifndef OPTIONPP_RESULT_ITERATOR_HPP
#define OPTIONPP_RESULT_ITERATOR_HPP

#include <optionpp/parser_result.hpp>

namespace optionpp {

  /**
   * @brief Iterator over parser_result arguments.
   */
  template <typename T, typename Ptr, typename Ref, bool IsOption>
  class result_iterator : public std::iterator<std::bidirectional_iterator_tag,
                                               T, std::ptrdiff_t,
                                               Ptr, Ref> {
  public:
    /**
     * @brief Default constructor.
     */
    result_iterator() noexcept : m_result{nullptr}, m_index{} {}
    /**
     * @brief Constructor.
     * @param result Parser result to iterate over.
     */
    result_iterator(T& result)
      : m_result{&result}, m_index{0} {
        if (!result.empty() && result[0].is_option != IsOption)
          ++(*this);
      }

    /**
     * @brief Dereference operator.
     * @throw bad_dereference If called on a default-constructed
     *                        instance.
     * @return Reference to the pointed-to entry.
     */
    Ref operator*() const;

    /**
     * @brief Member access operator.
     * @throw bad_dereference If called on a default-constructed
     *                        instance or on `end()`.
     * @return Reference to the pointed-to entry.
     */
    Ptr operator->() const { return &**this; }

    /**
     * @brief Prefix increment operator.
     * @return The incremented iterator pointing to the next non-option
     *         argument.
     */
    result_iterator& operator++();

    /**
     * @brief Postfix increment operator.
     * @return A copy of the unincremented iterator.
     */
    result_iterator operator++(int) {
      result_iterator<T, Ptr, Ref, IsOption> copy{*this};
      ++(*this);
      return copy;
    }

    /**
     * @brief Prefix decrement operator.
     * @throw out_of_range If the iterator already points to the
     *                     beginning.
     * @return The decremented iterator pointing to the previous non-option
     *         argument.
     */
    result_iterator& operator--();

    /**
     * @brief Postfix decrement operator.
     * @throw out_of_range If the iterator already points to the
     *                     beginning.
     * @return A copy of the undecremented iterator.
     */
    result_iterator operator--(int) {
      result_iterator<T, Ptr, Ref, IsOption> copy{*this};
      --(*this);
      return copy;
    }

    /**
     * @brief `result_iterator` comparison operator.
     * @param other Right operand to compare.
     * @return True if both iterators point to the same member of the
     *         `parser_result`.
     */
    bool operator==(const result_iterator<T, Ptr, Ref, IsOption>& other) const;

    /**
     * @brief `result_iterator` comparison operator.
     * @param other Right operand to compare.
     * @return False if both iterators point to the same member of the
     *         same `parser_result`.
     */
    inline bool operator!=(const result_iterator<T, Ptr, Ref, IsOption>& other) const {
      return !(*this == other);
    }

  private:
    T* m_result; //< `parser_result` being iterated over.
    typename T::size_type m_index; //< Current entry index.
  };

  /**
   * @brief `begin` function for range for loop support.
   * @param it A `non_option_iterator`.
   * @return The unchanged iterator `it`
   */
  template <typename T,
            typename Ptr, typename Ref, bool IsOption>
  inline result_iterator<T, Ptr, Ref, IsOption>
  begin(result_iterator<T, Ptr, Ref, IsOption> it) noexcept { return it; }

  /**
   * @brief `end` function for range for loop support.
   * @return The end iterator.
   */
  template <typename T,
            typename Ptr, typename Ref, bool IsOption>
  inline result_iterator<T, Ptr, Ref, IsOption>
  end(const result_iterator<T, Ptr, Ref, IsOption>&) noexcept {
    return result_iterator<T, Ptr, Ref, IsOption>{};
  }

  /**
   * @brief Iterator over option entries in a `parser_result`.
   */
  using option_iterator = result_iterator<parser_result,
                                          parsed_entry*,
                                          parsed_entry&,
                                          true>;

  /**
   * @brief `const_iterator` over non-option entries in a
   *        `parser_result`.
   */
  using option_const_iterator = result_iterator<const parser_result,
                                                const parsed_entry*,
                                                const parsed_entry&,
                                                true>;

  /**
   * @brief Iterator over non-option entries in a `parser_result`.
   */
  using non_option_iterator = result_iterator<parser_result,
                                              parsed_entry*,
                                              parsed_entry&,
                                              false>;

  /**
   * @brief `const_iterator` over non-option entries in a
   *        `parser_result`.
   */
  using non_option_const_iterator = result_iterator<const parser_result,
                                                    const parsed_entry*,
                                                    const parsed_entry&,
                                                    false>;

} // End namespace


/* Implementation */

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

  // If neither iterator was default-constructed, compare contents
  if (!left_default && !right_default)
    return m_index == other.m_index && m_result == other.m_result;
  // If both are `end()` iterators, they're equal
  else if (left_at_end && right_at_end)
    return true;
  else // Otherwise, not equal
    return false;
}

#endif
