# HFT-LOB: Low-Latency Limit Order Book

A high-performance limit order book implementation in C++ demonstrating advanced data structures and systems programming concepts.

## Overview

This project implements a limit order book using binary search trees for price level management and doubly-linked lists for maintaining price-time priority. Built from scratch to understand market microstructure and low-latency system design fundamentals.

## Features

### ✅ Stage 1: Order Book Management (Complete)
- **Binary Search Tree** for O(log n) price level operations
- **Doubly-Linked Lists** for FIFO order priority at each price
- **Smart Pointer Management** for memory safety
- **Full Order Lifecycle**: Add, cancel, and modify orders
- **Price-Time Priority**: Orders at same price execute in time order
- **Efficient Best Bid/Ask Tracking**: Cached pointers for O(1) access

### 🚧 Stage 2: Matching Engine (Coming Soon)
- Order matching logic
- Trade execution
- Performance benchmarking

## Architecture
```
Book (Order Book)
├── Buy Tree (Binary Search Tree)
│   └── Limit Nodes (Price Levels)
│       └── Order Lists (Doubly-Linked)
└── Sell Tree (Binary Search Tree)
    └── Limit Nodes (Price Levels)
        └── Order Lists (Doubly-Linked)
```

### Data Structures

**Order**: Individual order with ID, price, quantity, side  
**Limit**: Price level node containing all orders at that price  
**Book**: Root structure managing buy/sell trees and order index

### Key Design Decisions

- **BST over hash map**: Enables ordered traversal for price discovery
- **Doubly-linked lists**: O(1) insertion/deletion with time priority
- **Smart pointers**: `shared_ptr` for ownership, `weak_ptr` to break cycles
- **Order index**: `unordered_map` for O(1) order lookup by ID

## Project Structure
```
hft-lob/
├── include/
│   ├── order.h          # Order struct definition
│   ├── limit.h          # Limit (price level) struct
│   └── order_book.h     # Book class declaration
├── src/
│   ├── order_book.cpp   # Book implementation
│   └── main.cpp         # Interactive CLI demo
├── README.md
├── LICENSE
└── .gitignore
```

## Build & Run

### Prerequisites
- C++17 or higher
- g++ compiler

### Compile
```bash
cd src
g++ main.cpp order_book.cpp -I../include -std=c++20 -o main
```

### Run
```bash
./main
```

## Usage

Interactive command-line interface:
```
=== Order Book System ===
Commands:
  1 - Add Order
  2 - Remove Order
  3 - Modify Order
  4 - Print Book
  0 - Exit

> 1
Order ID: 1
Shares: 100
Price: 99
Side (B/S): B
Order added.

> 4
=== ORDER BOOK ===
SELL SIDE:
  $102: 50 shares (2 orders)
  $103: 40 shares (1 orders)

BUY SIDE:
  $99: 100 shares (1 orders)
  $100: 125 shares (2 orders)

Best Bid: $100 (125 shares)
Best Ask: $102 (50 shares)
Spread: $2
==================
```

## Implementation Highlights

### Order Management
- **Add Order**: Creates order, finds/creates price level, maintains list ordering
- **Remove Order**: Handles edge cases (head, tail, middle, only order)
- **Modify Order**: Preserves time priority for quantity decreases, reinserts for price changes

### Tree Operations
- **Find Limit**: O(log n) binary search for price level
- **Insert Limit**: Creates new price level node, updates best bid/ask cache
- **Remove Limit**: Handles node deletion with 0, 1, or 2 children

## Roadmap

- [x] Core order book data structures
- [x] Order lifecycle management (add/cancel/modify)
- [x] Tree operations (find/insert/remove)
- [x] Interactive demo
- [ ] Order matching engine
- [ ] Trade execution and reporting
- [ ] Performance benchmarking
- [ ] Red-Black tree balancing
- [ ] Memory pooling optimization
- [ ] Lock-free concurrent access

## Technical Details

**Complexity:**
- Add Order: O(log n) for price lookup + O(1) for list insertion
- Remove Order: O(1) lookup via index + O(1) list removal
- Modify Order: O(1) for quantity decrease, O(log n) for price change
- Best Bid/Ask: O(1) via cached pointers

**Memory:**
- Smart pointers prevent leaks and dangling references
- Weak pointers break circular dependencies (parent ↔ child)

## Learning Outcomes

This project demonstrates:
- Advanced C++ (smart pointers, move semantics, templates)
- Data structure design and trade-offs
- Systems programming (memory management, performance considerations)
- Domain knowledge (market microstructure, price-time priority)
- Software engineering (modularity, separation of concerns)

## Future Enhancements

- **Stage 2**: Complete matching engine with trade execution
- **Stage 3**: Performance optimization (RB trees, memory pools)
- **Stage 4**: Market data replay and backtesting
- **Stage 5**: Multi-threaded order processing

## Author

Akshat Kumar Jha

## License

MIT License - See LICENSE file for details

---

**Status**: Stage 1 Complete | Stage 2 In Progress
