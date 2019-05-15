#include <tuple>

#define MACRO(...) \
    std::cout << "num args: " \
    << std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value \
    << std::endl;#pragma once
