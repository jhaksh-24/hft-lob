# Low-Latency Limit Order Book

A from-scratch matching engine in C++ that prioritizes **correctness → clean design → performance**.

## Goal
Build a production-grade order book demonstrating:
- Market microstructure fundamentals (price-time priority, order types)
- High-performance C++ (cache-friendly data structures, lock-free queues, memory pooling)
- Real-world testing with historical market data

## Milestones
- **M0**: Project setup & build system ← you are here
- **M1**: Core order book (insert orders, maintain best bid/ask)
- **M2**: Order matching engine (price-time priority)
- **M3**: Order management (cancel, modify, fast lookup by ID)
- **M4**: Performance optimization (lock-free pipeline, memory pooling)
- **M5**: Backtesting & benchmarks (latency p50/p99, throughput)

## Tech Stack
- C++17/20
- CMake build system
- Historical data: LOBSTER/crypto exchange feeds
- Benchmarking: Google Benchmark (or custom)

## Build
Coming soon - check back after M0!
