// vim:set sw=4 et:
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QClipboard>
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

class AppMenu : public QObject {
    Q_OBJECT

    static constexpr const char * T_emptySearch = "Search...";

public:
    AppMenu()
        : menu(nullptr)
        , dummyAction(T_emptySearch, nullptr)
        , searchWidget(&menu)
        , searchAction(&menu)
    {
        searchAction.setDefaultWidget(&searchWidget);
        menu.addAction(&dummyAction);
        menu.addAction(&searchAction);
        QObject::connect(
            &searchWidget, &QLineEdit::textChanged,
            this, &AppMenu::searchChanged);
        QObject::connect(
            &dummyAction, &QAction::triggered,
            this, &AppMenu::dummyActionClick);
    }

    QMenu * get() { return &menu; }

public slots:
    void searchChanged(const QString & text) {
        if (text.size() == 0)
            dummyAction.setText(T_emptySearch);
        else
            dummyAction.setText(text);
    }

    void dummyActionClick(bool /*checked*/) {
        QClipboard * c = QApplication::clipboard();
        c->setText(searchWidget.text());
    }

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
