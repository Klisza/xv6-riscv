# Questions

## Task 1

### If you modified the variable path during step 4 and continue the execution of xv6, what happens and why?
If we set path[0] = '0' the path is getting set from /init to 0init which does not exist.
Therefore the kernel panics since it does not find the correct exec and the exec fails of the init program fails.


## Task 2

```
va 0x0 pte 0x21FC885B pa 0x87F22000 perm 0x5B
va 0x1000 pte 0x21FC7C1B pa 0x87F1F000 perm 0x1B
va 0x2000 pte 0x21FC7817 pa 0x87F1E000 perm 0x17
va 0x3000 pte 0x21FC7407 pa 0x87F1D000 perm 0x7
va 0x4000 pte 0x21FC70D7 pa 0x87F1C000 perm 0xD7
va 0x5000 pte 0x0 pa 0x0 perm 0x0
va 0x6000 pte 0x0 pa 0x0 perm 0x0
va 0x7000 pte 0x0 pa 0x0 perm 0x0
va 0x8000 pte 0x0 pa 0x0 perm 0x0
va 0x9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF6000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF7000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF8000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFF9000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFA000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFB000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFC000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFD000 pte 0x0 pa 0x0 perm 0x0
va 0x3FFFFFE000 pte 0x21FD08C7 pa 0x87F42000 perm 0xC7
va 0x3FFFFFF000 pte 0x2000184B pa 0x80006000 perm 0x4B

```

### For every page table entry, explain what it logically contains and what its permission bits are.

The virtual addresses 0x0 - 0x1000 are the text page entries which contain the code of the program.
The permission bits are set for read-execute.

The virtual addresses 0x2000 - 0x4000 are the data page entries which contain the data of the current process with bits set for read-write permissions.
The virtual address 0x3000 furthermore is kernel data since the user bit is unset.

The virtual addresses 0x5000 - 0x9000 and 0x3FFFFF6000 - 0x3FFFFFF000 are mapped to the unused page table entries and have no permissions set.

The last two virtual addresses are the trap frame page entry with read-write permission and with user permission unset and the trampoline page entry with read-execution permission bits set and user persission bit unset.
