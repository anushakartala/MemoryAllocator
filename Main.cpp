#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <unistd.h>

void easytest(BuddyAllocator* ba){
  // be creative here
  // know what to expect after every allocation/deallocation cycle
  // here are a few examples
  ba->printlist();
  // allocating a byte
  char* mem =  (char*)ba->alloc (1);
  ba->printlist ();
  ba->free (mem); // give back the memory you just allocated
  ba->printlist(); // shouldn't the list now look like as in the beginning



}

int main(int argc, char ** argv) {

 // int basic_block_size = 128, memory_length = 512*256 ; //* 1024;
	//int basic_block_size = 64, memory_length = 512 ; //* 1024;
  int b = 128;
  	int s = 512*2;
  	int c = 0;

  	while((c = getopt(argc, argv, "b:s:")) != -1){
  		switch(c){
  			case 'b':
  				//cout<<"here in b"<<optarg<<endl;
  				b = atoi(optarg);
  				if(b < 32)
  					b = 32;
  				break;
  			case 's':
  				s = atoi(optarg);
  				break;
  		}
  	}

  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(b, s);

  // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
  easytest (allocator);


  // stress-test the memory manager, do this only after you are done with small test cases
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test.



  // destroy memory manager
  delete allocator;
}



