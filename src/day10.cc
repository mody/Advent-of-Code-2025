#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <chrono>
#include <deque>
#include <iostream>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#if 0
[.##.] (3) (1,3) (2) (2,3) (0,2) (0,1) {3,5,4,7}
[...#.] (0,2,3,4) (2,3) (0,4) (0,1,2) (1,2,3,4) {7,5,12,7,2}
[.###.#] (0,1,2,3,4) (0,3,4) (0,1,2,4,5) (1,2) {10,11,11,5,10,5}
#endif

using Button = std::set<unsigned>;
using Joltage = std::vector<unsigned>;

struct Machine
{
    std::bitset<32> expected{0}, state{0};
    std::vector<Button> buttons;

    Joltage joltage_expected;
    Joltage joltage_state;

    uint32_t steps{0};
};

using Machines = std::vector<Machine>;


void part1_v1(Machines const& machines)
{
    uint64_t sum{0};

    for (Machine machine : machines) {
        std::set<uint64_t> visited;
        std::deque<Machine> wq;
        wq.push_back(std::move(machine));

        while(!wq.empty()) {
            auto const m{wq.front()};
            wq.pop_front();

            if (m.state == m.expected) {
                sum += m.steps;
                break;
            }

            if (visited.insert(m.state.to_ulong()).second == false) {
                continue;
            }

            for (auto const& b : m.buttons) {
                auto m2{m};
                for (auto pos : b) {
                    m2.state.flip(pos);
                }
                ++m2.steps;
                wq.push_back(std::move(m2));
            }
        }
    }

    fmt::print("1: {}\n", sum);
}


void part1_v2(Machines const& machines)
{
    uint64_t sum{0};


    for (Machine machine : machines) {
        std::vector<std::bitset<32>> result;

        for (uint32_t possible{0}; possible < (1U << machine.buttons.size()); ++possible) {
            const std::bitset<32> pushed(possible);
            std::bitset<32> leds;
            for (const auto [idx, butt] : std::views::enumerate(machine.buttons)) {
                if (pushed.test(idx)) {
                    for (auto b : butt) {
                        leds.flip(b);
                    }
                }
            }
            if (leds == machine.expected) {
                result.push_back(pushed);
            }
        }

        std::ranges::sort(result, [](auto const& a, auto const& b) { return a.count() < b.count(); });
        sum += result.front().count();
    }

    fmt::print("1: {}\n", sum);
}


int main()
{
    Machines machines;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        std::vector<std::string> groups;
        boost::algorithm::split(groups, line, boost::is_any_of(" "), boost::algorithm::token_compress_on);

        Machine m;

        for (auto const& g : groups) {
            if (g.at(0) == '[') {
                for (auto [pos, c] : std::views::enumerate(g.substr(1, g.size()-1))) {
                    if (c == '#') {
                        m.expected.set(pos);
                    }
                }
            }
            else {
                std::vector<std::string> numbers;
                boost::algorithm::split(
                        numbers, g.substr(1, g.size() - 1), boost::is_any_of(","), boost::algorithm::token_compress_on);
                std::vector<unsigned> values;
                for (auto const& v : numbers) {
                    values.push_back(std::stoi(v));
                }
                if (g.at(0) == '(') {
                    m.buttons.push_back(Button{values.begin(), values.end()});
                }
                else if (g.at(0) == '{') {
                    m.joltage_expected = Joltage{values.begin(), values.end()};
                    m.joltage_state.resize(m.joltage_expected.size(), 0);
                }
            }
        }

        machines.push_back(std::move(m));
    }

    {
        auto startTime{std::chrono::high_resolution_clock::now()};
        part1_v1(machines);
        auto endTime{std::chrono::high_resolution_clock::now()};
        fmt::print("v1 took {}us\n", std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
    }
    {
        auto startTime{std::chrono::high_resolution_clock::now()};
        part1_v2(machines);
        auto endTime{std::chrono::high_resolution_clock::now()};
        fmt::print("v2 took {}us\n", std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());
    }

    return 0;
}
