#!/bin/bash

# Exit immediately if any command fails
set -e

echo "Updating package lists..."
sudo apt update

echo "Installing build essentials, CMake, and Google Test source..."
sudo apt install -y build-essential cmake libgtest-dev

echo "Building Google Test binaries..."
cd /usr/src/gtest
sudo cmake .
sudo make

echo "Copying built libraries to /usr/lib/..."
sudo cp lib/*.a /usr/lib/

echo "Google Test setup completed successfully!"