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

#include <QApplication>
#include <QTranslator>
#include "contr/Controller.h"
#include "project.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};
    QTranslator translator;

    if (!translator.load(QLocale(), project::name, "_", ":/i18n"))
        qDebug() << "Failed to load locale " << QLocale().name();

    QApplication::installTranslator(&translator);

    Controller contr;
    MainWindow main_window{contr};
    main_window.show();

    return QApplication::exec();
}
