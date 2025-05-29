#ifndef OPERATIONSTACK_H
#define OPERATIONSTACK_H

#include <QUndoView>
#include <vector>

#include "filesystem/FileSystem.h"
#include "filesystem/Segment.h"
#include "qt-utils/Operation.h"

class OperationStack final : public QObject {
    Q_OBJECT

public:
    void connectUndoView(QUndoView& undo_view);

    fs::Structure getStructure(const fs::Segment& segment, unsigned index);

    void updateChanges(const fs::FileSystem& original, const fs::FileSystem& updated);

    void addOperation(const fs::FileSystem& fs, const fs::Structure& structure);

    bool saveHistory(const std::filesystem::path& file_name);

    void clear() {
        m_hex_data.clear();
        m_undo_stack.clear();
    }

    bool isClean() const {
        return m_undo_stack.isClean();
    }

    void setClean() {
        m_undo_stack.setClean();
    }

    bool areNewChangesUnsaved() const {
        return m_undo_stack.cleanIndex() != m_undo_stack.count();
    }

    void discardCurrentChanges() {
        m_undo_stack.setIndex(m_undo_stack.cleanIndex());
    }

    const Operation::DataContainer& dataContainer() {
        return m_hex_data;
    }

signals:
    void cleanChanged(bool is_dirty);

private:
    static QString getOpText(const std::vector<fs::Segment>& segments, unsigned address);

    Operation::DataContainer m_hex_data;
    QUndoStack m_undo_stack;
};

#endif // OPERATIONSTACK_H
