#include "point3d.h"

#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <cassert>
#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <unordered_set>

using Coord = int64_t;
using Point = Gfx_3d::Point<Coord>;
using Points = std::unordered_set<Point>;
using PointMap = std::unordered_map<Point, int>;

int get_id()
{
    static int last = 0;
    return ++last;
}


void part1(Points const& points)
{
    PointMap pointmap;

    for (auto const& px : points) {
        pointmap.insert({px, 0});
    }

    for (int i{0}; i < 10; ++i) {
        Point a, b;
        double dist{std::numeric_limits<double>::max()};

        for (auto it1{pointmap.cbegin()}; it1 != pointmap.cend(); ++it1) {
            if (it1->second)
                continue;

            for (auto it2{pointmap.cbegin()}; it2 != pointmap.cend(); ++it2) {
                if (it1 == it2)
                    continue;

                const double oth_dist{it1->first.euclidean_dist(it2->first)};
                //  fmt::print(
                //          "{}: test are ({}, {}, {}) and ({}, {}, {}) = {:.5f}\n",
                //          i,
                //          it1->first.x,
                //          it1->first.y,
                //          it1->first.z,
                //          it2->first.x,
                //          it2->first.y,
                //          it2->first.z,
                //          oth_dist);
                if (oth_dist < dist) {
                    a = it1->first;
                    b = it2->first;
                    dist = oth_dist;
                }
            }
        }

        assert(dist != std::numeric_limits<double>::max());

        int next_id{pointmap.at(b)};
        if (!next_id) {
            next_id = get_id();
            pointmap.at(b) = next_id;
        }
        assert(pointmap.at(a) == 0);
        pointmap.at(a) = next_id;

        fmt::print("{}: shortest are ({}, {}, {}) and ({}, {}, {}) = {}\n", i, a.x, a.y, a.z, b.x, b.y, b.z, dist);
    }

    std::map<int, std::vector<Point>> groups;

    for (auto const& [px, id] : pointmap) {
        groups[id].push_back(px);
    }

    for (auto const& [id, group] : groups) {
        fmt::print("{}:\t", id);
        for (auto const& px : group) {
            fmt::print("({}, {}, {}), ", px.x, px.y, px.z);
        }
        fmt::print("\n");
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

    part1(points);

    return 0;
}
