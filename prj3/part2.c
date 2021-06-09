/**
 * part2.c 
 */
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGES 1024
#define PAGE_MASK 1023

#define FRAME 256
#define FRAME_SIZE 1024


#define PAGE_SIZE 1024
#define OFFSET_BITS 10
#define OFFSET_MASK 1023

//262.144
#define MEMORY_SIZE FRAME * FRAME_SIZE

// Max number of characters per line of input file to read.
#define BUFFER_SIZE 10

struct tlbentry {
  int logical;
  int physical;
};

// TLB is kept track of as a circular array, with the oldest element being overwritten once the TLB is full.
struct tlbentry tlb[TLB_SIZE];
// number of inserts into TLB that have been completed. Use as tlbindex % TLB_SIZE for the index of the next TLB line to use.
int tlbindex = 0;

// pagetable[logical_page] is the physical page number for logical page. Value is -1 if that logical page isn't yet in the table.
int pagetable[PAGES];
int physicalPageCount[FRAME];

signed char main_memory[MEMORY_SIZE];


// Pointer to memory mapped backing file
signed char *backing;

int max(int a, int b)
{
  if (a > b)
    return a;
  return b;
}

/* Returns the physical address from TLB or -1 if not present. */
int search_tlb(int logical_page) {
  /* TODO */
    int a = tlbindex;  
    while(a!=0){
      a--;
      struct tlbentry *item = &tlb[a % TLB_SIZE];
      if(item->logical == logical_page){
          return item->physical;
          break;
        }

    }
    return -1;
    
}

/* Adds the specified mapping to the TLB, replacing the oldest mapping (FIFO replacement). */
void add_to_tlb(int logical, int physical) {
    /* TODO */
  struct tlbentry *item = &tlb[tlbindex % TLB_SIZE];
  item->logical = logical;
  item->physical = physical;
  tlbindex++;
}

int findLastUsed(){
    int temp = 0;
    for(int i = 0; i<FRAME;i++){
      if(temp > physicalPageCount[i]){
        temp = physicalPageCount[i];
      }
    }
    return temp;
}



int main(int argc, const char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Usage ./virtmem backingstore input\n");
    exit(1);
  }
  
  const char *backing_filename = argv[1]; 
  int backing_fd = open(backing_filename, O_RDONLY);
  backing = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, backing_fd, 0); 
  
  const char *input_filename = argv[2];
  FILE *input_fp = fopen(input_filename, "r");

  const char *policy = argv[3];
  const char* tmp = "-p";
  int result = strcmp(policy,tmp);
  if(result != 0){
    printf("Usage ./virtmem backingstore input -p value");
    exit(1);

  }
  const char *argv4 = argv[4];
  int policyChoice = atoi(argv4);
  printf("%d\n",policyChoice);
  if(policyChoice != 0 && policyChoice !=1 ){
   printf("Usage ./virtmem backingstore input -p value\n");
   printf("Value must be zero or one\n");
   exit(1);
 }
  
  // Fill page table entries with -1 for initially empty table.
  int i;
  for (i = 0; i < PAGES; i++) {
    pagetable[i] = -1;
  }
  for (i = 0; i<FRAME; i++){
    physicalPageCount[i] = 0;
  }
  
  // Character buffer for reading lines of input file.
  char buffer[BUFFER_SIZE];
  
  // Data we need to keep track of to compute stats at end.
  int total_addresses = 0;
  int tlb_hits = 0;
  int page_faults = 0;
  
  // Number of the next unallocated physical page in main memory
  unsigned char free_page = 0;
  unsigned char free_frame = 255;
  
  while (fgets(buffer, BUFFER_SIZE, input_fp) != NULL) {
    total_addresses++;
    int logical_address = atoi(buffer);

    /* TODO 
    / Calculate the page offset and logical page number from logical_address */
    int offset = logical_address & OFFSET_MASK;
    int logical_page = (logical_address >> OFFSET_BITS) & PAGE_MASK;

    ///////
    
    int physical_page = search_tlb(logical_page);
    // TLB hit
    if (physical_page != -1) {
      tlb_hits++;
      physicalPageCount[physical_page] += 1;
      // TLB miss
    } else {
      physical_page = pagetable[logical_page];
      physicalPageCount[physical_page] += 1;
      
      // Page fault
      if (physical_page == -1) {
          /* TODO */
        /* First in first out policy free_pages are assigned in order when free page number is
          equal to frame number the memory is full. Since this first in first out page that will 
          come will replace first page that is in the memory.
        */ 
        if(policyChoice==0){
          if(free_page == FRAME-1){
           free_page = 0;
        }
          page_faults++;
          physical_page = free_page;
          free_page++;
          pagetable[logical_page] = physical_page;
          //memcpy(main_memory + physical_page * PAGE_SIZE, backing + logical_page * PAGE_SIZE, PAGE_SIZE);   
        }

        /* Least Recently Used */
        else if(policyChoice==1){
         if(free_page == FRAME-1){
          printf("BURASIIIIIIIASDJASODJAPDPASDASDPOOASJDPJASPDOJSAPDJPAOSJDPOAJSDPOJASPDJPSAJDPASJPD");
            page_faults++;
            physical_page = findLastUsed();
            printf("%d\n",physical_page);
            pagetable[logical_page] = physical_page;
        }
          page_faults++;
          physical_page = free_page;
          physicalPageCount[free_page] = 1;
          free_page++;
          pagetable[logical_page] = physical_page;
        }

      }
      add_to_tlb(logical_page, physical_page);
  }
    
    int physical_address = (physical_page << OFFSET_BITS) | offset;
    signed char value = main_memory[physical_page * PAGE_SIZE + offset];
    
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_address, physical_address, value);
  }


  
  printf("Number of Translated Addresses = %d\n", total_addresses);
  printf("Page Faults = %d\n", page_faults);
  printf("Page Fault Rate = %.3f\n", page_faults / (1. * total_addresses));
  printf("TLB Hits = %d\n", tlb_hits);
  printf("TLB Hit Rate = %.3f\n", tlb_hits / (1. * total_addresses));
  
  return 0;
}
