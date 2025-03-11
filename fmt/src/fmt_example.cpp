//
// Created by @kalwalt on 13/03/2024.
//

#define FMT_HEADER_ONLY

#include <fmt/color.h>
#include <fmt/format.h>

int main ()
{
    std::string s = "Abc";
    fmt::print(fmt::format(fg(fmt::color::white) | bg(fmt::color::red), s));
    fmt::print("\n"); // Add a newline character
    return 0;
}
