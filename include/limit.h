#pragma once
#include <memory>
#include "order.h"

enum class Color { RED, BLACK };

struct Limit {
    int limitPrice;
    int size;
    int totalVolume;
    Color color = RED;
    
    std::weak_ptr<Limit>   parent;
    std::shared_ptr<Limit> leftChild = nullptr;
    std::shared_ptr<Limit> rightChild = nullptr;
    std::shared_ptr<Order> headOrder = nullptr;
    std::weak_ptr<Order>   tailOrder;
};
