#!/bin/bash

# Aura Retail OS - Compilation and Execution Script

echo "===================================="
echo "  Compiling Aura Retail OS..."
echo "===================================="

cd src
g++ -std=c++11 main.cpp -o aura_kiosk

if [ $? -eq 0 ]; then
    echo "✅ Compilation successful!"
    echo ""
    echo "===================================="
    echo "  Running Simulation..."
    echo "===================================="
    echo ""
    ./aura_kiosk
else
    echo "❌ Compilation failed!"
    exit 1
fi
