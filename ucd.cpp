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
        if (pattern.size() > 0) {
            fail.resize(pattern.size() + 1);
            fail[0] = -1;
            int j = -1;
            // fail[i] = length of longest suffix of p[:i] that is a prefix of p[:i]
            for (int i = 1; i <= pattern.size(); ++i) {
                while (j >= 0 && pattern[j] != pattern[i-1]) j = fail[j];
                fail[i] = ++j;
            }
        }
        if (!match(pattern)) throw std::runtime_error("kmp does not match pattern");
    }

    int match_end(const QString & text, int start=0) const {
        if (pattern.size() == 0) return start;
        int k = 0;
        for (int i = start; i < text.size(); ++i) {
            while (k >= 0 && pattern[k] != text[i]) k = fail[k];
            if (++k >= pattern.size()) return i + 1;
        }
        return -1;
    }

    bool match(const QString & text) const {
        return match_end(text) > -1;
    }

private:
    QString pattern;
    std::vector<int> fail;
};

class multi_kmp {
public:
    multi_kmp(QString p, QChar sep) {
        for (auto s : p.split(sep)) {
            if (s.size() > 0) matchers.emplace_back(s);
        }
    }

    int match_end(const QString & text, int start=0) const {
        for (const kmp & m : matchers) {
            start = m.match_end(text, start);
            if (start == -1) return -1;
        }
        return start;
    }

    bool match(const QString & text) const {
        return match_end(text) > -1;
    }

private:
    std::vector<kmp> matchers;
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

    template <typename F>
    void find_all(const QString & query, F f) {
        // TODO Implement QFutureInterface
        multi_kmp q(query, ' ');
        const char * i = bof;
        while (i < eof) {
            ucd_parser p(i);
            if (q.match(p.name()))
                f(unicode_character {p.codepoint(), p.name()});
            i = p.next();
        }
    }

    unicode_character search(const QString & query) {
        unicode_character best {0, ""};
        find_all(query, [&] (unicode_character match) {
            if (best.name.size() == 0 || match.name.size() < best.name.size())
                best = std::move(match);
        });
        return best;
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
