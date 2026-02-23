# Termi - Console Games Framework

[中文版](../README.md)

A lightweight C++ terminal-based game engine with built-in games (Snake & Tetris).

## Features

- **Cross-terminal Graphics Engine** - Custom console rendering with ANSI color support
- **Two Classic Games**
  - Snake with AI autopilot
  - Tetris with AI controller
- **Standard Library Only** - C++ standard library + Linux platform APIs, no third-party libraries
- **CMake build system** with preset configurations

## Project Structure

```
termi/
├── TermiCoreBase/          # Core game engine library
│   ├── include/            # Public headers
│   └── src/                # Implementation
├── games/
│   ├── snake/              # Snake game
│   │   ├── src/            # Game source
│   │   └── tests/          # Unit tests
│   └── tetris/             # Tetris game
│       ├── src/            # Game source
│       └── tests/          # Unit tests
├── doc/                    # English documentation
├── CMakePresets.json       # Build presets (shared)
└── CMakeUserPresets.json   # Local build presets
```

## Requirements

- C++17 compatible compiler
- CMake 3.21+
- Linux terminal (ANSI escape codes)

## Build

```bash
# Configure
cmake --preset=debug      # or: cmake --preset=release

# Build
cmake --build --preset=debug

# Run tests
ctest --preset=test-all
```

## Games

### Snake
Move: `W/A/S/D` or Arrow keys  
Pause: `P`  
AI Mode: `M`  
Fast Mode: `F`  
Reset: `R`

### Tetris
Move: `A/D`  
Rotate: `W` or `↑`  
Soft Drop: `S` or `↓`  
Hard Drop: `Space`  
AI Mode: `M`  
Pause: `P`  
Reset: `R`

## License

Copyright © 2024 [Theqiqi]

- **Personal Use**: Free, but attribution required
- **Commercial License**: Contact author for licensing
- **Copyleft**: Derivative works must use the same license
