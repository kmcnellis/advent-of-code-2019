#!/bin/bash

MAX=0
PATTERN=0
FILE=$1

runProgram() {
  OUT=`./main.exe $FILE $1`
  MAX=$(( $OUT  > $MAX ? $OUT : $MAX ))
  if [ $MAX == $OUT ]; then PATTERN=$1; fi
}

echo "file: " $FILE

while read line ; do runProgram $line ; done < perms.txt

echo "max: " $MAX
echo "pattern: " $PATTERN


