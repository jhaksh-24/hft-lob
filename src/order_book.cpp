#include<memory>
#include<chrono>

enum class Side {BUY, SELL};

struct Order {
    int  id;
    int  shares;
    int  price;
    Side side;

    auto entryTime; // Type to be corrected
    auto eventTime; // Type to be corrected

    std::weak_ptr<Limit>   parentLimit;
    std::shared_ptr<Order> nextOrder = nullptr;
    std::weak_ptr<Order>   prevOrder;
};

struct Limit {
    int limitPrice;
    int size;
    int totalVolume;

    std::weak_ptr<Limit>   parent;
    std::shared_ptr<Limit> leftChild = nullptr;
    std::shared_ptr<Limit> rightChild = nullptr;
    std::shared_ptr<Order> headOrder = nullptr;
    std::shared_ptr<Order> tailOrder;
};

class Book {
    std::unique_ptr<Limit> buyRoot;
    std::unique_ptr<Limit> sellRoot;
    
    std::weak_ptr<Limit> highestBuy;
    std::weak_ptr<Limit> lowestSell;

    void AddOrder (Order *order) {}

    void RemoveOrder (Order *order) {}

    void ExecuteOrder (Order *order) {}
}
