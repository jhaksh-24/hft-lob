#include "../include/order_book.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

void interactiveMode(Book& book) {
    int choice;
    while (true) {
        cout << "> ";
        cin >> choice;
        
        if (choice == 0) {
            break;
        }
        else if (choice == 1) {
            int id, shares, price;
            char side;
            cout << "Order ID: ";
            cin >> id;
            cout << "Shares: ";
            cin >> shares;
            cout << "Price: ";
            cin >> price;
            cout << "Side (B/S): ";
            cin >> side;
            
            Side s = (side == 'B' || side == 'b') ? Side::BUY : Side::SELL;
            book.AddOrder(id, shares, price, s);
            cout << "Order added.\n";
        }
        else if (choice == 2) {
            int id;
            cout << "Order ID: ";
            cin >> id;
            book.RemoveOrder(id);
            cout << "Order removed.\n";
        }
        else if (choice == 3) {
            int id, shares, price;
            cout << "Order ID: ";
            cin >> id;
            cout << "New Shares: ";
            cin >> shares;
            cout << "New Price: ";
            cin >> price;
            book.ModifyOrder(id, shares, price);
            cout << "Order modified.\n";
        }
        else if (choice == 4) {
            book.PrintBook();
        }
    }
}

void fileMode(Book& book, const string& filename) {
    string filepath = "../demo_files/" + filename;
    ifstream file(filepath);
    if (!file) {
        cout << "Error: Could not open file " << filename << endl;
        return;
    }
    
    string line;
    int lineNum = 0;
    
    while (getline(file, line)) {
        lineNum++;
        
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        istringstream iss(line);
        char action;
        iss >> action;
        
        if (action == 'A') {
            int id, shares, price;
            char side;
            iss >> id >> shares >> price >> side;
            
            Side s = (side == 'B' || side == 'b') ? Side::BUY : Side::SELL;
            cout << "\n[Line " << lineNum << "] Adding Order #" << id << endl;
            book.AddOrder(id, shares, price, s);
        }
        else if (action == 'R') {
            int id;
            iss >> id;
            cout << "\n[Line " << lineNum << "] Removing Order #" << id << endl;
            book.RemoveOrder(id);
        }
        else if (action == 'M') {
            int id, shares, price;
            iss >> id >> shares >> price;
            cout << "\n[Line " << lineNum << "] Modifying Order #" << id << endl;
            book.ModifyOrder(id, shares, price);
        }
        else if (action == 'P') {
            cout << "\n[Line " << lineNum << "] Printing Book:" << endl;
            book.PrintBook();
        }
        else {
            cout << "Warning: Unknown action '" << action << "' on line " << lineNum << endl;
        }
    }
    
    file.close();
    cout << "\nFile processing complete!\n";
}

int main() {
    Book book;
    
    cout << "=== Order Book System ===\n";
    cout << "1 - Interactive Mode\n";
    cout << "2 - Load from File\n";
    cout << "0 - Exit\n\n";
    
    int mode;
    cout << "Select mode: ";
    cin >> mode;
    
    if (mode == 1) {
        cout << "\nCommands:\n";
        cout << "  1 - Add Order\n";
        cout << "  2 - Remove Order\n";
        cout << "  3 - Modify Order\n";
        cout << "  4 - Print Book\n";
        cout << "  0 - Exit\n\n";
        
        interactiveMode(book);
    }
    else if (mode == 2) {
        string filename;
        cout << "Enter filename: ";
        cin >> filename;
        fileMode(book, filename);
    }
    
    cout << "Goodbye!\n";
    return 0;
}

