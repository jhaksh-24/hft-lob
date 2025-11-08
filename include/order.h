#pragma once
#include <memory>
#include <cstdint>

enum class Side { BUY, SELL };

struct Limit;  // forward declaration

struct Order {
    int  id;
    int  shares;
    int  price;
    Side side;
    uint64_t entryTime;
    uint64_t eventTime;
    
    std::weak_ptr<Limit>   parentLimit;
    std::shared_ptr<Order> nextOrder = nullptr;
    std::weak_ptr<Order>   prevOrder;
};
