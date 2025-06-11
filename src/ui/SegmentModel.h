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

#ifndef SEGMENTMODEL_H
#define SEGMENTMODEL_H

#include <QAbstractListModel>
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "contr/SegmentInfo.h"
#include "utils/utils.h"

class SegmentModel final : public QAbstractListModel {
    Q_OBJECT

    using Segments = std::vector<SegmentInfo>;

public:
    SegmentModel() = default;

    explicit SegmentModel(Segments segments) : m_segments{std::move(segments)} {}

    QVariant data(const QModelIndex& index, const int role) const override {
        if (!index.isValid())
            return {};
        const auto segment = m_segments.at(index.row());
        switch (role) {
            case Qt::DisplayRole:
                return tr("%1\n%2 KiB").arg(segment.name).arg(segment.size / 1024);
            case Qt::SizeHintRole:
                return QSize{static_cast<int>(100 * (std::log10(segment.size) - 2)), 100};
            default:
                break;
        }
        return {};
    }

    int rowCount(const QModelIndex&) const override {
        return utils::cast<int>(m_segments.size());
    }

    const Segments& getSegments() const {
        return m_segments;
    }

    void setSegments(const Segments& segments) {
        m_segments = segments;
        emit layoutChanged({});
    }

private:
    Segments m_segments;
};


#endif // SEGMENTMODEL_H
