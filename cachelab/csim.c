#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cachelab.h"

typedef struct Line {
  int valid;
  int tag;
  // for lru eviction, remove the biggest counter, set 0 when used
  int lru_counter;
} Line;

typedef struct Set {
  Line *lines;
} Set;

/* global variables */
unsigned int s = 0, E = 0, b = 0;
char access_log[20] = "";
Set *sets;
int hit_count = 0, miss_count = 0, eviction_count = 0;

void access_cache(unsigned long address) {
  unsigned int tag = address >> (s + b);
  int s_index = address >> b & ((1 << s) - 1);  // (1 << s)-1 for %
  int eviction_index = -1, max_counter = -1;
  int need_evict = 1;

  // first loop to see whether there is a match, or cold start
  for (size_t i = 0; i < E; i++) {
    if (sets[s_index].lines[i].valid) {
      // valid, compare tag
      if (sets[s_index].lines[i].tag == tag) {
        // hit
        hit_count += 1;
        sets[s_index].lines[i].lru_counter = 0;
        need_evict = 0;
        strcat(access_log, "hit ");
        break;
      } else {
        // tag mismatch, continue to next one
        continue;
      }
    } else {
      // cold start
      sets[s_index].lines[i].valid = 1;
      sets[s_index].lines[i].tag = tag;
      sets[s_index].lines[i].lru_counter = 0;
      strcat(access_log, "miss ");
      miss_count += 1;
      need_evict = 0;
      break;
    }
  }

  // second loop: add 1 to lru_counter and find the biggest one
  // may do eviction(only happens when all of the lines are valid)
  for (int i = 0; i < E; i++) {
    if (max_counter < sets[s_index].lines[i].lru_counter) {
      eviction_index = i;
      max_counter = sets[s_index].lines[i].lru_counter;
    }
    sets[s_index].lines[i].lru_counter += 1;
  }
  if (need_evict) {
    sets[s_index].lines[eviction_index].valid = 1;
    sets[s_index].lines[eviction_index].tag = tag;
    sets[s_index].lines[eviction_index].lru_counter = 0;
    strcat(access_log, "miss eviction ");
    miss_count += 1;
    eviction_count += 1;
  }
}

int main(int argc, char **argv) {
  /* P1: get user input*/
  int opt, hflag = 0, vflag = 0;
  char *tflag;
  // no ":" means an option, one ":" means there must have one param
  // two ":" means the option can have param
  const char *opt_string = "hvs:E:b:t:";
  /* loop over arguments */
  while ((opt = getopt(argc, argv, opt_string)) != -1) {
    /* determine which argument was found */
    switch (opt) {
      case 'h':
        hflag = 1;
        break;
      case 'v':
        vflag = 1;
        break;
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        tflag = optarg;
        break;
      default:
        printf("unknown argument");
        break;
    }
  }

  /*P2: dealing with user input, initialize the Set*/
  if (hflag) {
    printf(
        "Welcome using my cache lab simulatorm, friend! I am yewentao or Peter "
        "Ye in English, here are some command options which may help you.\n "
        "-h: Optional help flag that prints usage info\n "
        "-v: Optional verbose flag that displays trace info\n "
        "-s <s>: Number of set index bits (S = 2^s is the number of sets)\n "
        "-E <E>: Associativity (number of lines per set)\n "
        "-b <b>: Number of block bits (B = 2^b is the block size)\n "
        "-t <tracefile>: Name of the valgrind trace to replay\n ");
    return 0;
  }
  unsigned int S = 1 << s;
  sets = (Set *)malloc(sizeof(Set) * S);
  for (int i = 0; i < S; i++) {
    Line *lines = (Line *)malloc(sizeof(Line) * E);
    sets[i].lines = lines;
  }

  /*P3: scan the file and process each line*/
  char access_type;
  unsigned long address;
  int size;

  FILE *fp;

  fp = fopen(tflag, "r");
  if (fp == NULL) {
    printf("Fail to open file %s! Please check the path you input", tflag);
    exit(0);
  }

  while (fscanf(fp, " %c %lx,%d", &access_type, &address, &size) > 0) {
    strcpy(access_log, "");  // clear the log string
    switch (access_type) {
      case 'L':
        access_cache(address);
        break;
      case 'M':
        access_cache(address);
        access_cache(address);
        break;
      case 'S':
        access_cache(address);
        break;
      default:
        break;
    }
    if (vflag && (access_type != 'I')) {
      printf("%c %lx,%d %s\n", access_type, address, size, access_log);
    }
  }

  /* P4: print the result*/
  printSummary(hit_count, miss_count, eviction_count);

  /*P5: free all of the memory we malloc*/
  for (int i = 0; i < S; i++) {
    free(sets[i].lines);
  }

  free(sets);
  fclose(fp);
  return 0;
}
