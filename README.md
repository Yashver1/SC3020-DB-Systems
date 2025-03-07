# SC3020 Database Systems Project

This repository contains an implementation of fundamental database components including storage management, indexing with B+ trees, and query processing.

## Project Overview

This project implements several core database components:
- Storage component for efficient record management
- B+ tree indexing for fast data retrieval
- Range query execution with performance analysis

## Dependencies

- C++17 compatible compiler (GCC, Clang)
- CMake (version 3.15 or higher)
- Git (for fetching test dependencies)

## Setup and Build Instructions

## Installing Catch2 Testing Framework

This project uses Catch2 for unit testing. You'll need to have it installed before building the test suite.

# Install via Homebrew
brew install catch2

# Alternatively, for CMake integration
git clone https://github.com/catchorg/Catch2.git
cd Catch2
cmake -Bbuild -H. -DBUILD_TESTING=OFF
cmake --build build/ --target install

### For macOS

1. **Install dependencies:**
   ```bash
   brew install cmake
   ```

2. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/SC3020-DB-Systems.git
   cd SC3020-DB-Systems
   ```

3. **Configure the build:**
   ```bash
   cmake -S . -B build
   ```

4. **Build the project:**
   ```bash
   cmake --build build
   ```

### For Linux (Ubuntu/Debian)

1. **Install dependencies:**
   ```bash
   sudo apt update
   sudo apt install build-essential cmake git
   ```

2. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/SC3020-DB-Systems.git
   cd SC3020-DB-Systems
   ```

3. **Configure the build:**
   ```bash
   cmake -S . -B build
   ```

4. **Build the project:**
   ```bash
   cmake --build build
   ```

## Project Structure

- `include/` - Header files defining the components
- `src/` - Implementation files
- `test/` - Test cases using Catch2 framework
- `build/` - Generated build files (created by CMake)

## Running the Project

After building, you can run the main program from the project root:

```bash
./build/main
```

To run the tests:

```bash
./build/test/test
```

## Components

1. **Record System**
   - Manages fixed-size records for NBA game data
   - Implements efficient disk-based storage

2. **B+ Tree Index**
   - Creates a B+ tree on the specified attribute (FG_PCT_home)
   - Supports range queries and traversals

3. **Query Processing**
   - Executes range queries with performance tracking
   - Compares indexed access vs. brute force scanning

## Troubleshooting

### Missing Symbol Errors (Linker)

If you encounter linker errors about missing symbols (e.g., `_NUMBER_OF_DATA_BLOCKS_BRUTE_FORCE`), ensure that:

1. The `utility.cc` file properly defines all global variables declared in `utility.h`.
2. The build system is correctly linking all components.

### CMake Configuration Issues

If CMake fails to configure:

```bash
# Clean the build directory and retry
rm -rf build
cmake -S . -B build
```

### Compiler Errors

For compiler errors related to C++ standard compatibility:

```bash
# Explicitly specify C++17
cmake -S . -B build -DCMAKE_CXX_STANDARD=17
```

## License

[Your license information here]

