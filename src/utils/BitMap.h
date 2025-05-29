#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>
#include <iostream>
#include <vector>

class BitMap {
public:
    using size_type = unsigned int;
    using index_type = unsigned long;

    BitMap() = default;

    explicit BitMap(size_type size);

    BitMap(size_type size, std::istream &is);

    [[nodiscard]] constexpr const std::vector<std::uint8_t> &data() const noexcept {
        return m_data;
    }

    [[nodiscard]] size_type size() const noexcept {
        return m_data.size();
    }

    [[nodiscard]] bool operator[](index_type index) const;

    std::uint8_t &byte_at(size_type offset);

private:
    std::vector<std::uint8_t> m_data;
};


#endif // BITMAP_H
