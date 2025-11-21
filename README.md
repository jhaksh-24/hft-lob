# HFT-LOB: Low-Latency Limit Order Book 

A high-performance limit order book implementation in C++ demonstrating advanced data structures and systems programming concepts.

## Overview

This project implements a limit order book using binary search trees for price level management and doubly-linked lists for maintaining price-time priority. Built from scratch to understand market microstructure and low-latency system design fundamentals.

## Features

### ✅ Stage 1: Order Book Management (Complete)
- **Binary Search Tree** for O(log n) price level operations
- **Doubly-Linked Lists** for FIFO order priority at each price
- **Smart Pointer Management** for memory safety (shared_ptr, weak_ptr)
- **Full Order Lifecycle**: Add, cancel, and modify orders
- **Price-Time Priority**: Orders at same price execute in time order
- **Efficient Best Bid/Ask Tracking**: Cached pointers for O(1) access

### ✅ Stage 2: Matching Engine (Complete)
- **Automatic Order Matching**: Incoming orders match against opposite side
- **Price-Time Priority Execution**: FIFO matching at each price level
- **Partial Fill Support**: Orders can be partially matched
- **Trade Execution**: Real-time trade reporting with detailed logging
- **Multi-level Matching**: Orders can match across multiple price levels

### 🚧 Stage 3: Future Enhancements
- Performance benchmarking and optimization
- Red-Black tree balancing
- Database persistence
- Market data replay

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
- **Aggressive matching first**: Incoming orders match before resting in book

## Project Structure
```
hft-lob/
├── include/
│   ├── order.h          # Order struct definition
│   ├── limit.h          # Limit (price level) struct
│   └── order_book.h     # Book class declaration
├── src/
│   ├── order_book.cpp   # Book implementation
│   └── main.cpp         # Interactive CLI and file mode
├── demo_files/          # Pre-made test scenarios
│   └── basic_demo.txt
├── README.md
├── LICENSE
└── .gitignore
```

## Build & Run

### Prerequisites
- C++17 or higher
- CMake 3.15+ (recommended) or g++ compiler

### Option 1: CMake (Recommended)
```bash
# Create build directory
mkdir build && cd build

# Configure (Debug mode)
cmake ..

# Or configure (Release mode - optimized)
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build
make

# Run
./orderbook
```

### Option 2: Direct Compilation
```bash
cd src
g++ main.cpp order_book.cpp -I../include -std=c++20 -o main
./main
```

## Usage

### Interactive Mode

Real-time order entry with immediate matching feedback:
```
=== Order Book System ===
1 - Interactive Mode
2 - Load from File
0 - Exit

Select mode: 1

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
BUY SIDE:
  $99: 100 shares (1 orders)

Best Bid: $99 (100 shares)
Best Ask: N/A
==================
```

### File Mode

Load and execute pre-made test scenarios:
```bash
./main
> Select mode: 2
> Enter filename: crossing_orders.txt
```

#### Demo Files

**`basic_demo.txt`** - Simple order addition and book visualization  

#### File Format:**
```
# Comments start with #
# Format: Action ID Shares Price Side
# Actions: A=Add, R=Remove, M=Modify, P=Print

A 1 100 99 B      # Add buy order
A 2 50 101 S      # Add sell order
P                 # Print book
```

**Note:** When using CMake, demo files are automatically copied to the build directory.

## Matching Engine Example
```
> 1
Order ID: 3
Shares: 50
Price: 105
Side (B/S): B

>>> Matching Order #3: BUY 50 shares @ $105
  → Checking opposite side...
  → Found SELL @ $102 (30 shares)
  → Matching 30 shares...

TRADE: 30 shares @ $102
  Buyer  : Order #3
  Seller : Order #1

  → 20 shares remaining
  → Checking opposite side...
  → Found SELL @ $103 (20 shares)
  → Matching 20 shares...

TRADE: 20 shares @ $103
  Buyer  : Order #3
  Seller : Order #2

  → Order fully filled!

Order added.
```

## Implementation Highlights

### Order Management
- **Add Order**: Creates order, attempts matching first, adds unfilled portion to book
- **Remove Order**: Handles edge cases (head, tail, middle, only order), updates parent limit
- **Modify Order**: Preserves time priority for quantity decreases, reinserts for price changes

### Tree Operations
- **Find Limit**: O(log n) binary search for price level
- **Insert Limit**: Creates new price level node, updates best bid/ask cache
- **Remove Limit**: Handles node deletion with 0, 1, or 2 children; includes root removal

### Matching Engine
- **Price-Time Priority**: Matches from head of each price level (FIFO)
- **Multi-level Matching**: Continues matching across price levels until order filled
- **Aggressive Orders**: Incoming orders match against resting orders first
- **Trade Execution**: Updates quantities, removes filled orders, maintains book integrity

## Technical Details

**Complexity:**
- Add Order: O(log n) for price lookup + O(1) for list insertion + O(k) for k matches
- Remove Order: O(1) lookup via index + O(1) list removal + O(log n) if limit removed
- Modify Order: O(1) for quantity decrease, O(log n) for price change
- Best Bid/Ask: O(1) via cached pointers
- Match Order: O(k * log n) for k matches across price levels

**Memory:**
- Smart pointers prevent leaks and dangling references
- Weak pointers break circular dependencies (parent ↔ child, order ↔ limit)
- Order index enables O(1) lookup by ID

## Learning Outcomes

This project demonstrates:
- Advanced C++ (smart pointers, references, standard library containers)
- Data structure design and trade-offs (trees vs hash maps, lists vs arrays)
- Algorithm implementation (tree operations, matching logic)
- Systems programming (memory management, performance considerations)
- Domain knowledge (market microstructure, price-time priority, order types)
- Software engineering (modularity, separation of concerns, file I/O)

## Roadmap

- [x] Core order book data structures
- [x] Order lifecycle management (add/cancel/modify)
- [x] Tree operations (find/insert/remove including root)
- [x] Order matching engine with price-time priority
- [x] Trade execution and reporting
- [x] Interactive demo mode
- [x] File-based test scenarios
- [x] CMake build system
- [ ] Performance benchmarking (latency p50/p99, throughput)
- [ ] Red-Black tree balancing for guaranteed O(log n)
- [ ] Memory pooling optimization
- [ ] Database persistence (order history, trade log)
- [ ] Market data replay with real exchange data
- [ ] Lock-free concurrent access

## Known Limitations

- Uses unbalanced BST (can degrade to O(n) in worst case)
- No order validation (negative prices, zero quantities)
- Single-threaded (no concurrent access support)
- In-memory only (no persistence between runs)
- Color field in Limit struct (for future RB tree implementation) currently unused

## Future Enhancements

- **Stage 3**: Performance optimization (RB trees, memory pools, benchmarking)
- **Stage 4**: Persistence layer (database integration, trade logging)
- **Stage 5**: Market data integration (replay historical feeds, backtesting)
- **Stage 6**: Advanced features (order types, auction mechanisms, circuit breakers)

## Author

Akshat Kumar Jha

## License

MIT License - See LICENSE file for details

---

**Status**: Stage 2 Complete | Matching Engine Operational ✅
