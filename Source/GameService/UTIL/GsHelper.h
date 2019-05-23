#pragma once

#include <tuple>

#define MACRO(...) \
    std::cout << "num args: " \
    << std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value \
    << std::endl;


// quotation으로 감싸준다. 즉, 문자열로 만들어 줌
#define _QUOTE(X) #X
#define QUOTE(X) _QUOTE(X)

// pragma message 를 사용해서 컴파일 타임에 todo를 알려줌
#define todo(MSG) message(__FILE__ "(" QUOTE(__LINE__) ") : todo => " MSG) 