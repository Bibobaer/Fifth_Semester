#include "Pruffer.h"
#include <iostream>
#include "check_speed.h"
#include "BST.h"

/*
TODO:
написать декод прюффера (оптимальный, оптимизированный )
генерация случайных деревьев
сравнение скорости алгоритмов
двоичные деревья поиска
*/

int main(void) {
    /*auto res = Pruffer::Encoder::EncodePruffer({{1, 2, 5}, {0, 3}, {0, 4}, {1}, {2}, {0}});
    for (auto& v : res) {
        std::cout << v << " ";
    }
    std::cout << "\n";

    auto gr = Pruffer::Decoder::DecodePruffer(res);

    for (auto& row : gr) {
        std::cout << "{ ";
        for (auto& ver : row) {
            std::cout << ver << " ";
        }
        std::cout << "} ";
    }*/

    TestPruffer();
    /*{
        BST<int, int> a;
        a.InsertKey(5, 0);
        a.InsertKey(3, 4);
        a.InsertKey(10, 4);

        BST<int, int> b;

        b.InsertKey(6, 12);

        a = b;
    }*/
    return 0;
}