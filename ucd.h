// vim:set sw=4 et:
#ifndef UCD_H
#define UCD_H

#include <QString>

#include <memory>

struct unicode_character {
    uint codepoint;
    QString name;
};

class ucd_impl;

class unicode_character_database {
public:
    unicode_character_database();
    ~unicode_character_database();
    unicode_character search(const QString & query);

private:
    std::unique_ptr<ucd_impl> pimpl;
};

#endif // UCD_H
