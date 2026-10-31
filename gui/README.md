sudo apt-get update
sudo apt-get install build-essential pkg-config qtbase5-dev qtbase5-dev-tools
make -C gui -j4
./gui/build/tividy-gui
