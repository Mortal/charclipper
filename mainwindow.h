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

struct SearchResult {
    QString literal;
    QString description;
};

inline std::vector<SearchResult> search(QString input) {
    return {
        {QString::fromUtf8("²"), "squared"},
        {QString::fromUtf8("ᵀ"), "transpose"},
        {QString::fromUtf8("σ"), "small sigma"},
        {QString::fromUtf8("θ"), "small theta"},
        {QString::fromUtf8("∈"), "in / member of"},
        {QString::fromUtf8("ⁿ"), "nth power"},
        {input, "copy input"},
    };
}

struct CopyOnClick {
    QString text;

    void operator()(bool /*checked*/ = false) {
        QClipboard * c = QApplication::clipboard();
        c->setText(text);
    }
};

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
        QApplication * app = static_cast<QApplication *>(
            QCoreApplication::instance());
        // connect(app, &QApplication::focusChanged,
        //     [&] (QWidget * /*old*/, QWidget * now) {
        //         if (now == &searchWidget) {
        //             menu.setActiveAction(&searchAction);
        //         }
        //     });
        connect(&menu, &QMenu::aboutToShow,
            [&] () {
                searchWidget.setFocus();
            });
    }

    QMenu * get() { return &menu; }

public slots:
    void searchChanged(const QString & text) {
        if (text.size() == 0)
            dummyAction.setText(T_emptySearch);
        else
            dummyAction.setText(text);
        for (auto & act : searches) {
            menu.removeAction(act.get());
        }
        searches.resize(0);
        auto results = search(text);
        for (auto & res : results) {
            searches.emplace_back(new QAction(res.description, nullptr));
            menu.addAction(searches.back().get());
            QObject::connect(
                searches.back().get(), &QAction::triggered,
                CopyOnClick {res.literal});
        }
        if (searches.size() > 0) {
            menu.setActiveAction(searches.front().get());
        }
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
