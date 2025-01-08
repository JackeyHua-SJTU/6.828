# Lab pagetable

## C programming language
数组名可以退化为指向数组第一个元素的指针 所以int*既可以指向一个int类型的变量，也可以指向一个int数组的任何一个index

```C
typedef unsigned int uint64;
uint64 x = 0xffff;
void* a = x;
```
那么此时`a`的值就是0xffff 解引用才可以去访问0xffff地址处的东西

## Lab part
### Speed up system calls
We manually add a **USYSCALL** part under the **trapframe** of user address space. So we need to allocate a page for that in `allocproc()`. And then we need to map the user address to the physical address of the allocated page using `mappages()`. Do not forget to `kfree()` the page in `freeproc()`, or `uvmfree()` will throw an exception, because we have not freed all leaf nodes.

Notice the permission. User space should have **read** access to the shared page. So `PTE_R` and `PTE_U` perm are granted.

## Print a page table
We can step into the current level page table, check whether the entry is valid. If valid, first print the current pte info, and then recursively steps into the next level page table. _We should stop if the level is greater than 3_.

Since we need to print additional parameter info at the beginning, we can define a recursive helper function to traverse the whole page table.

## Detecting which pages have been accessed
First, read the reference card to find the `PTE_A` mask bit.

Then, we need to get the three parameter, `source_va`, `# of page`, and `dst_va`. Read the `pgtbltest.c` and you can see the passed-in destination address has a `unsigned int` type, so we need to define a 32-bit mask.

After that, we get every page's pte via `walk()`, and check whether it is accessed. If accessed, set the corresponding bit in the mask, and then clears the access bit.

Finally, write the mask back via `copyout()`.

## MISC
When we allocate a process, actually only `trampoline, trapframe` are really mapped, others left blank.

> **Question**: How to understand vm.c:87 use pagetable as a virtual address? How C (as a running process) has access to physical address, given that instructions can only use virtual address?

**Answer**: Because the `walk` function is a kernel only function. Most of the kernel VA to PA is direct mapped, which means that **VA = PA**. (If in the user space, we can not say for sure that after looking the page table, the PA we get equals to VA. Refer to the following code analysis part.) Besides, all user processes are in the direct mapped area. Therefore we can assure that directly visit the VA has the same effect of visiting the PA. 

Let's look a step further at the RISC-V assembly code of a small C code snippet to figure out **which part of the code in user space will include VA-PA transform**?

```C
typedef unsigned long uint64;
typedef uint64 pte_t;
typedef uint64* pagetable_t;

pte_t *
walk(pagetable_t pagetable, uint64 va)
{
    pte_t *pte = &pagetable[va];   
}
```
```Assembly
walk:
        addi    sp,sp,-48
        sd      ra,40(sp)
        sd      s0,32(sp)
        addi    s0,sp,48
        sd      a0,-40(s0)
        sd      a1,-48(s0)
        ld      a5,-48(s0)
        slli    a5,a5,3
        ld      a4,-40(s0)
        add     a5,a4,a5
        sd      a5,-24(s0)

```

Direct memory visit instructions like `ld` and `sd` include VA to PA transform. Take `sd a0,-40(s0)` as an example. The base address in `s0` register plus the bias -40 will be transformed according to the page table and then stores to the new physical address, instead of the virtual address pointed by `s0 - 40`. Suppose the physical address is `pa`, then in kernel space `pa == s0 - 40`, but this equation **may not hold** in user space.