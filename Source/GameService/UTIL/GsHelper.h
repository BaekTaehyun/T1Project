#pragma once

#include <tuple>

#define MACRO(...) \
    std::cout << "num args: " \
    << std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value \
    << std::endl;


// quotation���� �����ش�. ��, ���ڿ��� ����� ��
#define _QUOTE(X) #X
#define QUOTE(X) _QUOTE(X)

// pragma message �� ����ؼ� ������ Ÿ�ӿ� todo�� �˷���
#define todo(MSG) message(__FILE__ "(" QUOTE(__LINE__) ") : todo => " MSG) 