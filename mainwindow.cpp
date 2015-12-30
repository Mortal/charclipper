// vim:set sw=4 et:
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    trayIcon(new QSystemTrayIcon)
{
    ui->setupUi(this);
    // QMenu * menuTray = findChild<QMenu *>("menuTray");
    QAction * actionTest = findChild<QAction *>("actionTest");
    connect(actionTest, &QAction::triggered, [] () {
        QClipboard * c = QApplication::clipboard();
        c->setText("Hej");
    });
    this->search.reset(new AppMenu);
    trayIcon->setContextMenu(this->search->get());
    trayIcon->show();
}

MainWindow::~MainWindow()
{
}
