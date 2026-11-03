#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *memory;
  int pa;
  int i;

  printf(1, "\n--- Virtual to Physical Address Translation ---\n");

  // Allocate three pages.
  memory = sbrk(3 * 4096);

  if(memory == (char*)-1) {
    printf(1, "Memory allocation failed\n");
    exit();
  }

  // Touch all three pages.
  for(i = 0; i < 3; i++)
    memory[i * 4096] = 'A';

  // Translate different virtual addresses.
  for(i = 0; i < 3; i++) {
    printf(1, "\nVirtual address: 0x%x\n",
           (uint)(memory + i * 4096));

    pa = vtop((int)(memory + i * 4096));

    if(pa == -1)
      printf(1, "Translation failed\n");
    else
      printf(1, "Physical address: 0x%x\n", pa);
  }

  // Test an invalid/unmapped address.
  printf(1, "\nTesting invalid virtual address...\n");

  pa = vtop(0x70000000);

  if(pa == -1)
    printf(1, "Invalid address detected successfully.\n");
  else
    printf(1, "Unexpected translation: 0x%x\n", pa);

  sbrk(-3 * 4096);

  printf(1, "\nTranslation test completed.\n");

  exit();
}
