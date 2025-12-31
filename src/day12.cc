#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

enum class ParseState {
    Start,
    ShapeHeader,
    ShapeData,
    Area
};

struct Area {
    unsigned dx{0}, dy{0};
    std::vector<int> shapeCount;
};


void part1(std::vector<uint32_t> const& shapes, std::vector<Area> const& areas)
{
    unsigned passed{0};

    for (auto const& a : areas) {
        uint32_t shapeArea{0};
        for (auto const& [shapeSize, shapeCount] : std::views::zip(shapes, a.shapeCount)) {
            if (shapeCount) {
                shapeArea += shapeSize * shapeCount;
            }
        }

        if (shapeArea <= (a.dx * a.dy)) {
            ++passed;
        }
    }

    fmt::print("1: {}\n", passed);
}


int main()
{
    std::vector<uint32_t> shapes(6, 0);
    std::vector<Area> areas;

    auto state {ParseState::Start};
    int shapeId{-1};
    uint32_t shapeSize{0};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            if (state == ParseState::Area) {
                break;
            }
            else if (state == ParseState::ShapeHeader) {
                shapes.at(shapeId) = shapeSize;
                shapeSize = 0;
                shapeId = -1;
                state = ParseState::Start;
            } else {
                assert(false);
            }
        }
        else {
            if (state == ParseState::ShapeHeader) {
                shapeSize += std::ranges::count_if(line, [](auto const c) { return c == '#'; });
                continue;
            }
            else if (state == ParseState::Start && !line.contains('x')) {
                shapeId = std::atoi(&line[0]);
                state = ParseState::ShapeHeader;
                continue;
            }

            state = ParseState::Area;

            // expecting NxN or newline
            std::deque<std::string> parts;
            boost::algorithm::split(parts, line, boost::is_any_of("x: "), boost::algorithm::token_compress_on);
            assert(parts.size() == 8);

            Area a;
            a.dx = std::stoi(parts.front());
            parts.pop_front();
            a.dy = std::stoi(parts.front());
            parts.pop_front();
            for (auto const& s : parts) {
                a.shapeCount.push_back(std::stoi(s));
            }
            areas.push_back(std::move(a));
        }
    }

    part1(shapes, areas);

    return 0;
}
