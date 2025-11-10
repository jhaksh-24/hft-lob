#include "order_book.h"
#include <iostream>
#include <memory>

//==============================================================================
// TREE OPERATIONS
//==============================================================================

/*
 * FindLimit - Search for a price level in the appropriate tree
 */

std::shared_ptr<Limit> Book::FindLimit(int price, Side side) {
    std::shared_ptr<Limit> searchPtr = (side == Side::BUY ? buyRoot : sellRoot);

    while (searchPtr != nullptr) {
        if (searchPtr -> limitPrice > price) {
            searchPtr = searchPtr -> leftChild;
        }
        else if (searchPtr -> limitPrice < price) {
            searchPtr = searchPtr -> rightChild;
        }
        else {
            return searchPtr;
        }
    } 
    return nullptr;
}

/*
 * InsertLimit - Create and insert a new price level into the tree
 */

std::shared_ptr<Limit> Book::InsertLimit(int price, Side side) {

    // Currently using unbalanced tree... will correct it to use red black preferably

    std::shared_ptr<Limit>& root = (side == Side::BUY ? buyRoot : sellRoot);

    auto newLimit = std::make_shared<Limit>();
    newLimit -> limitPrice = price;
    newLimit -> size = 0;
    newLimit -> totalVolume = 0;

    if (root == nullptr) {
        root = newLimit;
        return newLimit;
    }

    std::shared_ptr<Limit> current = root;
    std::shared_ptr<Limit> parent = nullptr;

    while (current != nullptr) {
        if (current -> limitPrice > price) {
            parent = current;
            current = current -> rightChild;
        }

        else if (current -> limitPrice < price) {
            parent = current;
            current = current -> leftChild;
        }

        else {
            return current;
        }
    }

    newLimit -> parent = parent;
    (price < parent -> limitPrice ? parent ->leftChild = newLimit : parent -> rightChild = newLimit);

    if (side == Side::BUY && (highestBuy.expired() || price > highestBuy.lock() -> limitPrice)) {
        highestBuy = newLimit;
    }

    if(side == Side::SELL && (lowestSell.expired() || price < lowestSell.lock() -> limitPrice)) {
        lowestSell = newLimit;
    }
    return newLimit;
}

/*
 * RemoveLimit - Delete an empty price level from the tree
 */
void Book::RemoveLimit(std::shared_ptr<Limit> limit, Side side) {
    // Removing root will be addressed
    auto parentlimit = limit -> parent.lock();

    if (limit -> leftChild == nullptr && limit -> rightChild == nullptr) {
        if (parentlimit -> leftChild == limit) {
            parentlimit -> leftChild = nullptr;
        }
        else {
            parentlimit -> rightChild = nullptr;
        }
    }
    else if (limit -> leftChild == nullptr) {
        if (parentlimit -> leftChild == limit) {
            parentlimit -> leftChild = limit -> rightChild;
        }
        else {
            parentlimit -> rightChild = limit -> rightChild;
        }

        if (limit -> rightChild)
            limit -> rightChild -> parent = parentlimit;
    }
    else if (limit -> rightChild == nullptr) {
        if (parentlimit -> leftChild == limit) {
            parentlimit -> leftChild = limit -> leftChild;
        }
        else {
            parentlimit -> rightChild = limit -> leftChild;
        }

        if (limit -> leftChild)
            limit -> leftChild -> parent = parentlimit;
    }
    else {
        auto successor = limit -> rightChild;
        while (successor -> leftChild != nullptr) {
            successor = successor -> leftChild;
        }

        limit -> limitPrice = successor -> limitPrice;
        limit -> size = successor -> size;
        limit -> totalVolume = successor -> totalVolume;
        limit -> headOrder = successor -> headOrder;
        limit -> tailOrder = successor -> tailOrder;

        RemoveLimit(successor, side);
        return;
    }

    if (side == Side::SELL && lowestSell.lock() == limit) {
        if (sellRoot == nullptr)
            lowestSell.reset();

        else {
            auto it = sellRoot;
            while (it -> leftChild != nullptr) {
                it = it -> leftChild;
            }
            lowestSell = it;
        }
    }

    if (side == Side::BUY && highestBuy.lock() == limit) {
        if (buyRoot == nullptr)
            highestBuy.reset();
        else{
            auto it = buyRoot;
            while (it -> rightChild != nullptr) {
                it = it -> rightChild;
            }
            highestBuy = it;
        }
    }
}

//==============================================================================
// ORDER OPERATIONS
//==============================================================================

/*
 * AddOrder - Add a new order to the book
 */
void Book::AddOrder(int id, int shares, int price, Side side) {

    auto newOrder = std::make_shared<Order>();
    newOrder -> id = id;
    newOrder -> shares = shares;
    newOrder -> price = price;
    newOrder -> side = side;
    newOrder -> entryTime = 0;
    newOrder -> eventTime = 0;

    auto limit = FindLimit(price, side);
    if (limit == nullptr) {
        limit = InsertLimit(price, side);
    }

    orderIndex[id] = newOrder;
    newOrder -> parentLimit = limit;

    if (limit -> headOrder == nullptr) {
        limit -> headOrder = newOrder;
        limit -> tailOrder = newOrder;
    }
    else {
        auto oldTail = limit->tailOrder.lock();
        if (oldTail) {
            oldTail->nextOrder = newOrder;
            newOrder->prevOrder = oldTail;
        }
        limit -> tailOrder = newOrder;
    }

    limit -> size++;
    limit -> totalVolume += shares;
}

/*
 * RemoveOrder - Cancel an existing order
 */
void Book::RemoveOrder(int orderId) {
    auto it = orderIndex.find(orderId);
    if (it == orderIndex.end()) return;
    auto order = it -> second;

    auto limit = order -> parentLimit.lock();
    if (!limit) return;
    auto side = order -> side;

    if (limit -> tailOrder.lock() == order && limit -> headOrder == order) {
        limit -> headOrder = nullptr;
        limit -> tailOrder.reset();
    }
    else if (limit -> headOrder == order) {
        limit -> headOrder = order -> nextOrder;
        if (order -> nextOrder) {
            order -> nextOrder->prevOrder.reset();
        }
    }
    else if (limit -> tailOrder.lock() == order) {
        auto prev = order -> prevOrder.lock();
        if (prev) {
            prev -> nextOrder = nullptr;
        }
        limit -> tailOrder = order -> prevOrder;
    }    
    else {
        auto prev = order -> prevOrder.lock();
        auto next = order -> nextOrder;

        if (prev) {
            prev -> nextOrder = next;
        }
        if (next) {
            next -> prevOrder = order -> prevOrder;
        }
    }
    limit -> size--;
    limit -> totalVolume -= order -> shares;


    if (limit -> size == 0) {
        RemoveLimit(limit, side);
    }

    orderIndex.erase(orderId);

    order->parentLimit.reset();
    order->prevOrder.reset();
    order->nextOrder = nullptr;
}
/*
 * ModifyOrder - Modify an existing order's quantity or price
 * 
 * @param orderId   The ID of the order to modify
 * @param newShares New quantity (must be > 0)
 * @param newPrice  New limit price
 */
void Book::ModifyOrder(int orderId, int newShares, int newPrice) {
    auto it = orderIndex.find(orderId);
    if (it == orderIndex.end()) return;
    auto order = it -> second;

    int oldPrice = order -> price;
    int oldShares = order -> shares;
    Side oldSide = order -> side

    if (newPrice != oldPrice || newShares > oldShares) {
        RemoveOrder(orderId);
        AddOrder(orderId, newShares, newPrice, oldSide);
    }
    else if (newShares < oldShares) {
        order -> shares = newShares;
        auto limit = order->parentLimit.lock();
        if (limit) {
            limit->totalVolume -= (oldShares - newShares);
        }
    }
}

//==============================================================================
// MATCHING ENGINE
//==============================================================================

/*
 * MatchOrder - Attempt to match an order against the opposite side
 * 
 * @param order The incoming order to match
 */
void Book::MatchOrder(std::shared_ptr<Order> order) {
    // TODO: Implement match order
}

/*
 * ExecuteTrade - Execute a trade between two orders
 * 
 * @param buyOrder  The buy side order
 * @param sellOrder The sell side order
 * @param quantity  Number of shares to trade
 */
void Book::ExecuteTrade(std::shared_ptr<Order> buyOrder, 
                        std::shared_ptr<Order> sellOrder, 
                        int quantity) {
    // TODO: Implement trade execution
}

//==============================================================================
// UTILITY FUNCTIONS
//==============================================================================

/*
 * PrintBook - Debug function to visualize current order book state
 */
void Book::PrintBook() {
    // TODO: Implement book visualization
}
