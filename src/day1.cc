#include <fmt/core.h>

#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

int main()
{
    int dial{50};

    unsigned zeros{0};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        int ptr = std::stoi(&line[1]);
        if (line.at(0) == 'L') {
            ptr *= -1;
        }

        dial += ptr;

        while (dial < 0) {
            dial += 100;
        }

        dial %= 100;

        if (dial == 0) {
            ++zeros;
        }
    }

    fmt::print("1: {}\n", zeros);
    //  fmt::print("2: {}\n", sum);

    return 0;
}
