#!/bin/bash



echo "Watching starts"

while true; do

inotifywait -e modify,create,delete,move -r -m /home/aristofanis/Desktop/OS4/testDirs

done