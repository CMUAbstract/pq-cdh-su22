echo ""
echo "IMPORTANT NOTE: manually perform prerequisites in top-level README first"
read -n 1 -s -r -p "  Press any key to continue"
echo ""
echo ""


274be86616801d65cdc0e1f1bf380c98a7aa5b85

cd ../utilities/
if [ ! -d "stlink" ]
then
  git clone https://github.com/stlink-org/stlink.git
fi
cd stlink/
git checkout 5c03678
make clean
make release
echo ""
