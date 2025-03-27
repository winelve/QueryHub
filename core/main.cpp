//core核心模块的main函数


#include "BPlusTree.h"
#include <QDebug>
int main() {
    //测试数据


    bPlusTree bPlusTree;

    bPlusTree.insert({42, "apple", 3.14}, {"Red", 10, 0.99});
    bPlusTree.insert({13, "banana", 2.71}, {"Yellow", 5, 0.79});
    bPlusTree.insert({67, "cherry", 1.41}, {"Red", 20, 2.99});
    bPlusTree.insert({23, "date", 0.58}, {"Brown", 12, 1.49});
    bPlusTree.insert({89, "fig", 6.28}, {"Purple", 7, 3.19});
    bPlusTree.insert({5, "grape", 4.20}, {"Purple", 30, 2.59});
    bPlusTree.insert({55, "kiwi", 7.77}, {"Green", 15, 1.89});
    bPlusTree.insert({76, "lemon", 8.88}, {"Yellow", 25, 0.99});
    bPlusTree.insert({30, "mango", 9.99}, {"Orange", 8, 2.49});
    bPlusTree.insert({14, "nectarine", 5.55}, {"Orange", 18, 1.69});
    bPlusTree.insert({48, "orange", 3.33}, {"Orange", 22, 1.29});
    bPlusTree.insert({90, "papaya", 2.22}, {"Orange", 11, 1.99});
    bPlusTree.insert({2, "peach", 1.11}, {"Pink", 14, 2.09});
    bPlusTree.insert({19, "pear", 0.99}, {"Green", 16, 1.59});
    bPlusTree.insert({33, "pineapple", 6.66}, {"Brown", 6, 3.49});
    bPlusTree.insert({71, "plum", 7.89}, {"Purple", 9, 2.19});
    bPlusTree.insert({62, "raspberry", 4.56}, {"Red", 13, 2.79});
    bPlusTree.insert({8, "strawberry", 8.23}, {"Red", 24, 2.99});
    bPlusTree.insert({99, "tangerine", 9.01}, {"Orange", 10, 1.09});
    bPlusTree.insert({50, "watermelon", 10.10}, {"Green", 17, 4.99});


    std::vector<std::any> result1 = bPlusTree.search({42, "apple", 3.14});   // 存在
    std::vector<std::any> result2 = bPlusTree.search({13, "banana", 2.71});  // 存在
    std::vector<std::any> result3 = bPlusTree.search({76, "lemon", 8.88});   // 存在
    std::vector<std::any> result4 = bPlusTree.search({99, "tangerine", 9.01}); // 存在
    std::vector<std::any> result5 = bPlusTree.search({100, "unknown", 1.23});  // 不存在
    std::vector<std::any> result6 = bPlusTree.search({1, "not_in_tree", 5.67}); // 不存在
    std::vector<std::any> result7 = bPlusTree.search({8, "strawberry", 8.23}); // 存在
    std::vector<std::any> result8 = bPlusTree.search({23, "date", 0.58});  // 存在
    std::vector<std::any> result9 = bPlusTree.search({71, "plum", 7.89});  // 存在
    std::vector<std::any> result10 = bPlusTree.search({50, "watermelon", 10.10});  // 存在

    bPlusTree.printBPlusTree();
}

