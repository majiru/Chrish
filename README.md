# CHRISH 1 CPRE308-SPRING-2019

## NAME
Chris Shell

## SYNOPSIS
chrish [-p prompt]

## DESCRIPTION
A flexible practical shell made for chris and chris enthusiests.

## SPECIAL CHARACTERS

* ; or \n : End of command
* | : Pipe command
* < : replace stdin with file
* > replace stdout with file

## BUILTINS

* exit : Closes the program

* pid : Prints the current PID

* ppid : Prints parent pid

* pwd : Print current directory

* set [name] [value] : Sets enviornment variable

* get [name] : prints value of enviornment variable

* cd [dir] : Change directory to dir.
if dir is ommited, then cd will move to $HOME on unix and $home on plan9.


## DESIGN

Chrish is built to run on Plan9 and Unix derivatives. It is written
in a dialect of c89 commonly reffered to as 9c. 9c provides a alternative
to the commonaly used GNU glibc, and it is mostly MIT Licensed. This program
is able to run on unix thanks to the help of Russ Cox's Plan9Port. Which is a
compatability layer for compiling 9c source code under Unix hosts. The GNU
makefile included expects a vendored version of p9p for simplicity.

Most of the program itself is based around a Command strucut. This struct
is used mostly for ease of design, as it helps for the purpose of redirecting
stdin, stdout, and stderr to either other programs or file descriptors. The command.c
file contains all of the relevant code for making using of the Command structure.

To make this program work on both operating systems, the use of pre processor declaratives
are used in os critical sections. The mkfile and GNU Makefile have these declared as part
of the build process.

## BUILDING

* plan9: `mk`
* unix: `make`

## AUTHOR

Jacob Moody
