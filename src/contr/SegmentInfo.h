#ifndef SEGMENTINFO_H
#define SEGMENTINFO_H

#include <QString>
#include <cstddef>

struct SegmentInfo {
    QString name;
    QString alt_text;
    QString elem_name;

    int min_elem_idx;
    int max_elem_idx;
    std::size_t size;

    [[nodiscard]] constexpr bool is_single_element() const noexcept {
        return min_elem_idx == max_elem_idx;
    }
};

#endif // SEGMENTINFO_H
