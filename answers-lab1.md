# Questions

## Task 2

### Question 1: What syscall would you need for this task?

For this task we need to use the syscalls:

* fork: in order to call child processes
* exec: to launch our desired program
* wait: to wait until the child is done in order to see if it failed and to relaunch after it is done.
* pause: gives use the possiblity to sleep the program

### Question 2: Examing the Makefile of xv6, what line should you add so that your file compiles?
We need to add our program to the user programs in the Makefile so the line we need to add is:
`$U/_watch`

### Question 3: Try to kill your program with CTRL + C. What’s happening? Why?
The shell do not handle the shortcut CTRL + C to send a kill signal, so nothing happens and the program runs without interuption.

