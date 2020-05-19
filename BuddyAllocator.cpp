#include "BuddyAllocator.h"

#include <iostream>
#include <math.h>
using namespace std;

BlockHeader::BlockHeader(){}

BuddyAllocator::BuddyAllocator (int _basic_block_size, int _total_memory_length)
{
  basic_block_size = _basic_block_size, total_memory_size = _total_memory_length;
  //int newBlockSize = basic_block_size;
  //int newMemorySize = total_memory_size;
  int min = log2(basic_block_size);
  int max = log2(total_memory_size);
  //BlockHeader* mem = (BlockHeader *)alloc(_total_memory_length);
  for(int i=0;i<=max-min;i++)
  {
	  if(i==max-min)
	  {
		  BlockHeader*  b = (BlockHeader *)malloc(total_memory_size);
		  this->starter = b;
	  	  b->isUsed = false;
	  	  b->block_size = pow(2, max);
	  	  b->next = NULL;
	  	  LinkedList l;
	  	  l.head = b;
	  	  FreeList.push_back(l);

	  }
	  else

	  {
		  LinkedList l;
		  l.head = NULL;
		  FreeList.push_back(l);

	  }
  }




}

BuddyAllocator::~BuddyAllocator (){
delete starter;
}

void* BuddyAllocator::alloc(int length)
{
  /* This preliminary implementation simply hands the call over the
     the C standard library!
     Of course this needs to be replaced by your implementation.
  */
  //return malloc (length);
	int size = length + 4;
	int index = ceil(log2(size) - log2(basic_block_size));
	if(index<0)
		index=0;
	//cout<<"first index ="<<index<<"size of   ="<<FreeList[index].head<<endl;

	for(int i=index;i<FreeList.size();i++)
	{
		if((FreeList[i].head!=NULL) && (i==index))
		{
				BlockHeader* temp = FreeList[i].head;

				FreeList[i].remove(temp);
					return temp;
		}
		else
		{

			if(FreeList[i].head!=NULL)
			{
				BlockHeader* splitBlock = split(FreeList[i].head);
				while(splitBlock->block_size>=basic_block_size*(index+1))
				{
					if(splitBlock->block_size==basic_block_size*(index+1))
					{
						//cout<<"in if =" <<splitBlock->block_size <<endl;
						FreeList[i].remove(splitBlock);
						break;

					}
					else
					{

						splitBlock = split(splitBlock);
					}
				}
				return splitBlock;
			}

		}


	}

}

void BuddyAllocator::free(void* a) {
  /* Same here! */
  //::free (a);
	bool merged=false;
	//cout<<"here"<<endl;
	//cout<<"here size"<<((BlockHeader*)a)->block_size<<endl;
	int index = ceil(log2(((BlockHeader*)a)->block_size) - log2(basic_block_size));
		if(index<0)
			index=0;
		//cout<<"in free initial index"<<index;

	if(FreeList[index].head==NULL)
	{
		((BlockHeader*)a)->isUsed = false;
		((BlockHeader*)a)->next = NULL;
		FreeList[index].insert((BlockHeader*)a, ((BlockHeader*)a)->block_size );
	}
	else
	{
	for(int i=index;i<FreeList.size();i++)
	{
		BlockHeader* buddyBH = getbuddy((BlockHeader*) a);
		if((buddyBH==NULL) && (merged==false))
			break;
		else if((buddyBH==NULL) && (merged==true))
		{

			int mergedIndex = ceil(log2(((BlockHeader*)a)->block_size) - log2(basic_block_size));
					if(mergedIndex<0)
						mergedIndex=0;
			FreeList[mergedIndex].insert((BlockHeader*) a, ((BlockHeader*)a)->block_size);
			break;
		}
		else
		{
			BlockHeader* mergedBH = merge((BlockHeader*)a, buddyBH);
			//cout<<"buddy size to be removed"<<buddyBH->block_size<<endl;
			int index = ceil(log2(buddyBH->block_size) - log2(basic_block_size));
							if(index<0)
								index=0;
			FreeList[index].remove(buddyBH);
			a = mergedBH;
			//cout<<"merged size actual"<<mergedBH->block_size<<endl;
			//cout<<"merged size"<<((BlockHeader*)a)->block_size<<endl;
			merged=true;
		}

	}
	}

}

BlockHeader* BuddyAllocator::getbuddy (BlockHeader * addr)
{
	//cout<<"here in getbuddy "<<addr->block_size<<endl;
	int index = ceil(log2(addr->block_size) - log2(basic_block_size));
		if(index<0)
			index=0;
	BlockHeader* buddy = ((addr-starter)^(addr->block_size))+starter;
	BlockHeader* b = FreeList[index].head;
	if(arebuddies(buddy, b))
		return buddy;
	else
		return NULL;
}


bool BuddyAllocator::arebuddies (BlockHeader* block1, BlockHeader* block2)
{
	if(block1==block2)
		return true;
	else
		return false;
}


BlockHeader* BuddyAllocator::merge (BlockHeader* block1, BlockHeader* block2)
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around
{
	if(block1<block2)
	{
		int mergedBlockSize = 2 * block1->block_size;
		block1->block_size = mergedBlockSize;
		return block1;

	}
	else
	{
		int mergedBlockSize = 2 * block2->block_size;
				block2->block_size = mergedBlockSize;
				return block2;
	}

}

BlockHeader* BuddyAllocator::split (BlockHeader* block)
{

	BlockHeader * newPiece = block+block->block_size/2;
	int newBlockSize = block->block_size/2;
	newPiece->isUsed = false;
	newPiece->next = NULL;
	int index = ceil(log2(block->block_size/2) - log2(basic_block_size));
	if(index<0)
			index=0;

	FreeList[index].insert(newPiece, newBlockSize);
	block->block_size = block->block_size/2;
	return block;


}

void BuddyAllocator::printlist (){
  cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  int64_t total_free_memory = 0;
  for (int i=0; i<FreeList.size(); i++)
  {
    int blocksize = ((1<<i) * basic_block_size); // all blocks at this level are this size
    cout << "[" << i <<"] (" << blocksize << ") : ";  // block size at index should always be 2^i * bbs
    int count = 0;
    BlockHeader* b = FreeList[i].head;

    // go through the list from head to tail and count
    while (b)
    {
      total_free_memory += blocksize;
      count ++;
      // block size at index should always be 2^i * bbs
      // checking to make sure that the block is not out of place
      if (b->block_size != blocksize){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        //exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;
  }
    cout << "Amount of available free memory: " << total_free_memory << " byes" << endl;

}
