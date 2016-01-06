// vim:set sw=4 et:
#ifndef APPMENU_H
#define APPMENU_H


#include <memory>
#include <vector>
#include <iostream>


#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QWidgetAction>
#include <QFuture>
#include <QtConcurrent>

#include "ucd.h"


struct SearchResult {
    QString literal;
    QString description;
};

std::vector<SearchResult> search(unicode_character_database & ucd, QString input);

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
        , searchAction(nullptr)
    {
        searchWidget = new QLineEdit(nullptr);
        searchAction.setDefaultWidget(searchWidget);
        menu.addAction(&dummyAction);
        menu.addAction(&searchAction);
        QObject::connect(
            searchWidget, &QLineEdit::textChanged,
            this, &AppMenu::searchChanged);
        QObject::connect(
            &dummyAction, &QAction::triggered,
            this, &AppMenu::dummyActionClick);
        QApplication * app = static_cast<QApplication *>(
            QCoreApplication::instance());
        // connect(app, &QApplication::focusChanged,
        //     [&] (QWidget * /*old*/, QWidget * now) {
        //         if (now == searchWidget) {
        //             menu.setActiveAction(&searchAction);
        //         }
        //     });
        connect(&menu, &QMenu::aboutToShow,
            [&] () {
                searchWidget->setFocus();
            });
        connect(&searchFutureWatcher, &QFutureWatcher<std::vector<SearchResult> >::finished,
                this, &AppMenu::searchFinished);
    }

    virtual ~AppMenu() {}

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
        searchFuture.cancel();
        searchFuture = QtConcurrent::run([this, text] () {
            return search(ucd, text);});
        searchFutureWatcher.setFuture(searchFuture);
    }

    void searchFinished() {
        if (searchFuture.isCanceled()) return;
        for (auto & res : searchFuture.result()) {
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
        c->setText(searchWidget->text());
    }

private:
    QMenu menu;
    QAction dummyAction;
    QWidgetAction searchAction;
    QLineEdit * searchWidget; // owned by searchAction
    std::vector<std::unique_ptr<QAction> > searches;
    unicode_character_database ucd;
    QFuture<std::vector<SearchResult> > searchFuture;
    QFutureWatcher<std::vector<SearchResult> > searchFutureWatcher;
};

#endif // APPMENU_H
