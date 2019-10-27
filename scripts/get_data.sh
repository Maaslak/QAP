# Should be run from workdir

DATA_DIR=data
 
if test -f "$DATA_DIR"; then
    echo "$DATA_DIR exists"
else
    mkdir data
fi


TMP_DIR="$DATA_DIR/tmp"
mkdir "$TMP_DIR"
wget -P "$TMP_DIR" http://www.cs.put.poznan.pl/mkomosinski/lectures/mam/qap.zip
unzip "$TMP_DIR/qap.zip" -d "$TMP_DIR"
unzip "$TMP_DIR/qapdatsol.zip" -d "$DATA_DIR"
rm -rf "$TMP_DIR"
