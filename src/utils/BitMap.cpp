#include "utils/BitMap.h"

#include <libassert/assert.hpp>

BitMap::BitMap(const size_type size) : m_data(size) {}

BitMap::BitMap(const size_type size, std::istream &is) : m_data(size) {
    is.read(reinterpret_cast<char *>(m_data.data()), size);
}

bool BitMap::operator[](const index_type index) const {
    ASSERT(index / 8 < m_data.size());
    return m_data[index / 8] & (1 << (index % 8));
}

std::uint8_t &BitMap::byte_at(const size_type offset) {
    ASSERT(offset < m_data.size());
    return m_data[offset];
}
