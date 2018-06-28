#include <iostream>
#include "circular_buffer.h"

int main()
{
    circular_buffer <int> c;
    for (int i = 1; i <= 5; i++) {
        c.push_back(i);
    }

    for (int i = 6; i != 100; ++i)
    {
        c.push_back(i);
        c.pop_front();
    }

    auto it = c.begin();
    std::cout << *it << ' ';
    it++;
    std::cout << *it << ' ';
    it++;
    std::cout << *it << ' ';
    it++;
    std::cout << *it << ' ';
    it++;
    std::cout << *it << ' ';
    it++;
    std::cout << (it == c.end());

    //expect_eq(c, {95, 96, 97, 98, 99});
    return 0;
}
