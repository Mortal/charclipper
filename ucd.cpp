// vim:set sw=4 et:
#include "ucd.h"

#include <QFile>

struct ucd_parser {
    ucd_parser(const char * s) {
        bol = s;
        eol = strchr(bol, '\n');
    }

    const char * next() {
        return strchr(bol, '\n') + 1;
    }

    QString name() const {
        const char * i = strchr(bol, ';') + 1;
        const char * j = strchr(i, ';');
        return QString::fromUtf8(i, j - i);
    }

    uint32_t codepoint() const {
        uint32_t r = 0;
        const char * i = bol;
        const char * j = strchr(i, ';');
        while (i != j) {
            r *= 16;
            if (*i < 'A') r += (*i - '0');
            else r += (*i - 'A' + 10);
            ++i;
        }
        return r;
    }

    const char * bol;
    const char * eol;
};

/*
int ∗buildFail(char ∗p) {
int m = strlen(p); int ∗fail = new int[m+1];
for (int i = 1, j = fail[0] = -1; i <= m; ++i) {
while (j >= 0 && p[j] != p[i-1]) j = fail[j];
fail[i] = ++j;
} return fail;
}
int match(char ∗t, char ∗p, int ∗fail) {
int n = strlen(t), m = strlen(p); int cnt = 0;
for (int i = 0, k = 0; i < n; ++i) {
while (k >= 0 && p[k] != t[i]) k = fail[k];
if (++k >= m) k=fail[k],++cnt; //match at t[i-m+1 .. i]
} return cnt;
}
*/

class kmp {
public:
    kmp(QString p)
        : pattern(std::move(p))
    {
        fail.resize(pattern.size() + 1);
        fail[0] = -1;
        int j = -1;
        for (int i = 1; i <= pattern.size(); ++i) {
            while (j >= 0 && pattern[j] != pattern[i-1]) j = fail[j];
            fail[i] = ++j;
        }
        if (!match(pattern)) throw std::runtime_error("kmp does not match pattern");
    }

    bool match(const QString & text) {
        int k = 0;
        for (int i = 0; i < text.size(); ++i) {
            while (k >= 0 && pattern[k] != text[i]) k = fail[k];
            if (++k >= pattern.size()) return true;
        }
        return false;
    }

private:
    QString pattern;
    std::vector<int> fail;
};

class ucd_impl {
public:
    ucd_impl() : fp("UnicodeData.txt") {
        if (!fp.open(QFile::ReadOnly))
            throw std::runtime_error("QFile::open returned false");
        bof = (const char *) fp.map(0, fp.size());
        if (bof == nullptr)
            throw std::runtime_error("QFile::map returned null");
        eof = bof + fp.size();
    }

    unicode_character search(const QString & query) {
        kmp q(query);
        const char * i = bof;
        while (i < eof) {
            ucd_parser p(i);
            if (q.match(p.name()))
                return {p.codepoint(), p.name()};
            i = p.next();
        }
        return {0, ""};
    }

private:
    QFile fp;
    const char * bof;
    const char * eof;
};

unicode_character_database::unicode_character_database()
    : pimpl(new ucd_impl())
{
}

unicode_character_database::~unicode_character_database() {
}

unicode_character unicode_character_database::search(const QString & query) {
    return pimpl->search(query);
}
