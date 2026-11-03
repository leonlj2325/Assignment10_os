#include "types.h"
#include "stat.h"
#include "user.h"

#define MAX_FRAMES 5
#define REF_LEN 20

int pages[REF_LEN] = {
  7, 0, 1, 2, 0, 3, 0, 4, 2, 3,
  0, 3, 2, 1, 2, 0, 1, 7, 0, 1
};

void
printframes(int frames[], int n)
{
  int i;

  printf(1, "[");

  for(i = 0; i < n; i++) {
    if(frames[i] == -1)
      printf(1, " - ");
    else
      printf(1, " %d ", frames[i]);
  }

  printf(1, "]");
}

int
findpage(int frames[], int n, int page)
{
  int i;

  for(i = 0; i < n; i++) {
    if(frames[i] == page)
      return i;
  }

  return -1;
}

void
printpercentage(char *label, int value)
{
  int result = value * 1000 / REF_LEN;

  printf(1, "%s%d.%d%%", label, result / 10, result % 10);
}

// FIFO PAGE REPLACEMENT
void
fifo(int n, int show)
{
  int frames[MAX_FRAMES];
  int next = 0;
  int faults = 0;
  int hits = 0;
  int i;

  for(i = 0; i < n; i++)
    frames[i] = -1;

  if(show)
    printf(1, "\n--- FIFO: %d Frames ---\n", n);

  for(i = 0; i < REF_LEN; i++) {
    int page = pages[i];

    if(findpage(frames, n, page) != -1) {
      hits++;

      if(show) {
        printf(1, "Page %d: HIT  ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
    else {
      faults++;
      frames[next] = page;
      next = (next + 1) % n;

      if(show) {
        printf(1, "Page %d: FAULT ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
  }

  printf(1, "FIFO Results (%d frames): Faults = %d, Hits = %d\n",
         n, faults, hits);

  if(show) {
    printpercentage("Hit ratio: ", hits);
    printf(1, "\n");
    printpercentage("Page fault ratio: ", faults);
    printf(1, "\n");
  }
}

// LRU PAGE REPLACEMENT
void
lru(int n, int show)
{
  int frames[MAX_FRAMES];
  int lastused[MAX_FRAMES];
  int faults = 0;
  int hits = 0;
  int i, j, pos, oldest;

  for(i = 0; i < n; i++) {
    frames[i] = -1;
    lastused[i] = -1;
  }

  if(show)
    printf(1, "\n--- LRU: %d Frames ---\n", n);

  for(i = 0; i < REF_LEN; i++) {
    int page = pages[i];

    pos = findpage(frames, n, page);

    if(pos != -1) {
      hits++;
      lastused[pos] = i;

      if(show) {
        printf(1, "Page %d: HIT  ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
    else {
      faults++;

      pos = findpage(frames, n, -1);

      if(pos == -1) {
        oldest = 0;

        for(j = 1; j < n; j++) {
          if(lastused[j] < lastused[oldest])
            oldest = j;
        }

        pos = oldest;
      }

      frames[pos] = page;
      lastused[pos] = i;

      if(show) {
        printf(1, "Page %d: FAULT ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
  }

  printf(1, "LRU Results (%d frames): Faults = %d, Hits = %d\n",
         n, faults, hits);

  if(show) {
    printpercentage("Hit ratio: ", hits);
    printf(1, "\n");
    printpercentage("Page fault ratio: ", faults);
    printf(1, "\n");
  }
}

// OPTIMAL PAGE REPLACEMENT
void
optimal(int n, int show)
{
  int frames[MAX_FRAMES];
  int faults = 0;
  int hits = 0;
  int i, j, k, pos;
  int farthest, victim, nextuse;

  for(i = 0; i < n; i++)
    frames[i] = -1;

  if(show)
    printf(1, "\n--- OPTIMAL: %d Frames ---\n", n);

  for(i = 0; i < REF_LEN; i++) {
    int page = pages[i];

    if(findpage(frames, n, page) != -1) {
      hits++;

      if(show) {
        printf(1, "Page %d: HIT  ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
    else {
      faults++;

      pos = findpage(frames, n, -1);

      if(pos == -1) {
        farthest = -1;
        victim = 0;

        for(j = 0; j < n; j++) {
          nextuse = REF_LEN + 1;

          for(k = i + 1; k < REF_LEN; k++) {
            if(pages[k] == frames[j]) {
              nextuse = k;
              break;
            }
          }

          if(nextuse > farthest) {
            farthest = nextuse;
            victim = j;
          }
        }

        pos = victim;
      }

      frames[pos] = page;

      if(show) {
        printf(1, "Page %d: FAULT ", page);
        printframes(frames, n);
        printf(1, "\n");
      }
    }
  }

  printf(1, "OPTIMAL Results (%d frames): Faults = %d, Hits = %d\n",
         n, faults, hits);

  if(show) {
    printpercentage("Hit ratio: ", hits);
    printf(1, "\n");
    printpercentage("Page fault ratio: ", faults);
    printf(1, "\n");
  }
}

int
main(void)
{
  int n;

  printf(1, "\n====================================\n");
  printf(1, " PAGE REPLACEMENT ALGORITHM TEST\n");
  printf(1, "====================================\n");

  printf(1, "\nReference String:\n");

  for(n = 0; n < REF_LEN; n++)
    printf(1, "%d ", pages[n]);

  printf(1, "\n\n");

  // Detailed execution with three frames.
  fifo(3, 1);
  lru(3, 1);
  optimal(3, 1);

  // Comparison for 3, 4 and 5 frames.
  printf(1, "\n====================================\n");
  printf(1, " COMPARISON TABLE\n");
  printf(1, "====================================\n");

  for(n = 3; n <= 5; n++) {
    printf(1, "\n\n========== %d FRAMES ==========\n", n);

    fifo(n, 0);
    lru(n, 0);
    optimal(n, 0);
  }

  printf(1, "\nPage replacement testing completed.\n");

  exit();
}
