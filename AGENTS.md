# AGENTS.md

This file provides guidance to agents when working with code in this repository.

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

- `include/arc/Arc.hpp` — main library (header-only, note `arc/` subdirectory)
- `src/Arc.cpp` — empty, library is header-only
- `test/src/*Spec.cpp` — Google Test suites (uses `*Spec.cpp` naming pattern)
- `build/` — CMake build directory (gitignore'd)

CMake fetches Google Test v1.17.0 on first configure.
