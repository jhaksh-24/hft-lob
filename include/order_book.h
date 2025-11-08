#pragma once
#include <memory>
#include <unordered_map>
#include "limit.h"
#include "order.h"

class Book {
private:
    std::shared_ptr<Limit> buyRoot = nullptr;
    std::shared_ptr<Limit> sellRoot = nullptr;

    std::weak_ptr<Limit> highestBuy;
    std::weak_ptr<Limit> lowestSell;

    std::unordered_map<int, std::shared_ptr<Order>> orderIndex;

    Book() {
        buyRoot = std::make_shared<Limit>();
        buyRoot->color = Color::BLACK;

        sellRoot = std::make_shared<Limit>();
        sellRoot->color = Color::BLACK;
    }

    // Private helper functions
    std::shared_ptr<Limit> FindLimit(int price, Side side);
    std::shared_ptr<Limit> InsertLimit(int price, Side side);
    void RemoveLimit(std::shared_ptr<Limit> limit, Side side);

public:
    void AddOrder(int id, int shares, int price, Side side);
    void RemoveOrder(int orderId);
    void ModifyOrder(int orderId, int newShares, int newPrice);
    void MatchOrder(std::shared_ptr<Order> order);
    void ExecuteTrade(std::shared_ptr<Order> buyOrder, 
                        std::shared_ptr<Order> sellOrder, 
                        int quantity);
    void PrintBook();
};
