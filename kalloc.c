// Physical memory allocator with memory statistics tracking.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);

extern char end[];

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

int allocated_pages = 0;
int freed_pages = 0;

// Initialize allocator in two phases.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;

  p = (char*)PGROUNDUP((uint)vstart);

  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}

// Free a physical memory page.
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);

  // Count only runtime deallocations.
  if(kmem.use_lock)
    freed_pages++;

  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one physical memory page.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);

  r = kmem.freelist;

  if(r) {
    kmem.freelist = r->next;

    if(kmem.use_lock)
      allocated_pages++;
  }

  if(kmem.use_lock)
    release(&kmem.lock);

  return (char*)r;
}

// Display physical memory allocation statistics.
void
kalloc_memstats(void)
{
  struct run *r;
  int available = 0;

  acquire(&kmem.lock);

  for(r = kmem.freelist; r; r = r->next)
    available++;

  cprintf("\n--- Memory Statistics ---\n");
  cprintf("Total allocated pages: %d\n", allocated_pages);
  cprintf("Total freed pages: %d\n", freed_pages);
  cprintf("Currently allocated pages: %d\n",
          allocated_pages - freed_pages);
  cprintf("Available physical memory pages: %d\n", available);
  cprintf("-------------------------\n");

  release(&kmem.lock);
}
