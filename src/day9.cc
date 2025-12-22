#include "point2d.h"

#include <fmt/core.h>
#include <fmt/ranges.h>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Points = std::unordered_set<Point>;


struct Compressed
{
    static std::vector<Coord> map_x;
    static std::vector<Coord> map_y;

    std::vector<Coord>::iterator ix{map_x.end()}, iy{map_y.end()};


    constexpr Compressed() noexcept = default;
    constexpr Compressed(Point const& px) noexcept
        : ix{std::ranges::find(map_x, px.x)}
        , iy{std::ranges::find(map_y, px.y)}
    {
        assert(ix != map_x.cend());
        assert(iy != map_y.cend());
    }

    constexpr Compressed(Coord x_, Coord y_) noexcept
        : ix{std::next(map_x.begin(), x_)}
        , iy{std::next(map_y.begin(), y_)}
    {
    }

    constexpr Compressed(Compressed const&) noexcept = default;
    constexpr Compressed(Compressed&&) noexcept = default;

    constexpr Compressed& operator= (Compressed const&) noexcept = default;
    constexpr Compressed& operator= (Compressed&&) noexcept = default;

    constexpr auto operator<=> (Compressed const&) const = default;

    constexpr Compressed& operator+= (Gfx_2d::Direction const& o) noexcept
    {
        std::advance(ix, o.dx);
        std::advance(iy, o.dy);
        return *this;
    }

    constexpr friend Compressed operator+ (Compressed lhs, Gfx_2d::Direction const& o) noexcept
    {
        lhs += o;
        return lhs;
    }

    Coord x() const { return ix != map_x.end() ? std::distance(map_x.begin(), ix) : std::numeric_limits<Coord>::max(); }
    Coord y() const { return iy != map_y.end() ? std::distance(map_y.begin(), iy) : std::numeric_limits<Coord>::max(); }

    Point to_point() const { return {*ix, *iy}; }
};


std::vector<Coord> Compressed::map_x;
std::vector<Coord> Compressed::map_y;


struct Square
{
    Point a, b;
    int64_t area{0};
};


struct World
{
    Points const& points;

    Coord min_x{std::numeric_limits<Coord>::max()}, min_y{std::numeric_limits<Coord>::max()}, max_x{0}, max_y{0};

    std::string data;

    Square max_a;


    World(Points const& p)
        : points{p}
    {
        minmax_area();

        for (auto const& px : points) {
            Compressed::map_x.push_back(px.x);
            Compressed::map_y.push_back(px.y);
        }

        std::ranges::sort(Compressed::map_x);
        std::ranges::sort(Compressed::map_y);

        Compressed::map_x.erase(std::unique(Compressed::map_x.begin(), Compressed::map_x.end()), Compressed::map_x.end());
        Compressed::map_y.erase(std::unique(Compressed::map_y.begin(), Compressed::map_y.end()), Compressed::map_y.end());

        min_x = min_y = 0;
        max_x = Compressed::map_x.size();
        max_y = Compressed::map_y.size();

        data.resize(max_y * max_x, '.');

        for (auto const& px : points) {
            set(Compressed{px}, '#');
        }
    }

    char get(Compressed const& px) const { return data.at(px.y() * max_x + px.x()); }
    void set(Compressed const& px, char c) { data.at(px.y() * max_x + px.x()) = c; }

    bool inside(Compressed const& px) const
    {
        return px.x() >= 0 && px.x() < max_x && px.y() >= 0 && px.y() < max_y
                && (px.y() * max_x + px.x()) < (max_y * max_x);
    };

    bool contains(Compressed const& px) const { return data.at(px.y() * max_x + px.x()) != '.'; }


    void dump_points() const
    {
        for (Coord y = 0; y < max_y; ++y) {
            for (Coord x = 0; x < max_x; ++x) {
                const Point px{x, y};
                auto it{points.find(px)};
                fmt::print("{:c}", it != points.cend() ? '#' : '.');
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }

    void dump_data() const
    {
        std::string_view v{data};
        for (Coord y = 0; y < max_y; ++y) {
            fmt::print("{}\n", v.substr(y * max_x, max_x));
        }
        fmt::print("\n");
    }

    void minmax_area()
    {
        std::multimap<int64_t, Square> areas;

        for (auto it1{points.begin()}; it1 != points.end(); ++it1) {
            auto const& p1{*it1};
            for (auto it2{std::next(it1)}; it2 != points.end(); ++it2) {
                auto const& p2{*it2};
                const int64_t dx{std::abs(p2.x - p1.x) + 1};
                const int64_t dy{std::abs(p2.y - p1.y) + 1};
                const int64_t area{dx * dy};

                areas.insert({area, Square{.a = std::min(p1, p2), .b = std::max(p1, p2), .area = area}});
            }
        }

        max_a = areas.rbegin()->second;
    }


    void make_line(Compressed const& from, Compressed const& to)
    {
        Gfx_2d::Direction dir;
        char c;

        if (from.x() == to.x()) {
            dir = (from.y() < to.y()) ? Gfx_2d::Down : Gfx_2d::Up;
            c = '|';
        }
        else if (from.y() == to.y()) {
            dir = (from.x() < to.x()) ? Gfx_2d::Right : Gfx_2d::Left;
            c = '-';
        }
        else {
            assert(false);
        }

        for (Compressed px{from + dir}; px != to; px += dir) {
            set(px, c);
        }
    }

    void part1() { fmt::print("1: {}\n", max_a.area); }

    void part2()
    {
        for (auto it1{points.begin()}; it1 != points.end(); ++it1) {
            auto const& p1{*it1};
            bool hasX{false}, hasY{false};
            for (auto it2{std::next(it1)}; it2 != points.end(); ++it2) {
                auto const& p2{*it2};
                if (p1.x == p2.x) {
                    hasY = true;
                    make_line(p1, p2);
                }
                else if (p1.y == p2.y) {
                    hasX = true;
                    make_line(p1, p2);
                }

                if (hasX && hasY)
                    break;
            }
        }


        auto test_inside = [this](Compressed const& start,
                                  Gfx_2d::Direction out_dir,
                                  Gfx_2d::Direction in_dir,
                                  char wall) -> std::optional<Compressed>
        {
            for (auto outer{start}; inside(outer); outer += out_dir) {
                for (auto inner{outer}; inside(inner); inner += in_dir) {
                    if (!contains(inner))
                        continue;
                    if (get(inner) != wall) {
                        break;
                    }
                    inner += in_dir;
                    if (inside(inner) && !contains(inner)) {
                        return inner;
                    }
                    break;
                }
            }
            return std::nullopt;
        };

        Compressed inside_px;
        if (auto x{test_inside({0, 0}, Gfx_2d::Down, Gfx_2d::Right, '|')}; x) {
            inside_px = x.value();
        }
        else if (x = test_inside({0, 0}, Gfx_2d::Right, Gfx_2d::Down, '-'); x) {
            inside_px = x.value();
        }
        else if (x = test_inside({max_x - 1, max_y - 1}, Gfx_2d::Up, Gfx_2d::Left, '|'); x) {
            inside_px = x.value();
        }
        else if (x = test_inside({max_x - 1, max_y - 1}, Gfx_2d::Left, Gfx_2d::Up, '-'); x) {
            inside_px = x.value();
        }


        std::deque<Compressed> work;
        work.push_back(inside_px);
        while (!work.empty()) {
            auto px{work.front()};
            work.pop_front();

            assert(inside(px));
            if(contains(px)) continue;

            set(px, 'X');
            for (auto const& dir :
                 {Gfx_2d::Up,
                  Gfx_2d::Down,
                  Gfx_2d::Left,
                  Gfx_2d::Right
                  })
            {
                auto p2{px + dir};
                if (inside(p2) && !contains(p2)) {
                    work.push_back(std::move(p2));
                }
            }
        }

        Square full_a;

        for (auto it1{points.begin()}; it1 != points.end(); ++it1) {
            auto const& from{*it1};
            for (auto it2{std::next(it1)}; it2 != points.end(); ++it2) {
                auto const& to{*it2};

                if (from.x == to.x || from.y == to.y)
                    continue;

                const int64_t dx{std::abs(to.x - from.x) + 1};
                const int64_t dy{std::abs(to.y - from.y) + 1};
                const int64_t area{dx * dy};

                if (full_a.area >= area) {
                    continue;
                }

                Compressed a{Point{std::min(from.x, to.x), std::min(from.y, to.y)}};
                const Compressed b{Point{std::max(from.x, to.x), std::max(from.y, to.y)}};

                bool fail{false};
                for (Compressed outer{a}; outer.y() <= b.y(); outer += Gfx_2d::Down) {
                    for (Compressed inner{outer}; inner.x() <= b.x(); inner += Gfx_2d::Right) {
                        if (!contains(inner)) {
                            fail = true;
                            break;
                        }
                    }
                }

                if (fail) {
                    continue;
                }

                assert(area > full_a.area);
                full_a.area = area;
                full_a.a = from;
                full_a.b = to;
            }
        }

        fmt::print("2: {}\n", full_a.area);
    }
};


int main()
{
    Points points;

    {
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty())
                break;

            const auto sep{line.find(',')};
            line.at(sep) = '\0';
            points.insert({std::stoi(&line[0]), std::stoi(&line[sep + 1])});
        }
    }

    World world(points);

    world.part1();
    world.part2();

    return 0;
}
