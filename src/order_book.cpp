#include "order_book.h"
#include <iostream>
#include <memory>

//==============================================================================
// TREE OPERATIONS
//==============================================================================

/*
 * FindLimit - Search for a price level in the appropriate tree
 * 
 * @param price The limit price to search for
 * @param side  BUY or SELL (determines which tree to search)
 * @return Shared pointer to Limit if found, nullptr otherwise
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
 * 
 * @param price The limit price for the new node
 * @param side  BUY or SELL (determines which tree to insert into)
 * @return Shared pointer to the newly created Limit
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
 * 
 * @param limit The Limit node to remove
 * @param side  BUY or SELL (determines which tree)
 */
void Book::RemoveLimit(std::shared_ptr<Limit> limit, Side side) {
    // TODO: Implement tree deletion
}

//==============================================================================
// ORDER OPERATIONS
//==============================================================================

/*
 * AddOrder - Add a new order to the book
 * 
 * Creates order, finds or creates appropriate Limit, adds to list,
 * updates index, and attempts matching.
 * 
 * @param id     Unique order identifier
 * @param shares Number of shares
 * @param price  Limit price
 * @param side   BUY or SELL
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
 * 
 * @param orderId The ID of the order to remove
 */
void Book::RemoveOrder(int orderId) {
    // TODO: Implement order removal
}

/*
 * ModifyOrder - Modify an existing order's quantity or price
 * 
 * @param orderId   The ID of the order to modify
 * @param newShares New quantity (must be > 0)
 * @param newPrice  New limit price
 */
void Book::ModifyOrder(int orderId, int newShares, int newPrice) {
    // TODO: Implement order modification
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
    while (order -> shares != 0)
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
