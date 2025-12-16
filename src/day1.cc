#include <fmt/core.h>

#include <cassert>
#include <iostream>
#include <string>

int main()
{
    int dial{50};

    unsigned zeros1{0}, zeros2{0};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        int ptr = std::stoi(&line[1]);
        int sign{1};

        if (line.at(0) == 'L') {
            sign = -1;
        }

        for (int i{0}; i < ptr; ++i) {
            dial = dial + (sign * 1);
            if (dial >= 100)
                dial -= 100;
            if (dial < 0)
                dial += 100;
            if (dial == 0)
                ++zeros2;
        }

        if (dial == 0) {
            ++zeros1;
        }
    }

    fmt::print("1: {}\n", zeros1);
    fmt::print("2: {}\n", zeros2);

    return 0;
}
