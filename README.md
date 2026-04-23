# Arc — Atomic Reference Counting for C++

**Arc** is a header-only C++ library that provides thread-safe reference-counted smart pointers, inspired by Rust's `Arc<T>`.

## Features

- **Thread-safe reference counting** via C++ atomics
- **`Arc<T>`** — Shared ownership smart pointer with automatic cleanup
- **`Arc<void>`** — Type-erased variant for dynamic typing
- **`Weak<T>`** — Non-owning references to prevent circular dependencies
- **Header-only** — No linking required, just include and use

## Installation

```bash
cmake -B build
cmake --build build
```

Or copy the `include/arc/` directory into your project.

## Quick Start

```cpp
#include <arc/Arc.hpp>

// Create a shared pointer
auto arc = Arc<int>::make(42);

// Clone it (reference count increases)
auto arc2 = arc;

// Access the value
int val = *arc;

// Use with lambdas
arc.map([](int& n) { n *= 2; });
```

## Project Structure

```
include/arc/
├── Arc.hpp        # Main Arc<T> implementation
├── ArcVoid.hpp    # Type-erased Arc<void>
├── SharedState.hpp # Reference counting internals
├── Weak.hpp       # Weak<T> reference implementation
└── Traits.hpp     # Type traits

src/
└── Arc.cpp        # Empty, library is header-only

test/src/
├── ArcSpec.cpp    # Arc tests
└── WeakSpec.cpp   # Weak reference tests
```

## Building Tests

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

Run a specific test:
```bash
ctest --test-dir build -R ArcShouldBeAbleToCreate
```

## Requirements

- C++23
- CMake 3.29+
- Google Test (fetched automatically)

## License

MIT
