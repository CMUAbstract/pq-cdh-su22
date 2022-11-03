#!/bin/bash
#
# setup_dependencies.sh
# A bash script that sets up dependencies
#
# Usage: ./setup_dependencies.sh
# Assumptions:
#  - The prerequisite dependencies described in the top-level README have been
#    successfully installed
# Arguments:
#  - None; this script assumes the git repo directory structure. Note that, due
#    to this assumption, this script should be executed from its containing
#    directory (i.e., the scripts folder)
# Results:
#  - Sets up dependencies for the board software
#
# Written by Bradley Denby
# Other contributors: None
#
# See the top-level LICENSE file for the license.

# print prerequisite message
echo ""
echo "IMPORTANT NOTE: manually perform prerequisites in top-level README first"
read -n 1 -s -r -p "  Press any key to continue"
echo ""
echo ""

# stlink
cd ../utilities/
if [ ! -d "stlink" ]
then
  git clone https://github.com/stlink-org/stlink.git
fi
cd stlink/
git checkout 254a525
make clean
make release
echo ""

# GNU Arm Embedded Toolchain
cd ../
wget https://developer.arm.com/-/media/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
echo "Unpacking..."
tar xjf gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2
echo "GNU Arm Embedded Toolchain ready"
echo "Don't forget to source the sourcefile:"
echo "  $ source sourcefile.txt"
echo "Be sure to source from the scripts/ directory"
echo ""
cd ../scripts/
source sourcefile.txt

# libopencm3
cd ../software/
if [ ! -d "libopencm3" ]
then
  git clone https://github.com/libopencm3/libopencm3.git
fi
cd libopencm3/
git checkout 44e142d
make
echo ""

echo "Dependency setup complete!"
