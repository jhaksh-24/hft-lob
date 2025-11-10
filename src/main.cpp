#include "order_book.h"
#include <iostream>

int main() {
    Book book;
    
    std::cout << "=== Order Book System ===\n";
    std::cout << "Commands:\n";
    std::cout << "  1 - Add Order\n";
    std::cout << "  2 - Remove Order\n";
    std::cout << "  3 - Modify Order\n";
    std::cout << "  4 - Print Book\n";
    std::cout << "  0 - Exit\n\n";
    
    int choice;
    while (true) {
        std::cout << "> ";
        std::cin >> choice;
        
        if (choice == 0) {
            break;
        }
        else if (choice == 1) {
            int id, shares, price;
            char side;
            std::cout << "Order ID: ";
            std::cin >> id;
            std::cout << "Shares: ";
            std::cin >> shares;
            std::cout << "Price: ";
            std::cin >> price;
            std::cout << "Side (B/S): ";
            std::cin >> side;
            
            Side s = (side == 'B' || side == 'b') ? Side::BUY : Side::SELL;
            book.AddOrder(id, shares, price, s);
            std::cout << "Order added.\n";
        }
        else if (choice == 2) {
            int id;
            std::cout << "Order ID: ";
            std::cin >> id;
            book.RemoveOrder(id);
            std::cout << "Order removed.\n";
        }
        else if (choice == 3) {
            int id, shares, price;
            std::cout << "Order ID: ";
            std::cin >> id;
            std::cout << "New Shares: ";
            std::cin >> shares;
            std::cout << "New Price: ";
            std::cin >> price;
            book.ModifyOrder(id, shares, price);
            std::cout << "Order modified.\n";
        }
        else if (choice == 4) {
            book.PrintBook();
        }
    }
    
    std::cout << "Goodbye!\n";
    return 0;
}
