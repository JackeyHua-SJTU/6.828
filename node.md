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