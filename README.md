# OperatingSystems4
Dynamic Realtime backup a FileSystem using inotify event monitoring system for Linux.
to run it : make clean && make && valgrind ./test3 source/ backup/
I have turned off the signal handling in inotifyFunctions.c
rm -rf backup/ && make clean && make && ./mirr source/ backup/