// vim:set sw=4 et:
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include "appmenu.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QSystemTrayIcon> trayIcon;
    std::unique_ptr<AppMenu> search;
};

#endif // MAINWINDOW_H
