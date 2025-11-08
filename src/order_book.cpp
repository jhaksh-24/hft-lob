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
    std::shared_ptr<Limit> searchPtr = (side == BUY ? buyRoot : sellRoot);

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
    // TODO: Implement tree insertion
    // Verifing with FindLimit can take time so we gonna check in real time
    std::shared_ptr<Limit> current = (side == BUY ? buyRoot : sellRoot);
    std::shared_ptr<Limit> next;
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
    // TODO: Implement order addition
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
    // TODO: Implement matching logic
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
