// vim:set sw=4 et:
#include "appmenu.h"

std::vector<SearchResult> search(QString input) {
    std::vector<SearchResult> res = {
        {QString::fromUtf8("²"), "squared"},
        {QString::fromUtf8("ᵀ"), "transpose"},
        {QString::fromUtf8("σ"), "small sigma"},
        {QString::fromUtf8("θ"), "small theta"},
        {QString::fromUtf8("δ"), "small delta"},
        {QString::fromUtf8("∂"), "partial differential"},
        {QString::fromUtf8("∈"), "in / member of"},
        {QString::fromUtf8("ⁿ"), "nth power"},
        {QString::fromUtf8("∑"), "summation"},
        {input, "copy input"},
    };
    return res;
}
