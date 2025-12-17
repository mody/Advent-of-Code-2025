#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

using Row = std::vector<uint64_t>;
using Rows = std::vector<Row>;
using StringVect = std::vector<std::string>;

void part1(Rows const& values, StringVect const& ops)
{
    uint64_t sum{0};

    for (auto const& [idx, op] : ops | std::views::enumerate) {
        if (op == "+") {
            for (auto const& row : values) {
                sum += row.at(idx);
            }
        }
        else {
            uint64_t tmp{1};
            for (auto const& row : values) {
                if (row.at(idx) == 0) {
                    tmp = 0;
                    break;
                }
            }
            if (tmp) {
                for (auto const& row : values) {
                    tmp *= row.at(idx);
                }
                sum += tmp;
            }
        }
    }

    fmt::print("1: {}\n", sum);
}


void part2(StringVect input)
{
    std::ranges::reverse(input);
    for (auto& line : input) {
        std::ranges::reverse(line);
    }

    uint64_t sum{0};
    Row r;

    for (size_t pos{0}; pos < input.at(0).size(); ++pos) {
        int exp{0};
        char op{0};
        uint64_t val{0};

        for (auto const& line : input) {
            char c{line.at(pos)};
            if (c == ' ') {
                continue;
            } else if (c == '+') {
                op = '+';
                continue;
            } else if (c == '*') {
                op = '*';
                continue;
            }
            assert(c >= '0');
            assert(c <= '9');

            c -= '0';
            val += c * std::pow(10, exp++);
        }

        r.push_back(val);

        if (op) {
            if (op == '+') {
                sum += std::ranges::fold_left(r, 0, std::plus{});
            } else {
                if (std::ranges::find(r, 0) == r.cend()) {
                    sum += std::ranges::fold_left(r, 1, std::multiplies{});
                }
            }
            r.clear();
            ++pos;
        }
    }

    fmt::print("2: {}\n", sum);
}

int main()
{
    Rows values;
    StringVect ops;

    StringVect input;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        input.push_back(line);

        ops.clear();

        boost::algorithm::trim(line);
        boost::algorithm::split(ops, line, boost::is_any_of(" "), boost::algorithm::token_compress_on);

        if (ops.at(0) == "+" || ops.at(0) == "*")
            break;

        Row r;
        for (auto const& v : ops) {
            r.push_back(std::stoull(&v[0]));
        }
        values.push_back(std::move(r));
    }

    assert(ops.size() == values.at(0).size());

    //  part1(values, ops);
    part2(input);

    return 0;
}
