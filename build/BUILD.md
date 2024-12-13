# How to Use the Build Scripts

### 1. **First-Time Build and Run**

To build the project and run the executable for the first time:

```bash
./tetris_v1.sh        # Found at the root of the project
```

### 2. **Clean, Build, and Run**

If you want to clean the build directory before building:

```bash
./tetris_v1.sh clean  # Found at the root of the project
```

### 3. **Build Only**

To build the project without running the executable:

```bash
cd build
./build.sh
```

To clean and build:

```bash
cd build
./build.sh clean
```

### 4. **Run Only**

If the project is already built, and you want to run the executable without rebuilding:

```bash
cd build
./run.sh
```

## Requirements

- **CMake** version 3.30 or higher.
- A C++ compiler supporting C++23.
- **ftxui** (fetched automatically by CMake via `FetchContent`).

## Notes

- The `build.sh` script creates the build directory (`build/cmake`) and handles the CMake build process.
- The `run.sh` script runs the `TetrisRoyale_Terminal` executable from the build directory.