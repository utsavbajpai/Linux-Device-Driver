////////////////////////////////////////////////////////////////////////////////
//
//  File           : cart_cache.c
//  Description    : This is the implementation of the cache for the CART
//                   driver.
//
//  Author         : [** YOUR NAME **]
//  Last Modified  : [** YOUR DATE **]
//

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Project includes
#include "cmpsc311_log.h"
#include <cart_controller.h>

// Defines

#define CACHE_NO_FRAME (CART_MAX_CARTRIDGES * CART_CARTRIDGE_SIZE)

typedef struct lruRecord{
	struct lruRecord *next;
	struct lruRecord *prev;
	uint32_t frameAddress;
	char frame[CART_FRAME_SIZE];
} CacheRecord;
//Globals
CacheRecord *head;
CacheRecord *tail;
int LruListSize;
uint32_t MaxCacheFrames;
CacheRecord *Cache;
//
// Functions

static int remove_from_lru(CacheRecord *record)
{
	if ((record->next != NULL) || (record->prev != NULL))
	{
		// take record out of lru if there
		if (record->prev != NULL)
		{
			record->prev->next = record->next;
		}
		else
		{
			head = record->next;
		}
	
		if (record->next != NULL)
		{
			record->next->prev = record->prev;
		}
		else
		{
			tail = record->prev;
		}
		record->next = NULL;
		record->prev = NULL;
	}

	return 0;

}

static int put_at_lru_head(CacheRecord *record)
{
	if (head == record)
		return 0;

	remove_from_lru(record);
	
	if (head == NULL)
	{
		head = record;
		tail = record;
	}
	else
	{
		head->prev = record;
		record->next = head;
		head = record;
	}

	return 0;
} 

////////////////////////////////////////////////////////////////////////////////
//
// Function     : set_cart_cache_size
// Description  : Set the size of the cache (must be called before init)
//
// Inputs       : max_frames - the maximum number of items your cache can hold
// Outputs      : 0 if successful, -1 if failure

int set_cart_cache_size(uint32_t max_frames) {

	MaxCacheFrames = max_frames;
	Cache = malloc(MaxCacheFrames * sizeof(CacheRecord));

	if(Cache == NULL)
		return -1;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : init_cart_cache
// Description  : Initialize the cache and note maximum frames
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int init_cart_cache(void) {
	int i;

	head = NULL;
	tail = NULL;
	LruListSize = 0;

	for(i = 0; i < MaxCacheFrames; i++) {
		Cache[i].frameAddress = CACHE_NO_FRAME; 
		Cache[i].next = NULL;
		Cache[i].prev = NULL;
	}
	return 0;

}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : close_cart_cache
// Description  : Clear all of the contents of the cache, cleanup
//
// Inputs       : none
// Outputs      : o if successful, -1 if failure

int close_cart_cache(void) {

	free(Cache);
	return 0;

}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : put_cart_cache
// Description  : Put an object into the frame cache
//
// Inputs       : cart - the cartridge number of the frame to cache
//                frm - the frame number of the frame to cache
//                buf - the buffer to insert into the cache
// Outputs      : 0 if successful, -1 if failure

int put_cart_cache(CartridgeIndex cart, CartFrameIndex frm, void *buf)  {

	uint32_t frameAddress = cart * frm;
	if (frameAddress >= CACHE_NO_FRAME)
		return -1;
	int idx = frameAddress % MaxCacheFrames;

	//copy buf to cache
	Cache[idx].frameAddress = frameAddress; 
	memcpy(Cache[idx].frame, buf, CART_FRAME_SIZE);

	put_at_lru_head(&Cache[idx]);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : get_cart_cache
// Description  : Get an frame from the cache (and return it)
//
// Inputs       : cart - the cartridge number of the cartridge to find
//                frm - the  number of the frame to find
// Outputs      : pointer to cached frame or NULL if not found

void * get_cart_cache(CartridgeIndex cart, CartFrameIndex frm) {

	uint32_t frameAddress = cart * frm;
	if (frameAddress >= CACHE_NO_FRAME)
		return NULL;
	int idx = frameAddress % MaxCacheFrames;
	if (Cache[idx].frameAddress == frameAddress)
	{
		put_at_lru_head(&Cache[idx]);
		return Cache[idx].frame;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function     : delete_cart_cache
// Description  : Remove a frame from the cache (and return it)
//
// Inputs       : cart - the cart number of the frame to remove from cache
//                blk - the frame number of the frame to remove from cache
// Outputs      : pointe buffer inserted into the object

void * delete_cart_cache(CartridgeIndex cart, CartFrameIndex frm) {

	uint32_t frameAddress = cart * frm;
	if (frameAddress >= CACHE_NO_FRAME)
		return NULL;
	int idx = frameAddress % MaxCacheFrames;
	if (Cache[idx].frameAddress == frameAddress)
	{
		remove_from_lru(&Cache[idx]);
		Cache[idx].frameAddress = CACHE_NO_FRAME;
		return Cache[idx].frame;
	}

	return NULL;
}

//
// Unit test

////////////////////////////////////////////////////////////////////////////////
//
// Function     : cartCacheUnitTest
// Description  : Run a UNIT test checking the cache implementation
//
// Inputs       : none
// Outputs      : 0 if successful, -1 if failure

int cartCacheUnitTest(void) {

	// Return successfully
	logMessage(LOG_OUTPUT_LEVEL, "Cache unit test completed successfully.");
	return(0);
}
