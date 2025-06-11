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

#ifndef STRUCTUREMODEL_H
#define STRUCTUREMODEL_H

#include <QAbstractTableModel>
#include <QBrush>
#include <array>
#include <cstddef>
#include <hexedit/hexedit.hpp>
#include <utility>
#include <vector>

#include "filesystem/Structure.h"

class StructureModel final : public QAbstractTableModel {
    Q_OBJECT
public:
    StructureModel() = default;

    explicit StructureModel(fs::Structure structure, QObject* parent = nullptr);

    [[nodiscard]] const fs::Structure& getStructure() const;
    void setStructure(const fs::Structure& structure);
    void setByteData(const QByteArray& data);

    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    void setModified(int index, bool value);
    [[nodiscard]] bool isModified(int index) const;
    std::size_t byteOffset(const QModelIndex& index) const;
    std::size_t byteSize(const QModelIndex& index) const;

    void change_selection(HexEdit& hex_edit, const QItemSelection& selected) const;
    void highlight_changes(const fs::ByteArray& edit_data);

private:
    fs::Structure m_structure;
    std::array<QString, 3> m_header{tr("Property"), tr("Value"), tr("Field size (bytes)")};
    std::vector<bool> m_fields_modified;
    QBrush m_highlight_brush{QColor(0xff, 0xff, 0x99, 0xff)};
};

#endif // STRUCTUREMODEL_H
