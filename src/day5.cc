#include <fmt/core.h>

#include <boost/icl/interval.hpp>
#include <boost/icl/interval_set.hpp>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using Val = uint64_t;

struct Recipes
{
    using Interval = boost::icl::interval<Val>;
    using ISet = boost::icl::interval_set<Val>;

    ISet recipes;

    bool contains(Val v) const
    {
        return recipes.find(v) != recipes.end();
    }

    Val all_values() const
    {
        Val total{0};
        for (auto const& i : recipes) {
            total += i.upper() - i.lower() + 1;
        }
        return total;
    }

    void add(Val from, Val to) { recipes += Interval::closed(from, to); }
};

int main()
{
    Recipes recipes;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        const auto sep{line.find('-')};
        line.at(sep) = '\0';
        recipes.add(std::stoull(&line[0]), std::stoull(&line[sep + 1]));
    }

    Val cnt1{0};

    while (std::getline(std::cin, line)) {
        if (line.empty())
            break;

        const Val v{std::stoull(&line[0])};
        if (recipes.contains(v)) {
            ++cnt1;
        }
    }

    fmt::print("1: {}\n", cnt1);
    fmt::print("2: {}\n", recipes.all_values());

    return 0;
}
