#!/bin/bash

# Test script for libnmcli
set -e

echo "=== Testing libnmcli compilation and usage ==="

# Check if we're in the right directory
if [ ! -f "meson.build" ]; then
    echo "Error: Please run this script from the clients/cli directory"
    exit 1
fi

# Go to build directory
cd ../../build

echo "1. Building libnmcli..."
ninja nmcli

echo "2. Building example program..."
ninja nmcli-example

echo "3. Testing example program..."
if [ -f "nmcli-example" ]; then
    echo "Running nmcli-example..."
    ./nmcli-example
    echo "Example program completed successfully"
else
    echo "Error: nmcli-example not found"
    exit 1
fi

echo "4. Checking library files..."
if [ -f "libnmcli/libnmcli.so" ]; then
    echo "✓ Shared library created: libnmcli/libnmcli.so"
    ls -la libnmcli/libnmcli.so
else
    echo "✗ Shared library not found"
    exit 1
fi

if [ -f "libnmcli.pc" ]; then
    echo "✓ pkg-config file created: libnmcli.pc"
    cat libnmcli.pc
else
    echo "✗ pkg-config file not found"
    exit 1
fi

echo "5. Testing pkg-config..."
if pkg-config --exists libnmcli; then
    echo "✓ pkg-config recognizes libnmcli"
    echo "CFLAGS: $(pkg-config --cflags libnmcli)"
    echo "LIBS: $(pkg-config --libs libnmcli)"
else
    echo "✗ pkg-config does not recognize libnmcli"
    exit 1
fi

echo "6. Testing simple compilation with pkg-config..."
cat > test_libnmcli.c << 'EOF'
#include <stdio.h>
#include "nmcli.h"

int main() {
    printf("Testing libnmcli version: %s\n", nmcli_get_version());
    
    if (!nmcli_lib_init()) {
        fprintf(stderr, "Failed to initialize libnmcli\n");
        return 1;
    }
    
    printf("libnmcli initialized successfully\n");
    
    nmcli_lib_cleanup();
    printf("libnmcli cleanup completed\n");
    
    return 0;
}
EOF

gcc -o test_libnmcli test_libnmcli.c $(pkg-config --cflags --libs libnmcli)
echo "✓ Test program compiled successfully"

echo "7. Running test program..."
./test_libnmcli

echo "8. Cleaning up test files..."
rm -f test_libnmcli.c test_libnmcli

echo ""
echo "=== All tests passed! ==="
echo ""
echo "libnmcli has been successfully built and tested."
echo "You can now use it in your applications."
echo ""
echo "To install the library:"
echo "  sudo ninja install"
echo ""
echo "To use in your programs:"
echo "  gcc -o myapp myapp.c \$(pkg-config --cflags --libs libnmcli)" 