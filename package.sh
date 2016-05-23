# Packages the sources and the compiled binaries

NAME="sulzbach-ferrazza.zip"
HELPER="packaging-helper"

rm -f $NAME
zip -q -r $NAME . -x .\* blender/\* build/\*

mkdir $HELPER
cd $HELPER
cmake ..
make
cd ..
zip -q -j $NAME $HELPER/game/walls-of-doom
rm -rf $HELPER
