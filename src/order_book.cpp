#include "../include/order_book.h"
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
    auto parentlimit = limit -> parent.lock();

    if(!parentlimit) {
        std::shared_ptr<Limit> & root = (side == Side::BUY ? buyRoot : sellRoot);

        if (!limit -> leftChild && !limit -> rightChild) {
            root = nullptr;
        }
        else if (!limit->leftChild) {
            root = limit->rightChild;
            root->parent.reset();
        }
        else if (!limit->rightChild) {
            root = limit->leftChild;
            root->parent.reset();
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

        if (side == Side::SELL) {
            if (sellRoot) {
                auto it = sellRoot;
                while (it->leftChild) it = it->leftChild;
                lowestSell = it;
            }
            else {
                lowestSell.reset();
            }
        }
        if (side == Side::BUY) {
            if (buyRoot) {
                auto it = buyRoot;
                while (it->rightChild) it = it->rightChild;
                highestBuy = it;
            }
            else {
                highestBuy.reset();
            }
        }

        return;

    }

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

    orderIndex[id] = newOrder;

    MatchOrder(newOrder);

    if (newOrder -> shares > 0) {

        auto limit = FindLimit(price, side);
        if (limit == nullptr) {
            limit = InsertLimit(price, side);
        }
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
        limit -> totalVolume += newOrder -> shares;
    }
    else {
        orderIndex.erase(id);
    }
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
 */
void Book::ModifyOrder(int orderId, int newShares, int newPrice) {
    auto it = orderIndex.find(orderId);
    if (it == orderIndex.end()) return;
    auto order = it -> second;

    int oldPrice = order -> price;
    int oldShares = order -> shares;
    Side oldSide = order -> side;

    if (newPrice != oldPrice || newShares > oldShares) {
        RemoveOrder(orderId);
        AddOrder(orderId, newShares, newPrice, oldSide);
    }
    if (newShares < oldShares) {
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
 */
void Book::MatchOrder(std::shared_ptr<Order> order) {

    std::cout << "\n>>> Matching Order #" << order->id << ": "
        << (order->side == Side::BUY ? "BUY" : "SELL") << " "
        << order->shares << " shares @ $" << order->price << std::endl;


    while (order -> shares > 0) {
        std::cout << "  → Checking opposite side..." << std::endl;
        auto oppositeLimit = (order->side == Side::BUY ? lowestSell.lock() : highestBuy.lock());

        if (!oppositeLimit || oppositeLimit->limitPrice == 0 || !oppositeLimit->headOrder) {
            std::cout << "  → No opposite orders available" << std::endl;
            break;
        }

        std::cout << "  → Found " << (order->side == Side::BUY ? "SELL" : "BUY")
            << " @ $" << oppositeLimit->limitPrice
            << " (" << oppositeLimit->totalVolume << " shares)" << std::endl;

        if (order -> side == Side::BUY) {
            if (order -> price >= oppositeLimit -> limitPrice) {
                auto restingOrder = oppositeLimit -> headOrder;
                int tradeQty = std::min(order -> shares, restingOrder -> shares);
                std::cout << "  → Matching " << tradeQty << " shares..." << std::endl;
                ExecuteTrade(order, restingOrder, tradeQty);
                if (order->shares > 0) {
                    std::cout << "  → " << order->shares << " shares remaining" << std::endl;
                }
                else {
                    std::cout << "  → Order fully filled!" << std::endl;
                }
            }
            else {
                std::cout << "  → Prices don't cross" << std::endl;
                break;
            }        
        }
        else {
            if (order -> price <= oppositeLimit -> limitPrice) {
                auto restingOrder = oppositeLimit -> headOrder;
                int tradeQty = std::min(order -> shares, restingOrder -> shares);
                ExecuteTrade(restingOrder, order, tradeQty);
                if (order->shares > 0) {
                    std::cout << "  → " << order->shares << " shares remaining" << std::endl;
                } else {
                    std::cout << "  → Order fully filled! ✓" << std::endl;
                }
            }
            else {
                std::cout << "  → Prices don't cross" << std::endl;  // Add this
                break;
            }
        }
    }

    if (order->shares > 0) {
        std::cout << "  → Adding " << order->shares << " shares to book\n" << std::endl;
    }
    std::cout << std::endl;
}

/*
 * ExecuteTrade - Execute a trade between two orders
 */
void Book::ExecuteTrade(std::shared_ptr<Order> buyOrder, 
                        std::shared_ptr<Order> sellOrder, 
                        int quantity) {
    
    std::cout << "TRADE: " << quantity << " shares @ $" << sellOrder -> price << std::endl;
    std::cout << "  Buyer  : Order #" << buyOrder -> id << std::endl;
    std::cout << "  Seller : Order #" << sellOrder -> id << std::endl;

    buyOrder -> shares -= quantity;
    sellOrder -> shares -= quantity;

    auto buyLimit = buyOrder -> parentLimit.lock();
    auto sellLimit = sellOrder -> parentLimit.lock();

    if (buyLimit) buyLimit -> totalVolume -= quantity;
    if (sellLimit) sellLimit -> totalVolume -= quantity;

    if (buyOrder -> shares == 0) RemoveOrder(buyOrder -> id);
    if (sellOrder -> shares == 0) RemoveOrder(sellOrder -> id);
}

//==============================================================================
// UTILITY FUNCTIONS
//==============================================================================

/*
 * PrintBook - Debug function to visualize current order book state
 */
void Book::PrintBook() {
    std::cout << "=== ORDER BOOK ===" << std::endl;

    std::cout << "SELL SIDE:" << std::endl;
    PrintSide(sellRoot);
    std::cout << std::endl;

    std::cout << "BUY SIDE:" << std::endl;
    PrintSide(buyRoot);
    std::cout << std::endl;

    auto bestBid = highestBuy.lock();
    auto bestAsk = lowestSell.lock();

    if (bestBid && bestAsk) {
        std::cout << "Best Bid: $" << bestBid->limitPrice
                  << " (" << bestBid->totalVolume << " shares)" << std::endl;
        std::cout << "Best Ask: $" << bestAsk->limitPrice
                  << " (" << bestAsk->totalVolume << " shares)" << std::endl;
        std::cout << "Spread: $" << bestAsk->limitPrice - bestBid->limitPrice << std::endl;
    } else {
        std::cout << "Best Bid/Ask not available (book side empty)." << std::endl;
    }

    std::cout << "==================" << std::endl;
}


/*
 * PrintSide - Helper function for PrintBook()
 */
void Book::PrintSide(std::shared_ptr<Limit> node) {
    if (!node) return;

    PrintSide(node->leftChild);
    std::cout << "  $" << node->limitPrice << ": "
              << node->totalVolume << " shares ("
              << node->size << " orders)" << std::endl;
    PrintSide(node->rightChild);
}
