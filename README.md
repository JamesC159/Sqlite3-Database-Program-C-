# Sqlite3-Database-Program-C-
simple, small C++ database program. For use of learning purposes only. I will not be accepting changes to the repository.

If you want to execute this sqlite3 database simulation program, clone to desktop. Place the files in a convenient location. If you are on a UNIX machine (Mac or Linux), simply enter the terminal, change directories to the current location of the files you have cloned (cd ~/your/file/path) and compile the program (g++ -o executable_name main.cpp Artist.cpp Track.cpp Cd.cpp -lsqlite3 -g). After compilation, enter ./executable_name as is. 

If the program does not compile and you wish to fix it yourself, make sure to have a debugger (GDB on Linux or LLDB on MAC). Type gdb executable_name or lldb exectuable_name in terminal. Google commands for debugging.

When running, when the menu system pops up, you should choose the first choice and follow through with the instructions. At the end of the program, the menu system should pop back up. This time you should choose to delete all records from the database. If you do not take these steps, the program will not demonstrate what it should.

#CURRENT IMPLEMENTATION DOES NOT ASSUME BAD INPUT!

Nov 17th update - This program does not deal with memory leaks at the moment. The main focus was to get it working first.

Sorry I did not provide Windows instruction, I am a UNIX man! :)
