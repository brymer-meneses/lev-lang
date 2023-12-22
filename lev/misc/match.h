#pragma once


template<class... Ts>
struct match : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
match(Ts...) -> match<Ts...>;
