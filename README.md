# Tetris Royale

This project uses CMake to build and run the `TetrisRoyale_Terminal` executable. The build process is divided into multiple scripts for convenience.

## Directory Structure (basic for now)

```
root/
├── build/           # Files made upon building, required for running the project
│   ├── BUILD.md     # Contains the instructions related to building the project from scratch
│   ├── build.sh     # Script to build the project (with optional cleaning)
│   └── run.sh       # Script to run the executable
├── src/             # Source code directory
├── tetris_v1.sh     # Main script to build and run the project
└── CMakeLists.txt   # CMake configuration file
```