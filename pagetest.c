#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *memory;
  int pid;

  printf(1, "\n===== PAGE PROTECTION TEST =====\n");

  // Allocate one page.
  memory = sbrk(4096);

  if(memory == (char*)-1) {
    printf(1, "Memory allocation failed\n");
    exit();
  }

  // Ensure the page is physically allocated.
  memory[0] = 'A';

  printf(1, "\n1. Initial page information:\n");
  pageinfo((int)memory);

  // Change the page to read-only.
  printf(1, "\n2. Changing page permissions to read-only...\n");

  if(setreadonly((int)memory) < 0) {
    printf(1, "Failed to change page permissions\n");
    exit();
  }

  printf(1, "Page permissions changed successfully.\n");

  printf(1, "\n3. Updated page information:\n");
  pageinfo((int)memory);

  // Fork so that the parent survives the child's page fault.
  printf(1, "\n4. Attempting to write to the protected page...\n");

  pid = fork();

  if(pid < 0) {
    printf(1, "Fork failed\n");
    exit();
  }

  if(pid == 0) {
    printf(1, "Child attempting write...\n");

    memory[0] = 'B';

    printf(1, "ERROR: Write unexpectedly succeeded.\n");
    exit();
  }

  wait();

  printf(1, "\nChild terminated after the protected write attempt.\n");
  printf(1, "Page protection test completed.\n");

  exit();
}
