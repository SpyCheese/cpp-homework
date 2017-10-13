#include <iostream>
#include <string>
#include "persistent_set.h"
#include "linked_ptr.h"
#include <cstdlib>
#include <set>
#include <vector>

template < typename Set >
std::vector < std::vector < int > > test(std::string const& msg) {
    int tt = clock();
    srand(1);
    std::vector < Set > ss(256);
    for (int i = 0; i < 1000000; ++i) {
        int r = rand() & 7;
        if (r == 0) {
            ss[rand() & 255] = ss[rand() & 255];
        } else if (r <= 5) {
            ss[rand() & 255].insert(rand() & 16383);
        } else {
            int j = rand() & 255;
            auto it = ss[j].find(rand() & 16383);
            if (it != ss[j].end())
                ss[j].erase(it);
        }
    }
    std::vector < std::vector < int > > a(256);
    for (int i = 0; i < 256; ++i)
        for (int x : ss[i])
            a[i].push_back(x);
    std::cout << msg << " " << 1.*(clock() - tt) / CLOCKS_PER_SEC << "\n";
    return a;
}

int main() {
    auto r1 = test<persistent_set<int, shared_ptr>>("shared_ptr");
    auto r2 = test<persistent_set<int, linked_ptr>>("linked_ptr");
    auto r3 = test<std::set<int>>("std::set  ");
    assert(r1 == r2);
    assert(r2 == r3);

    return 0;
}
