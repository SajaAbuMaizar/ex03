---------------------------------------------------------------------------------------------------------
---------------------------------------- OOP2 Ex03 - Exceptions -----------------------------------------
---------------------------------------------------------------------------------------------------------
------------------------------------------- Students and IDs -------------------------------------------- 
-------- Samah Rajabi (student number: 211558556) & Saja Abu Maizar (student number: 208072371) ---------
---------------------------------------------------------------------------------------------------------
----------------------------------------  General Description  ------------------------------------------
---------------------------------------------------------------------------------------------------------
The program preforms mathmatical operations on sets.
It runs in the terminal where, in a loop, it recieves input from the user and calculates and displays
the results of the calculation.
It calculates the results of a requested operation on a group of sets of integers, it can preform Union,
intersection or difference between two sets.
The user can also create more complex operations and add them to the calculator and then request
a calculation of their operation on certain sets.
The program can handle unexpected errors and wrong input using the format "try, throw and catch",
for example, it handles "bad input" (e.g letters instead of numbers) by asking the user to re-enter
an appropriate input (an integer).
---------------------------------------------------------------------------------------------------------
------------------------------------------------- Design ------------------------------------------------
---------------------------------------------------------------------------------------------------------
Given a program that executes mathmatical operations on sets, we modified its functions and added new
ones in order to handle exceptions and errors.
The program uses the format "Try, throw and catch" to handle exceptions.
In a do-while loop, the function SetCalculator::run calls for the function SetCalculator::runCommands
that, in each itteration,  "tries" to read an action from the user (by calling the function
SetCalculator::readAction) and to run the action (by calling the function SetCalculator::runAction),
runCommands also "catches" different types of exceptions such as std::invalid_argument and 
std::istream::failure. The functions readAction, runAction and thier helpers throw exceptions when needed
to the function runCommands that catches and handles them.
More functions also contain the format "Try, throw and catch" to handle exceptions in thier body such
as the function setCalculator::Read that catches FileError exceptions.
We mainly used the exceptions that are provided by the standered library. For handeling files exceptions,
we built two classes: InvalidPath and FileError, both of them inherate from std::exeption. 
---------------------------------------------------------------------------------------------------------
--------------------------------------------- List of Files ---------------------------------------------
---------------------------------------------------------------------------------------------------------
Source files:-
--------------
We didn't add executable files to the project.
 
Headers:-
---------
 ● FileException.h: contains classes that handle file exceptions. 
 
Resources:
----------
The user can add the files that contain commands here, he should also add thier titels to cMakeList file.
---------------------------------------------------------------------------------------------------------
-------------------------------------------- Data Structures --------------------------------------------
---------------------------------------------------------------------------------------------------------
Classes:
--------
 ● InvalidPath: to handle invalid pathes of files.
 ● FileError: to handle exceptions that are found in a file while preforming "Read".
---------------------------------------------------------------------------------------------------------
------------------------------------------ Noteable Algorithms ------------------------------------------
---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
---------------------------------------------- Known Bugs -----------------------------------------------
---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------
------------------------------------------- Additional Notes --------------------------------------------
---------------------------------------------------------------------------------------------------------
 ● The command "Read" reads actions from a text file, the user writes the name of the file that contains
   the actions without the extention (.txt). For example, to read command from the file: "Commands.txt",
   the user writes "Read Commands" in the terminal.
 ● The command "Read" can only be executed from the terminal, it isn't support in files, meaning a file 
   can't contain the command "Read".
 ● -------- what happens when a file reads more/less arguments ------------------!!!!!
 ● When the program reads an action that needs N arguments from the terminal, in case of extra arguments
   the program reads the first N arguments, executes the action (or throw an exeption) and finally clears
   the buffer from the extra arguments.




   https://stackoverflow.com/questions/4813129/how-to-get-the-line-number-from-a-file-in-c