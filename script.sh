#!/bin/bash
dirname="$1"
tarname="$2"
mkdir $HOME/files/$dirname
while [ "$3" ]
do
find ~ -iname "*.$3" -exec cp {} $HOME/files/$dirname \;
shift
done
tar -cf $HOME/files/$tarname.tar $HOME/files/$dirname
openssl des3 -in $HOME/files/$tarname.tar -out $HOME/files/$tarname.tar.des3
