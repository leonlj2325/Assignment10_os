#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *memory;
  int size;
  int i;

  size = 5 * 4096;

  printf(1, "\nInitial memory statistics:\n");
  memstats();

  printf(1, "\nAllocating 5 pages...\n");

  memory = sbrk(size);

  if(memory == (char*)-1) {
    printf(1, "Allocation failed\n");
    exit();
  }

  // Access each allocated page.
  for(i = 0; i < 5; i++)
    memory[i * 4096] = 'A';

  printf(1, "\nMemory statistics after allocation:\n");
  memstats();

  printf(1, "\nReleasing 5 pages...\n");

  if(sbrk(-size) == (char*)-1) {
    printf(1, "Deallocation failed\n");
    exit();
  }

  printf(1, "\nMemory statistics after deallocation:\n");
  memstats();

  exit();
}
