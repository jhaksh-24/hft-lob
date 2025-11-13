#pragma once
#include <memory>
#include <unordered_map>
#include "limit.h"
#include "order.h"

class Book {
public:
    std::shared_ptr<Limit> buyRoot = nullptr;
    std::shared_ptr<Limit> sellRoot = nullptr;

    std::weak_ptr<Limit> highestBuy;
    std::weak_ptr<Limit> lowestSell;

    std::unordered_map<int, std::shared_ptr<Order>> orderIndex;


    // Private helper functions
    std::shared_ptr<Limit> FindLimit(int price, Side side);
    std::shared_ptr<Limit> InsertLimit(int price, Side side);
    void RemoveLimit(std::shared_ptr<Limit> limit, Side side);


    void AddOrder(int id, int shares, int price, Side side);
    void RemoveOrder(int orderId);
    void ModifyOrder(int orderId, int newShares, int newPrice);
    void MatchOrder(std::shared_ptr<Order> order);
    void ExecuteTrade(std::shared_ptr<Order> buyOrder, 
                      std::shared_ptr<Order> sellOrder, 
                      int quantity);
    void PrintBook();
    void PrintSide(std::shared_ptr<Limit> node);
};
