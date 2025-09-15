#!/bin/sh

OPTIONS="-I. "

comp() {
  gcc -E -Wp,-v - 2>&1 << EOF
EOF
}

I=`comp | grep "^ /"`
for i in $I
do
  OPTIONS="$OPTIONS -I$i "
done

echo $R
for option
do
  case "$option" in
  -I* ) OPTIONS="$OPTIONS $option"
     ;;
  -D* ) OPTIONS="$OPTIONS $option"
     ;;
  esac
done

echo $OPTIONS
