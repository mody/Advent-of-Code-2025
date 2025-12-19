#include "point3d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <cassert>
#include <deque>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_set>

using Coord = int64_t;
using Point = Gfx_3d::Point<Coord>;
using Points = std::unordered_set<Point>;
using PointMap = std::unordered_map<Point, int>;

struct Connection {
    Point from, to;
    double dist;
};

unsigned get_id()
{
    static int last = 0;
    return ++last;
}


void process(Points const& points)
{
    auto print_part1 = [](const PointMap& pm)
    {
        std::map<int, std::vector<Point>> g1;
        for (auto const& [px, id] : pm) {
            g1[id].push_back(px);
        }

        std::vector<size_t> sizes;
        for (auto const& [id, group] : g1) {
            if (id)
                sizes.push_back(group.size());
        }
        std::ranges::sort(sizes, std::greater{});
        fmt::print("1: {}\n", sizes.at(0) * sizes.at(1) * sizes.at(2));
    };

    PointMap pointmap;
    for (auto const& px : points) {
        pointmap.insert({px, 0});
    }

    std::vector<Connection> connections;
    for (auto it1{points.cbegin()}; it1 != points.cend(); ++it1) {
        for (auto it2{std::next(it1)}; it2 != points.cend(); ++it2) {
            connections.push_back({.from = *it1, .to = *it2, .dist = it1->euclidean_dist(*it2)});
        }
    }

    std::ranges::sort(connections, [](const auto& a, const auto& b) { return a.dist < b.dist; });

    std::map<unsigned, unsigned> ids;
    ids.insert({0u, pointmap.size()});

    int iter{0};
    for (auto const& c : connections) {
        auto& id1{pointmap.at(c.from)};
        auto& id2{pointmap.at(c.to)};

        if (id1 == 0 && id2 == 0) {
            ids.at(id1) -= 1;
            ids.at(id2) -= 1;
            id1 = id2 = get_id();
            ids.insert({id1, 2});
        }
        else if (id1 == 0) {
            ids.at(id1) -= 1;
            ids.at(id2) += 1;
            id1 = id2;
        }
        else if (id2 == 0) {
            ids.at(id2) -= 1;
            ids.at(id1) += 1;
            id2 = id1;
        } else if (id1 != id2) {
            // merge
            const int old_id{std::max(id2, id1)};
            const int new_id{std::min(id2, id1)};
            for (auto& [px, id3] : pointmap) {
                if (id3 == old_id) {
                    id3 = new_id;
                    ids.at(old_id) -= 1;
                    ids.at(new_id) += 1;
                }
            }
            ids.erase(old_id);
        }
        assert(id1 == id2);
        if (++iter == 1000) {
            print_part1(pointmap);
        }

        if (iter >= 10 && ids.contains(1) && ids.at(1) == pointmap.size()) {
            fmt::print("2: {}\n", c.from.x * c.to.x);
            break;
        }
    }
}

int main()
{
    Points points;

    {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty())
                break;

            std::deque<std::string> items;
            boost::algorithm::split(items, line, boost::is_any_of(","), boost::algorithm::token_compress_on);

            points.insert(
                    Point{std::stoll(items[0].c_str()), std::stoll(items[1].c_str()), std::stoll(items[2].c_str())});
        }
    }

    process(points);

    return 0;
}
