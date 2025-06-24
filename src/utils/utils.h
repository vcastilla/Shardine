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

#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <concepts>
#include <filesystem>
#include <fstream>
#include <istream>
#include <libassert/assert.hpp>
#include <limits>
#include <string>
#include <utility>

namespace utils {

/* Stream initialization */

template<typename T>
concept StreamInitializable = requires(std::istream& is, T val) { is >> val; };

template<StreamInitializable T>
[[nodiscard]] constexpr T stream_init(std::istream& is) {
    T val;
    is >> val;
    return val;
}

template<StreamInitializable T>
[[nodiscard]] constexpr T stream_init(std::istream& is, const std::istream::off_type offset) {
    is.seekg(offset, std::ios::beg);
    return stream_init<T>(is);
}

/* Casting */

template<std::unsigned_integral Unsigned>
[[nodiscard]] constexpr auto to_signed(const Unsigned val) noexcept {
    using Signed = std::make_signed_t<Unsigned>;
    ASSERT(val <= std::numeric_limits<Signed>::max());
    return static_cast<Signed>(val);
}

template<std::signed_integral Signed>
[[nodiscard]] constexpr auto to_unsigned(const Signed val) noexcept {
    ASSERT(val >= 0);
    return static_cast<std::make_unsigned_t<Signed>>(val);
}

template<std::integral To, std::integral From>
[[nodiscard]] constexpr To cast(const From val) noexcept {
    ASSERT(std::cmp_greater_equal(val, std::numeric_limits<To>::min()));
    ASSERT(val <= std::numeric_limits<To>::max());
    return static_cast<To>(val);
}

/* Time */

[[nodiscard]] inline std::string timestamp_to_fmt_str(const std::time_t timestamp, const char* fmt) {
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timestamp), fmt);
    return ss.str();
}

[[nodiscard]] inline std::string timestamp_to_str(const std::time_t timestamp) {
    // See https://cppreference.com/w/cpp/io/manip/put_time for formatting details
    return timestamp_to_fmt_str(timestamp, "%F %T");
}

[[nodiscard]] inline std::string now_to_str(const char* fmt = "%FT%T%Z") {
    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::system_clock::to_time_t(now);
    return timestamp_to_fmt_str(timestamp, fmt);
}

/* File utilities */

inline bool create_file(const std::filesystem::path& path, const std::streamoff size) {
    std::ofstream ofs{path, std::ios::binary};
    ofs.seekp(size - 1, std::ios::beg);
    ofs << '\0';
    return !ofs.fail();
}

inline bool file_equal(const std::filesystem::path& p1, const std::filesystem::path& p2) {
    std::ifstream f1(p1, std::ifstream::binary);
    std::ifstream f2(p2, std::ifstream::binary);

    if (!f1 || !f2)
        return false;

    return std::equal(std::istreambuf_iterator(f1.rdbuf()), std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator(f2.rdbuf()));
}

} // namespace utils

#endif // UTILS_H
