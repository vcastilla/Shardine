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

#include <QTest>
#include <string>
#include "filesystem/Field.h"

using namespace std::string_literals;

const char* bytes(const auto& val) {
    return reinterpret_cast<const char*>(&val);
}

class FieldTest final : public QObject {
    Q_OBJECT

private slots:

    void name() {
        const auto field = fs::Field{"Field", int{}};
        QCOMPARE("Field", field.name());
    }

    void value_data() {
        QTest::addColumn<int>("input");
        QTest::addColumn<std::string>("expected");

        QTest::newRow("single-digit")        <<     1 <<     "1"s;
        QTest::newRow("mult-digit")          <<  1234 <<  "1234"s;
        QTest::newRow("zero")                <<     0 <<     "0"s;
        QTest::newRow("negative")            <<    -1 <<    "-1"s;
        QTest::newRow("negative-mult-digit") << -1234 << "-1234"s;
    }

    void value() {
        QFETCH(int, input);
        QFETCH(std::string, expected);

        const auto field = fs::Field{"Field", int{}};
        const auto actual = field.value(bytes(input));
        QCOMPARE(expected, actual);
    }

    void valueWithCustomFunction() {
        constexpr int val = 1234;
        const fs::Field field{"Field", val, [](const int a) { return std::to_string(a + 1); }};
        QCOMPARE("1235", field.value(bytes(val)));
    }

    void valueWithCharArray() {
         constexpr char str[] = "A custom\ntest string";
        const auto field = fs::Field{"Field", str};
        QCOMPARE(str, field.value(str));
    }
};

QTEST_MAIN(FieldTest)
#include "FieldTest.moc"
