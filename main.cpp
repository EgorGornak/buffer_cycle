#include <iostream>
#include <vector>
#include <algorithm>
#include "circular_buffer.h"

int main() {
    circular_buffer<int> c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(3);
    c.push_back(4);
    c.insert(c.begin(), 3);
    
    auto i = c.erase(std::next(c.begin()));
    std::cout << *i << '\n';
    i = c.erase(i);
    std::cout << *i << '\n';
    return 0;
}