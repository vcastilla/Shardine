#ifndef OPERATION_H
#define OPERATION_H

#include <QByteArray>
#include <QUndoCommand>
#include <cstddef>
#include <libassert/assert.hpp>
#include <optional>
#include <unordered_map>
#include <utility>

#include "utils/utils.h"

class Operation final : public QUndoCommand {
public:
    using DataContainer = std::unordered_map<std::size_t, fs::Structure>;

    Operation(fs::Structure data, DataContainer& container, const QString& text) :
        m_data{std::move(data)}, m_container{container} {
        setText(text);
        if (const auto pair = m_container.find(m_data.address); pair != m_container.end() && pair->second == m_data)
            setObsolete(true);
    }

    Operation(const fs::Structure& data, DataContainer& container) :
        Operation{data, container,
                  QObject::tr("%1 bytes changed at 0x%2").arg(data.raw_data.size()).arg(data.address, 1, 16)} {}

    void undo() override {
        if (m_old_data) {
            const auto pair = ASSERT_VAL(m_container.find(m_data.address) != m_container.end());
            pair->second = m_old_data.value();
        } else {
            m_container.erase(m_data.address);
        }
        m_old_data.reset();
    }

    void redo() override {
        if (const auto pair = m_container.find(m_data.address); pair != m_container.end()) {
            m_old_data = pair->second;
            pair->second = m_data;
        } else {
            m_container.emplace(m_data.address, m_data);
        }
    }

    int id() const override {
        return utils::cast<int>(m_data.address);
    }

    [[nodiscard]] std::size_t address() const {
        return m_data.address;
    }

    [[nodiscard]] const fs::Structure& data() const {
        return m_data;
    }

    [[nodiscard]] const DataContainer& container() const {
        return m_container;
    }

private:
    fs::Structure m_data;
    std::optional<fs::Structure> m_old_data;
    DataContainer& m_container;
};


#endif // OPERATION_H
