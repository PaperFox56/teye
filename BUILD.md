# Building teye

This document explains how to build the teye library from source.

## Prerequisites

### Required Tools
- **GCC** (or any C99-compatible compiler)
- **GNU Make** 3.81 or later
- **Standard C library** (glibc or musl)

### Optional Tools
- **ldconfig** (for system-wide installation)
- **pkg-config** (for development)

### Development Headers
No additional dependencies are required - teye is a self-contained library.

## Building

### Quick Start

```bash
# Clone the repository
git clone https://github.com/yourusername/teye.git
cd teye

# Build both shared and static libraries (default)
make

# Build only shared library
make shared

# Build only static library
make static
```

### Build Options

| Variable | Values | Default | Description |
|----------|--------|---------|-------------|
| `BUILD_TYPE` | `shared`, `static`, `both` | `both` | Which library type(s) to build |
| `DEBUG` | `0`, `1` | `0` | Enable debug symbols and address sanitizer |
| `PREFIX` | path | `/usr/local` | Installation prefix |
| `CC` | compiler | `gcc` | C compiler to use |

### Examples

```bash
# Build with debug symbols
make DEBUG=1

# Build only static library
make static

# Build only static library with debug
make static DEBUG=1

# Build both libraries with debug
make both DEBUG=1

# Build with custom compiler
make CC=clang

# Build for installation in custom location
make PREFIX=$HOME/.local
```

## Build Output

After building, you'll find:

```
teye/
├── bin/              # Object files (.o)
├── lib/              # Compiled libraries
│   ├── libteye.so    # Shared library
│   └── libteye.a     # Static library
└── include/          # Public headers (source)
```

## Running Tests

The test suite validates library functionality:

```bash
# Build and run tests
cd tests
make

# Run specific test
./bounce
# or
./teye-clock

# Clean test artifacts
make clean
```

### Test Configuration

The tests can be built against either library type:

```bash
# Link against static library
make STATIC=1

# Link against shared library with debug
make DEBUG=1

# Build and run tests in one go
make clean && make && ./bounce
```

## Installation

### System-wide Installation

```bash
# Install to /usr/local (requires sudo)
sudo make install

# Or install with custom prefix
make install PREFIX=$HOME/.local
```

The installation copies:
- Libraries to `$(PREFIX)/lib/`
- Headers to `$(PREFIX)/include/teye/`

### Post-Installation

```bash
# Check library
ls -l /usr/local/lib/libteye.*

# Check headers
ls -l /usr/local/include/teye/

# Test linking
gcc -o test test.c -lteye -L/usr/local/lib
```

## Cleaning

```bash
# Remove build artifacts
make clean

# Remove test artifacts
cd tests && make clean

# Uninstall from system
sudo make uninstall
```

## Debugging

### Building with Debug Symbols

```bash
make DEBUG=1
```

This enables:
- `-g` - Debug symbols
- `-fsanitize=address` - Address sanitizer
- `-O0` - No optimization

### Running with Address Sanitizer

When built with `DEBUG=1`, the address sanitizer will catch memory errors:

```bash
# Build with debug
make DEBUG=1

# Run tests - sanitizer will report any issues
cd tests
./bounce
```
