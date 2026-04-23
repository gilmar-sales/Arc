# Arc — Atomic Reference-Counted Smart Pointer

Single-header library (`include/Arc.hpp`) implementing a thread-safe `Arc<T>` smart pointer with writer-preference RWLock. C++23.

## Build & Test

```bash
cmake -B build
cmake --build build
ctest --test-dir build
```

Run a single test:
```bash
ctest --test-dir build -R ArcShouldBeAbleToCreate
```

## Project Structure

- `include/Arc.hpp` — entire library (header-only)
- `src/Arc.cpp` — empty, library is header-only
- `test/ArcSpec.cpp` — Google Test suite
- `build/` — CMake build directory (gitignore'd)

CMake fetches Google Test v1.17.0 on first configure.
