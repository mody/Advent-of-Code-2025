#include <fmt/core.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <ranges>

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
        } else {
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


void part2(Rows const& values, StringVect const& ops)
{
    //  uint64_t sum{0};
    //  fmt::print("2: {}\n", sum);
}

int main()
{
    Rows values;
    StringVect ops;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

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

    part1(values, ops);
    part2(values, ops);

    return 0;
}
