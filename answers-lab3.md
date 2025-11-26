# Questions

## Task 1

### Question 2: Which other xv6 system call(s) could be made faster using this shared page? Explain how.

The only other syscall which could be made faster by using this shared page is a syscall which only provides read only information and doesn't cause other side effects.
For this reason the only possible syscall would be ```SYS_uptime``` since it returns the global counter ticks and has the same value for all the processes.
We could add a page entry where we store the pointer to the current tick value instead of going through the whole syscall process with context switching and can just read the value from the page.
In order to archieve that we can just add another variable with a struct inside the kernel page which then stores the information about the current uptime and gets updated at each interrupt.

Other syscalls cannot be spead up with the shared page since many of the syscalls need arguments to get processed and cannot be realized by reading a single global value from a page.

## Task 2

### Question 2: In the syscall handler, what additional function do you need to use to copy the string from userspace to kernelspace? How does this function work?

We need to use argstr to copy the string from userspace onto kernelspace. 
```argstr``` does call the function which fetches the nth syscall word argument via the ```argaddr``` as a userspace address. Then ```argstr``` call ```fetchstr``` which uses ```copyinstr``` which walks the pagetable of the userprocess to get the physical address of the syscall argument. Then the function copies the word into a buffer so now we have a copy of the user string in kernel memory and can compare if the strings match.
