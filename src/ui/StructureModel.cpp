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

#include "StructureModel.h"

#include <algorithm>
#include <libassert/assert.hpp>

#include "utils/utils.h"

StructureModel::StructureModel(fs::Structure structure, QObject* parent) :
    QAbstractTableModel(parent), m_structure{std::move(structure)}, m_fields_modified(m_structure.fields.size()) {}

const fs::Structure& StructureModel::getStructure() const {
    return m_structure;
}

void StructureModel::setStructure(const fs::Structure& structure) {
    m_structure = structure;
    m_fields_modified.clear();
    m_fields_modified.resize(m_structure.fields.size());
    emit layoutChanged();
}

void StructureModel::setByteData(const QByteArray& data) {
    ASSERT(m_structure.raw_data.size() == data.size());
    m_structure.raw_data = data;
    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

int StructureModel::rowCount(const QModelIndex& /*parent*/) const {
    return utils::cast<int>(m_structure.fields.size());
}

int StructureModel::columnCount(const QModelIndex& /*parent*/) const {
    return utils::cast<int>(m_header.size());
}

QVariant StructureModel::data(const QModelIndex& idx, const int role) const {
    const int row = idx.row();
    const int col = idx.column();
    switch (role) {
        case Qt::DisplayRole:
            switch (col) {
                case 0:
                    return m_structure.name_at(row);
                case 1:
                    return QString::fromStdString(m_structure.value_at(row));
                case 2:
                    return QString::number(m_structure.field_size_at(row));
                default:
                    break;
            }
            break;
        case Qt::BackgroundRole:
            if (isModified(row))
                return m_highlight_brush;
            break;
        default:
            break;
    }

    return {};
}

QVariant StructureModel::headerData(const int section, const Qt::Orientation orientation, const int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return m_header[section];
    }
    return {};
}

void StructureModel::setModified(const int idx, const bool value) {
    ASSERT(idx >= 0);
    ASSERT(idx < m_fields_modified.size());
    m_fields_modified[idx] = value;
    emit dataChanged(index(idx, 0), index(idx, columnCount() - 1));
}

bool StructureModel::isModified(const int idx) const {
    ASSERT(idx >= 0);
    ASSERT(idx < m_fields_modified.size());
    return m_fields_modified[idx];
}

std::size_t StructureModel::byteOffset(const QModelIndex& idx) const {
    return m_structure.idx_to_size(idx.row());
}

std::size_t StructureModel::byteSize(const QModelIndex& idx) const {
    return m_structure.field_size_at(idx.row());
}

void StructureModel::change_selection(HexEdit& hex_edit, const QItemSelection& selected) const {
    if (selected.empty())
        return;
    const auto idx = selected.indexes().first();
    const auto offset = utils::cast<qint64>(byteOffset(idx));
    const auto size = utils::cast<qint64>(byteSize(idx));
    hex_edit.setSelection(offset, offset + size);
}

void StructureModel::highlight_changes(const fs::ByteArray& edit_data) {
    const auto structure = getStructure();
    ASSERT(structure.raw_data.size() == edit_data.size());
    unsigned it = 0;
    int row = 0;
    for (const auto& field: structure.fields) {
        for (unsigned i = 0; i < field.byte_size(); i++) {
            if (structure.raw_data[it + i] != edit_data[it + i]) {
                setModified(row, true);
                break;
            }
        }
        it += field.byte_size();
        row++;
    }
    setByteData(edit_data);
}
