#!/bin/bash

set -e  # Exit immediately if a command fails
set -o pipefail  # Exit if any command in a pipeline fails

VCPKG_DIR="external/vcpkg"

# Ensure git submodule is added and initialized
echo "Cloning vcpkg as a submodule..."
git submodule add https://github.com/microsoft/vcpkg.git $VCPKG_DIR || true
git submodule update --init --recursive

# Navigate to vcpkg directory
cd $VCPKG_DIR

# Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
./bootstrap-vcpkg.sh

# Install graphics libs
echo "Installing glad..."
./vcpkg install glad
echo "Installing sfml..."
./vcpkg install sfml

echo "vcpkg setup complete!"
