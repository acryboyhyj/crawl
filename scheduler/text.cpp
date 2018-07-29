#include <stdio.h>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;
std::vector<vector<int>> SpilitTask(int client_count, int url_count) {
    std::vector<int> vec{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<vector<int>> result{};
    int every_url_count = url_count / client_count + 1;
    int count           = 0;
    std::vector<int> temp{};
    for (size_t i = 0; i < vec.size(); i++) {
        temp.push_back(vec[i]);
        count++;
        if (count == every_url_count) {
            result.push_back(temp);
            temp.clear();
            count = 0;
        }
    }
    result.push_back(temp);
    return result;
}

int main() {
    vector<vector<int>> task = SpilitTask(3, 10);
    for (const auto& a : task) {
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cout << std::endl;
    }
    return 0;
}
