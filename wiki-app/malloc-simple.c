//
// Authors:	Marek Lindner <marek@openmoko.com>
//
//		This program is free software; you can redistribute it and/or
//		modify it under the terms of the GNU General Public License
//		as published by the Free Software Foundation; either version
//		3 of the License, or (at your option) any later version.
//

#include <stdlib.h>
#include <malloc-simple.h>
#include <wikilib.h>
#include <msg.h>
#include <string.h>

#ifndef __c33 // ugly hack but for now it works...

void malloc_init_simple(void) { }
void malloc_status_simple(void) { }

void *malloc_simple(uint32_t size, uint32_t tag)
{
	return malloc(size);
}

void free_simple(void *ptr, uint32_t tag)
{
	free(ptr);
}

#else // __c33


extern uint8_t __START_heap;
extern uint8_t __END_heap;

#define MEM_SIZE	(&__END_heap - &__START_heap)
#define RAM_START	(&__START_heap)

// page size should not be smaller than struct malloc_page
#define PAGE_SIZE 	(256)
#define MEMORY_DEBUG	1
#define MAGIC_NUMBER	0x12345678
#define NUM_PAGES	(MEM_SIZE / PAGE_SIZE)
#define CTRL_PAGE	1
#define GET_PAGE(curr_page, num_pages)	(((uint8_t *)(curr_page)) + ((num_pages) * PAGE_SIZE))


enum mem_status {
	MEM_UNUSED,
	MEM_INUSE
};


struct malloc_page
{
	uint32_t magicNumber;
	uint32_t filler[10];
	struct malloc_page *next;
	struct malloc_page *prev;
	uint32_t size;          // number of pages
	uint32_t alloc_tag;
	uint32_t free_tag;
	enum mem_status status;
};


struct malloc_page *first_page = (struct malloc_page *)RAM_START;


static void init_page(struct malloc_page *page)
{
	ASSERT(page);
	page->magicNumber = MAGIC_NUMBER;
	page->prev = NULL;
	page->next = NULL;
	page->size = 0;
	page->status = MEM_UNUSED;
	page->alloc_tag = MEM_TAG_UNUSED;
	page->free_tag = MEM_TAG_UNUSED;
}


static struct malloc_page *page_new(struct malloc_page *curr_page, uint32_t needed_pages)
{
	struct malloc_page *new_page = (struct malloc_page *)GET_PAGE(curr_page, needed_pages + 1);

	init_page(new_page);
	ASSERT(curr_page);
	ASSERT(curr_page->magicNumber == MAGIC_NUMBER);

	curr_page->next = new_page;
	new_page->prev = curr_page;

	// one more for the malloc control page
	new_page->size = curr_page->size - needed_pages - CTRL_PAGE;
	curr_page->size = needed_pages;

	return new_page;
}


void malloc_init_simple(void)
{
	memset(first_page, 0, sizeof(struct malloc_page));

	init_page(first_page);
	first_page->size = NUM_PAGES;
}


void *malloc_simple(uint32_t size, uint32_t tag)
{
	struct malloc_page *curr_page, *new_page;
	uint32_t needed_pages = size / PAGE_SIZE;

	if (size % PAGE_SIZE) {
		needed_pages++;
	}

	for (curr_page = first_page; curr_page != NULL; curr_page = curr_page->next) {
		ASSERT(curr_page);
		ASSERT(curr_page->magicNumber == MAGIC_NUMBER);
		if (curr_page->status != MEM_UNUSED) {
			continue;
		}

		// free memory but not enough space for us due to memory fragmentation
		if ((curr_page->next != NULL) && (curr_page->size < needed_pages)) {
			continue;
		}

		// end of memory reached - out of memory
		if ((curr_page->next == NULL) && (curr_page->size < needed_pages + CTRL_PAGE)) {
			break;
		}

		// the memory lying ahead has no control structure - we need to create it
		if (curr_page->next == NULL) {
			new_page = page_new(curr_page, needed_pages);
		}

		curr_page->alloc_tag = tag;
		curr_page->free_tag = MEM_TAG_UNUSED;
		curr_page->status = MEM_INUSE;
		ASSERT(curr_page);
		ASSERT(curr_page->magicNumber == MAGIC_NUMBER);
		return (void *)GET_PAGE(curr_page, 1);
	}

	// out of memory
	return NULL;
}


void free_simple(void *ptr, uint32_t tag)
{
	struct malloc_page *my_page, *curr_page;

	if (!ptr) {
		return;
	}

	if ((uint8_t *)ptr < RAM_START + PAGE_SIZE){
		return;
	}

	my_page = (struct malloc_page *)GET_PAGE(ptr, -1);
	ASSERT(my_page);
	ASSERT(my_page->magicNumber == MAGIC_NUMBER);

	// double free
	if (my_page->status == MEM_UNUSED) {
#if MEMORY_DEBUG
		msg(MSG_INFO, "MEMORY DEBUG: possible double free detected - trying to free %p again\n",
		    ptr);
		msg(MSG_INFO, "MEMORY DEBUG: alloc tag: %d, old free tag: %d, curr free tag: %d)\n",
		    my_page->alloc_tag, my_page->free_tag, tag);
#endif
		return;
	}

	my_page->status = MEM_UNUSED;
	my_page->free_tag = tag;

	//msg(MSG_INFO, "freeing: alloc tag: %d, free tag %d\n", my_page->alloc_tag, tag);

	// we may be able to merge larger chunks of free space that follows the current chunk
	for (curr_page = my_page->next; curr_page != NULL; curr_page = curr_page->next) {
		ASSERT(curr_page->magicNumber == MAGIC_NUMBER);

		if (curr_page->status != MEM_UNUSED) {
			break;
		}

		my_page->size += curr_page->size + CTRL_PAGE;

		my_page->next = curr_page->next;
		if (curr_page->next) {
			ASSERT(curr_page->next->magicNumber == MAGIC_NUMBER);
			curr_page->next->prev = my_page;
		}
	}

	// merge free space that preceeds the current chunk
	for (curr_page = my_page->prev; curr_page != NULL; curr_page = curr_page->prev) {
		ASSERT(curr_page->magicNumber == MAGIC_NUMBER);

		if (curr_page->status != MEM_UNUSED) {
			break;
		}

		curr_page->size += curr_page->next->size + CTRL_PAGE;

		// save the tags from the item that is being deleted now
		curr_page->alloc_tag = curr_page->next->alloc_tag;
		curr_page->free_tag = curr_page->next->free_tag;

		ASSERT(curr_page->next);
		ASSERT(curr_page->next->magicNumber == MAGIC_NUMBER);
		curr_page->next = curr_page->next->next;
		curr_page->next->prev = curr_page;
	}
}

void malloc_status_simple(void)
{
	struct malloc_page *curr_page;
	uint32_t mem_inuse = 0, mem_frag = 0, malloc_pages = 0, free_mem = 0;

	msg(MSG_INFO, "\nmemory status:\n");

	for (curr_page = first_page; curr_page != NULL; curr_page = curr_page->next) {
		malloc_pages++;

		ASSERT(curr_page);
		ASSERT(curr_page->magicNumber == MAGIC_NUMBER);
		if (curr_page->status == MEM_INUSE) {
			mem_inuse += curr_page->size;
		}

		if ((curr_page->status == MEM_UNUSED) && (curr_page->next != NULL)) {
			mem_frag += curr_page->size;
		}

		free_mem = curr_page->size;

#if MEMORY_DEBUG
		if (curr_page->magicNumber != MAGIC_NUMBER) {
			msg(MSG_INFO,
			    "MEMORY DEBUG: invalid magic number (%08x - expected %08x) - alloc tag: %d, prev alloc tag: %d\n",
			    curr_page->magicNumber, MAGIC_NUMBER,
			    curr_page->alloc_tag,
			    (curr_page->prev != NULL ? curr_page->prev->alloc_tag : 0));
		}

		if (curr_page->status == MEM_INUSE) {
			msg(MSG_INFO, "MEMORY DEBUG: used mem - alloc tag: %d, size: %d\n",
			    curr_page->alloc_tag, curr_page->size * PAGE_SIZE);
		}
		// do not show end of memory page
		else if (curr_page->next != NULL) {
			msg(MSG_INFO, "MEMORY DEBUG: fragmented mem - alloc tag: %d, size: %d, free tag: %d\n",
			    curr_page->alloc_tag, curr_page->size * PAGE_SIZE, curr_page->free_tag);
		}
#endif
	}

	msg(MSG_INFO, " * memory allocated: %d\n * memory fragmented: %d\n",
	    mem_inuse * PAGE_SIZE, mem_frag * PAGE_SIZE);

	msg(MSG_INFO, " * num control pages: %d\n * free memory: %d\n * total memory: %d\n",
	    malloc_pages, free_mem * PAGE_SIZE, MEM_SIZE);
}

#endif
