#pragma once
#include <vector>

namespace Pruffer {

using adjacency_list = std::vector<std::vector<int>>;

class Encoder {
 public:
    static std::vector<int> EncodePruffer(const adjacency_list& graph);
    static std::vector<int> EncodePrufferPQ(const adjacency_list& graph);
};

class Decoder {
 public:
    static adjacency_list DecodePruffer(const std::vector<int>& code);
    static adjacency_list DecodePrufferPQ(const std::vector<int>& code);
};

}