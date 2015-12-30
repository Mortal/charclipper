#include "appmenu.h"

std::vector<SearchResult> search(QString input) {
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
