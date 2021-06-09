For part1

How to compile: gcc part1.c
How to run : ./a.out BACKING_STORE.bin addresses.txt

In this part we assigned the values page , page_size ,offset and both mask variables to the given data in the pdf. In search_tlb function we iterate through tlb array to find if the logical page we are looking for is inside it. In add_tlb we add a new item to the tlb array. In the main function we first find offset and logical page using the logical address read from the file. Then we first search tlb array if we find the logical page we are looking exists. If it does not exist we check the page table (pagetable[]), to find the logical page. If we can't find it there either it means page fault exists.(Not in main memory) Then we assign empty frame to the physical address and map it to the logical address inside the page table. By using memcpy function we bring it into the main memory. Then we add the mapped addresses to tlb array for future access.


For part2

How to compile: gcc part2.c
How to run : ./a.out BACKING_STORE.bin addresses.txt -p 0      or     ./a.out BACKING_STORE.bin addresses.txt -p 1 
