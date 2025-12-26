#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>

#include <oneapi/tbb/parallel_for_each.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <vector>

using Button = std::set<int>;
using Joltage = std::vector<int>;
using Leds = std::bitset<32>;

struct Machine
{
    Leds expected{0};

    std::vector<Button> buttons;
    Joltage joltage;
};

using Machines = std::vector<Machine>;

namespace {


Leds make_leds(Joltage const& joltage)
{
    Leds result{0};

    for (auto const& [pos, j] : std::views::enumerate(joltage)) {
        if (j & 1) {
            result.set(pos);
        }
    }

    return result;
}


std::vector<Leds> solve_leds(Leds const& expected, std::vector<Button> const& buttons)
{
    std::vector<Leds> result;

    for (uint32_t possible{0}; possible < (1U << buttons.size()); ++possible) {
        const Leds pushed(possible);
        Leds leds;
        for (const auto [idx, butt] : std::views::enumerate(buttons)) {
            if (pushed.test(idx)) {
                for (auto b : butt) {
                    leds.flip(b);
                }
            }
        }
        if (leds == expected) {
            result.push_back(pushed);
        }
    }

    std::ranges::sort(result, [](auto const& a, auto const& b) { return a.count() < b.count(); });
    return result;
}


std::optional<Joltage> substract_leds(Leds const& leds, std::vector<Button> const& buttons, Joltage joltage)
{
    for (const auto [idx, butt] : std::views::enumerate(buttons)) {
        if (leds.test(idx)) {
            for (auto b : butt) {
                if(--joltage.at(b) < 0) {
                    return std::nullopt;
                }
            }
        }
    }

    return joltage;
}

Joltage halve_joltage(Joltage joltage)
{
    for (auto& j : joltage) {
        j = j / 2;
    }
    return joltage;
}


uint32_t iterate_joltage(
        std::vector<Button> const& buttons,
        Joltage const& joltage,
        std::unordered_map<uint64_t, uint32_t>& result_cache,
        std::map<uint32_t, std::vector<Leds>> const& led_cache)
{
    auto const& key{boost::hash_range(joltage.cbegin(), joltage.cend())};
    if (auto it{result_cache.find(key)}; it != result_cache.end()) {
        return it->second;
    }

    if (std::ranges::all_of(joltage, [](auto const& v) { return v == 0; })) {
        result_cache.insert({key, 0});
        return 0;
    }

    std::vector<uint32_t> scores;

    if (make_leds(joltage).none()) {
        scores.push_back(2 * iterate_joltage(buttons, halve_joltage(joltage), result_cache, led_cache));
    }

    for (auto const& led : led_cache.at(make_leds(joltage).to_ullong())) {
        if (led == 0)
            continue;
        auto next_joltage{substract_leds(led, buttons, joltage)};
        if (!next_joltage) {
            continue;
        }
        scores.push_back(led.count() + iterate_joltage(buttons, next_joltage.value(), result_cache, led_cache));
    }

    if (scores.empty()) {
        // no solution
        scores.push_back(1000000);
    }

    auto it{std::min_element(scores.cbegin(), scores.cend())};
    result_cache.insert({key, *it});
    return *it;
}


}  // namespace


void part1_v2(Machines const& machines)
{
    uint64_t sum{0};

    for (Machine machine : machines) {
        auto result{solve_leds(machine.expected, machine.buttons)};
        sum += result.front().count();
    }

    fmt::print("1: {}\n", sum);
}


void part2(Machines const& machines)
{
    std::atomic<uint64_t> sum{0};

    tbb::parallel_for_each(
            machines,
            [&sum](auto const& machine)
            {
                std::map<uint32_t, std::vector<Leds>> led_cache;
                for (uint32_t led{0}; led < (1U << machine.joltage.size()); ++led) {
                    led_cache.insert(std::make_pair(led, solve_leds(led, machine.buttons)));
                }
                std::unordered_map<uint64_t, uint32_t> result_cache;
                sum.fetch_add(iterate_joltage(machine.buttons, machine.joltage, result_cache, led_cache));
            });

    //  for (auto const& [line, machine] : std::views::enumerate(machines)) {
    //      std::map<uint32_t, std::vector<Leds>> led_cache;
    //      for (uint32_t led{0}; led < (1U << machine.joltage.size()); ++led) {
    //          led_cache.insert(std::make_pair(led, solve_leds(led, machine.buttons)));
    //      }

    //      std::unordered_map<uint64_t, uint32_t> result_cache;
    //      auto score{iterate_joltage(machine.buttons, machine.joltage, result_cache, led_cache)};
    //      //  fmt::print("Line {}/{}: answer {}\n", line+1, machines.size(), score);
    //      fflush(stdout);
    //      sum.fetch_add(score);
    //  }

    fmt::print("2: {}\n", sum.load());
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
                for (auto [pos, c] : std::views::enumerate(g.substr(1, g.size() - 1))) {
                    if (c == '#') {
                        m.expected.set(pos);
                    }
                }
            }
            else {
                std::vector<std::string> numbers;
                boost::algorithm::split(
                        numbers,
                        g.substr(1, g.size() - 1),
                        boost::is_any_of(", "),
                        boost::algorithm::token_compress_on);
                std::vector<int> values;
                for (auto const& v : numbers) {
                    values.push_back(std::stoi(v));
                }
                if (g.at(0) == '(') {
                    m.buttons.push_back(Button{values.begin(), values.end()});
                }
                else if (g.at(0) == '{') {
                    m.joltage = Joltage{values.begin(), values.end()};
                }
            }
        }

        machines.push_back(std::move(m));
    }

    part1_v2(machines);
    part2(machines);

    return 0;
}
