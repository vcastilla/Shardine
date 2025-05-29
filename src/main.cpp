#include <QApplication>
#include "contr/Controller.h"
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app{argc, argv};
    Controller contr;

    MainWindow main_window{contr};
    main_window.show();

    return QApplication::exec();
}
