//
// Created by @kalwalt on 13/03/2024.
//

#define FMT_HEADER_ONLY

#include <fmt/color.h>

int main ()
{
    std::string s = "Abc\n";
    fmt::print(bg(fmt::color::red), s);
    return 0;
}
