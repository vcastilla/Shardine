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

#include "OperationStack.h"

#include <fstream>

void OperationStack::connectUndoView(QUndoView& undo_view) {
    undo_view.setStack(&m_undo_stack);
    connect(undo_view.selectionModel(), &QItemSelectionModel::selectionChanged, this,
            [this](const QItemSelection&, const QItemSelection&) { emit cleanChanged(!m_undo_stack.isClean()); });
}

fs::Structure OperationStack::getStructure(const fs::Segment& segment, const unsigned index) {
    const auto offset = segment.address + segment.element_size * (index - segment.min_element_index);
    if (const auto pair = m_hex_data.find(offset); pair != m_hex_data.end())
        return pair->second;

    const auto structure = segment.read(index);
    m_hex_data.emplace(structure.address, structure);
    return structure;
}

void OperationStack::updateChanges(const fs::FileSystem& original, const fs::FileSystem& updated) {
    discardCurrentChanges();

    const auto vec = updated.segments();

    const auto changes = fs::diff(original, updated);
    // m_undo_stack.beginMacro(tr("External modifications"));
    const auto msg = tr("(External %1) %2").arg(QString::fromStdString(utils::now_to_str("%T")));
    for (const auto& [old, change]: changes) {
        // Add old change to the container to reflect the previous state
        m_hex_data.emplace(old.address, old);
        // Add change to the history
        m_undo_stack.push(new Operation{change, m_hex_data, msg.arg(getOpText(vec, change.address))});
    }
    // m_undo_stack.endMacro();
    m_undo_stack.setClean();
}

void OperationStack::addOperation(const fs::FileSystem& fs, const fs::Structure& structure) {
    const auto txt = getOpText(fs.segments(), structure.address);
    m_undo_stack.push(new Operation{structure, m_hex_data, txt});
}

bool OperationStack::saveHistory(const std::filesystem::path& file_name) {
    std::ofstream file{file_name};
    const auto end = m_undo_stack.cleanIndex();
    for (int i = 0; i < end; i++)
        file << m_undo_stack.text(i).toStdString() << '\n';
    return file.good();
}

QString OperationStack::getOpText(const std::vector<fs::Segment>& segments, const unsigned address) {
    const auto [segment, idx] = fs::addr_to_segment(segments, address);
    const auto elem_name = segment.element_name.toLower();
    if (segment.is_single_element())
        return tr("Modified %1").arg(elem_name);
    return tr("Modified %1 number %2").arg(elem_name).arg(idx);
}
