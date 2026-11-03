# Operating Systems – Assignment 10

## Roll-no: 2403MC04
## Name: Leon Joel Shaji

## Memory Management and Page Replacement

### 1. Introduction

The objective of this assignment was to understand and implement some of the memory management mechanisms used in operating systems. The work was carried out using the xv6 operating system (x86 version).

The assignment covered four main areas: physical memory allocation statistics, virtual-to-physical address translation, page table inspection and protection, and page replacement algorithms.

The existing xv6 implementation was modified wherever necessary, and separate user programs were written to test each implementation.

---

### 2. Question 1 – Physical Memory Statistics

The first part involved modifying the physical memory allocator in `kalloc.c` to maintain statistics about memory allocation and deallocation.

Three counters were introduced:

- Total allocated pages
- Total freed pages
- Currently allocated pages

The counters were updated inside the existing `kalloc()` and `kfree()` functions. Whenever a page is successfully allocated, the allocation counter and currently allocated counter are incremented. Whenever a page is freed, the freed counter is incremented and the currently allocated counter is decremented.

A new system call, `memstats()`, was implemented to display these statistics along with the number of available physical memory pages.

A user program named `memtest.c` was created to test the implementation. It initially displayed the memory statistics, allocated five pages using `sbrk()`, accessed the allocated memory and then released the pages.

**Observed result:**

| Metric | Before allocation | After allocation | After deallocation |
|---|---:|---:|---:|
| Total allocated pages | 425 | 430 | 430 |
| Total freed pages | 214 | 214 | 219 |
| Currently allocated pages | 211 | 216 | 211 |
| Available physical pages | 56790 | 56785 | 56790 |

The results show that allocating five pages increased the allocated page count by five. After releasing the pages, the currently allocated count and available physical memory returned to their initial values.

---

### 3. Question 2 – Virtual-to-Physical Address Translation

The second part involved implementing a mechanism to translate a virtual address into its corresponding physical address.

In xv6, each process has its own page directory and page tables. These structures maintain the mapping between virtual pages and physical memory frames.

A function named `vtop()` was added to `vm.c`. It uses the existing `walkpgdir()` function to locate the page table entry corresponding to a given virtual address.

The implementation checks whether the address belongs to the process, whether a corresponding page table entry exists, and whether the page is present and accessible in user mode.

The physical address is calculated by extracting the physical frame address using `PTE_ADDR()` and combining it with the original page offset.

A system call named `vtop()` was registered so that user programs could request address translation. A test program named `vttest.c` was written to allocate three pages, access them and translate their virtual addresses.

**Observed result:**

| Virtual Address | Physical Address |
|---|---|
| 0x3000 | 0xDFBC000 |
| 0x4000 | 0xDF76000 |
| 0x5000 | 0xDFBF000 |

An additional test was performed using an invalid virtual address. The implementation correctly rejected the address and returned an error.

The experiment demonstrated that consecutive virtual addresses do not necessarily correspond to consecutive physical addresses. The mapping is determined by the page tables.

---

### 4. Question 3 – Page Table Inspection and Protection

The third part focused on inspecting page table entries and modifying memory access permissions.

Two functions were added to `vm.c`:

**1. `pageinfo()`**

This function accepts a virtual address, locates its page table entry and displays the following information:

- Physical frame number
- Present bit (P)
- Writable bit (W)
- User bit (U)

**2. `setreadonly()`**

This function removes the write permission from a mapped user page by clearing the `PTE_W` bit in its page table entry.

The TLB was also flushed using `lcr3()` so that the updated permissions would take effect immediately.

Both functions were exposed through system calls. A user program named `pagetest.c` was created to test the implementation.

The program allocated a page, accessed it, displayed its initial permissions and then changed the page to read-only.

**Observed result:**

Initial page permissions:

```text
Present (P): 1
Writable (W): 1
User (U): 1
```

After changing the page to read-only:

```text
Present (P): 1
Writable (W): 0
User (U): 1
```

To verify the protection, the program created a child process using `fork()` and attempted to write to the protected page.

The write operation triggered a page fault:

```text
trap 14 err 7
```

The child process was terminated, while the parent process continued execution.

This demonstrated that the processor correctly enforced the page-level write protection configured through the page table.

---

### 5. Question 4 – Page Replacement Algorithms

The final part involved implementing and comparing three commonly used page replacement algorithms:

1. First In First Out (FIFO)
2. Least Recently Used (LRU)
3. Optimal Page Replacement

A standalone C program named `pagereplace.c` was developed to simulate all three algorithms.

The following reference string was used:

```text
7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1
```

The algorithms were tested with 3, 4 and 5 page frames.

**Algorithm descriptions:**

- **FIFO:** Replaces the page that has been in memory for the longest time, irrespective of how recently it was accessed.
- **LRU:** Replaces the page that has not been used for the longest period.
- **Optimal:** Replaces the page whose next reference occurs farthest in the future. It provides the minimum possible number of page faults for a given reference string.

The program displays the frame contents after each page reference and identifies whether the reference resulted in a hit or a page fault.

**Observed results:**

| Frames | Algorithm | Page Faults | Page Hits | Hit Ratio |
|---:|---|---:|---:|---:|
| 3 | FIFO | 15 | 5 | 25% |
| 3 | LRU | 12 | 8 | 40% |
| 3 | Optimal | 9 | 11 | 55% |
| 4 | FIFO | 10 | 10 | 50% |
| 4 | LRU | 8 | 12 | 60% |
| 4 | Optimal | 8 | 12 | 60% |
| 5 | FIFO | 9 | 11 | 55% |
| 5 | LRU | 7 | 13 | 65% |
| 5 | Optimal | 7 | 13 | 65% |

**Observations:**

- FIFO produced the highest number of page faults in all three configurations.
- LRU performed better than FIFO because it considers recent page usage.
- Optimal produced the lowest number of page faults.
- Increasing the number of frames generally reduced the number of page faults.
- For this particular reference string, LRU and Optimal produced identical results with four and five frames.

The experiment helped demonstrate how the choice of page replacement algorithm affects memory performance.

---

### 6. Conclusion

Through this assignment, I worked with several important memory management concepts in xv6.

The first three questions involved modifying the existing kernel, introducing new system calls and writing user-level programs to verify their behaviour. The fourth question involved implementing page replacement algorithms and comparing their performance using a fixed reference string.

The experiments provided a better understanding of how physical memory is allocated, how virtual addresses are translated, how page-level permissions are enforced by hardware, and how operating systems manage limited physical memory.

All four implementations were compiled and tested successfully, and the observed results matched the expected behaviour.
