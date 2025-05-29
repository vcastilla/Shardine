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
