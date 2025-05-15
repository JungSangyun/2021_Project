#!/bin/bash

set -e

# Ensure the script is run with root privileges
if [[ "$EUID" -ne 0 ]]; then
  echo "Please run this script as root (use sudo)"
  exit 1
fi

echo "Checking system architecture..."
ARCHITECTURE=$(dpkg --print-architecture)
SUPPORTED_ARCHS=("amd64" "arm64" "i386")

if [[ ! " ${SUPPORTED_ARCHS[@]} " =~ " ${ARCHITECTURE} " ]]; then
  echo "Unsupported architecture: $ARCHITECTURE"
  echo "Supported architectures: ${SUPPORTED_ARCHS[*]}"
  exit 1
fi

echo "Detected architecture: $ARCHITECTURE"

# Get OS codename
echo "Detecting OS codename..."
CODENAME=$(lsb_release -c -s)

if [[ -z "$CODENAME" ]]; then
  echo "Could not detect OS codename. Make sure lsb-release is installed."
  exit 1
fi

echo "OS Codename: $CODENAME"

# Install required packages
echo "Installing prerequisites..."
apt update
apt install -y apt-transport-https gnupg wget

# Add Tor Project repository
echo "Adding Tor Project repository..."
REPO_FILE="/etc/apt/sources.list.d/tor.list"

cat <<EOF > "$REPO_FILE"
deb     [arch=$ARCHITECTURE signed-by=/usr/share/keyrings/deb.torproject.org-keyring.gpg] https://deb.torproject.org/torproject.org $CODENAME main
deb-src [arch=$ARCHITECTURE signed-by=/usr/share/keyrings/deb.torproject.org-keyring.gpg] https://deb.torproject.org/torproject.org $CODENAME main
EOF

# Add the GPG key
echo "Importing GPG key..."
wget -qO- https://deb.torproject.org/torproject.org/A3C4F0F979CAA22CDBA8F512EE8CBC9E886DDD89.asc | gpg --dearmor > /usr/share/keyrings/deb.torproject.org-keyring.gpg

# Install Tor
echo "Updating package list and installing Tor..."
apt update
apt install -y tor deb.torproject.org-keyring

echo "Tor installation completed successfully!"
