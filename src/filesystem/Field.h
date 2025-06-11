/*
 * Copyright 2025 Víctor Castilla Rodríguez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef FIELD_H
#define FIELD_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <functional>
#include <sstream>
#include <string>
#include <utility>

namespace fs {

/* Concept for a function that takes a Value and returns a string. */
template<typename Function, typename Value>
concept StrFunction = requires(Function fn, Value val) {
    { fn(val) } -> std::convertible_to<std::string>;
};

class Field {
    using Byte = std::byte;

public:
    template<typename Value>
    constexpr Field(std::string name, Value, StrFunction<Value> auto&& to_str) :
        m_name{std::move(name)}, m_byte_size{sizeof(Value)}, m_to_str{[to_str](const Byte arr[]) {
            Value v;
            std::memcpy(&v, arr, sizeof(Value));
            return to_str(v);
        }} {}

    template<typename Value>
    constexpr Field(const std::string& name, Value value) :
        Field(name, value, [](const Value& val) {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }) {}

    template<typename Value, std::size_t N>
    constexpr Field(std::string name, const Value (&)[N], StrFunction<Value> auto&& to_str) :
        m_name{std::move(name)}, m_byte_size{sizeof(Value[N])}, m_to_str{[to_str](const Byte arr[]) {
            Value v;
            std::memcpy(&v, arr, sizeof(Value));
            return to_str(v);
        }},
        m_array_len{N} {}

    template<typename Value, std::size_t N>
    constexpr Field(const std::string& name, const Value (&value)[N]) :
        Field(name, value, [](const Value& val) {
            std::stringstream ss;
            ss << val;
            return ss.str();
        }) {}

    // Specialization for char array
    template<std::size_t N>
    constexpr Field(const std::string& name, const char (&)[N]) :
        m_name{std::move(name)}, m_byte_size{sizeof(char[N])}, m_to_str{[](const Byte arr[]) {
            std::stringstream ss;
            ss << reinterpret_cast<const char*>(arr);
            return ss.str();
        }} {}

    [[nodiscard]] const std::string& name() const noexcept {
        return m_name;
    }

    [[nodiscard]] std::string value(const auto arr[]) const {
        return m_to_str(reinterpret_cast<const Byte*>(arr));
    }

    [[nodiscard]] constexpr std::size_t byte_size() const noexcept {
        return m_byte_size;
    }

    [[nodiscard]] constexpr bool is_array() const noexcept {
        return m_array_len > 1;
    }

    [[nodiscard]] constexpr std::size_t array_len() const noexcept {
        return m_array_len;
    }

    [[nodiscard]] constexpr std::size_t array_elem_size() const noexcept {
        return m_byte_size / m_array_len;
    }

private:
    std::string m_name;
    std::size_t m_byte_size;
    std::function<std::string(const Byte[])> m_to_str;
    std::size_t m_array_len{1};
};

} // namespace fs

#endif // FIELD_H
