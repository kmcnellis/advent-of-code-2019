#!/bin/bash

MAX=0
PATTERN=0
FILE=$1

runProgram() {
  OUT=`printf "%s\n%s\n"  ${1:0:1} 0    | ./main.exe $FILE`
  OUT=`printf "%s\n%s\n"  ${1:1:1} $OUT | ./main.exe $FILE`
  OUT=`printf "%s\n%s\n"  ${1:2:1} $OUT | ./main.exe $FILE`
  OUT=`printf "%s\n%s\n"  ${1:3:1} $OUT | ./main.exe $FILE`
  OUT=`printf "%s\n%s\n"  ${1:4:1} $OUT | ./main.exe $FILE`
  MAX=$(( $OUT  > $MAX ? $OUT : $MAX ))
  if [ $MAX == $OUT ]; then PATTERN=$1; fi
}

echo "file: " $FILE

while read line ; do runProgram $line ; done < perms.txt

echo "max: " $MAX
echo "pattern: " $PATTERN


