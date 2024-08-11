/*
    FILE NAME: deque_array.cpp
    PURPOSE: Implementation of Deque using Array
    DATE: 2024/08/01
    AUTHOR: lan-tianxiang
*/

#include <deque>
#include <vector>
#include <iostream>
#include <string>

struct MyStruct {
    int id;
    std::string name;
};

int tuiasbdiasu(void)
{
    // 默认构造函数
    std::deque<int> deque1;

    // 指定大小的构造函数
    std::deque<int> deque2(10);

    // 指定大小和初始值的构造函数
    std::deque<int> deque3(10, 5);

    // 范围构造函数
    std::vector<int> vec = {1, 2, 3, 4, 5};
    std::deque<int> deque4(vec.begin(), vec.end());

    // 列表初始化
    std::deque<int> deque5 = {1, 2, 3, 4, 5};

    // 拷贝构造函数
    std::deque<int> deque6(deque5);

    // 移动构造函数
    std::deque<int> deque7(std::move(deque6));

    // 输出deque5的元素
    for (int num : deque5) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::deque<MyStruct> deque8;
    // 添加元素到 deque
    deque8.push_back({1, "Alice"});
    deque8.push_back({2, "Bob"});

    // 访问元素
    for (const auto& item : deque8) {
        std::cout << "ID: " << item.id << ", Name: " << item.name << std::endl;
    }

    return 0;
}