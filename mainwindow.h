// vim:set sw=4 et:
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidgetAction>

#include <memory>
#include <vector>

namespace Ui {
class MainWindow;
}

class AppMenu {
public:
    AppMenu()
        : menu(nullptr)
        , dummyAction("menu item", nullptr)
        , searchWidget(&menu)
        , searchAction(&menu)
    {
        searchAction.setDefaultWidget(&searchWidget);
        menu.addAction(&dummyAction);
        menu.addAction(&searchAction);
    }

    QMenu * get() { return &menu; }

private:
    QMenu menu;
    QAction dummyAction;
    QLineEdit searchWidget;
    QWidgetAction searchAction;
    std::vector<std::unique_ptr<QAction> > searches;
};

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
