#include <QKeyEvent>

#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_F11:
    case Qt::Key_F:
        if (this->isFullScreen()) {
            this->showNormal();
        } else {
            this->showFullScreen();
        }
        break;

    case Qt::Key_Escape:
        this->showNormal();
        break;

    default:
        break;
    }


}
