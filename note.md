# Lab Syscall
> 3-4 hours in total

Process of calling a system call
- Call the prototype in user mode
- Store the corresponding ID in `a7` register and switch to kernel mode via `ecall`
- Kernel will retrieve the ID from `a7` in the trap frame and execute the real handler function
- Kernel will set the return value back to the `a0` in the trap frame

Kernel retrieves the parameter also from the registers in the trap frame. Therefore, _Trap frame is the channel for communication_.

Note that the prototypes in user mode and kernel mode are different.
> User mode : int trace(int);

The user mode prototype is to be called by the user, and the trap operation will place the parameter of the function into corresponding registers.

> Kernel mode : uint64 sys_sysinfo(void);

The kernel mode prototype is to be called by the kernel. Besides, notice the function pointer array `uint64 (*syscalls[])(void)`, we need a **unified** function declaration. Given that kernel can retrieve the parameters by registers in the trap frame, the prototype does not need any parameters, thus `void`.