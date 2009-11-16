/*
 * memory - memory driver for heap based memory allocator
 *
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Christopher Hall <hsw@openmoko.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "standard.h"

#include <string.h>

#include "serial.h"
#include "memory.h"


#define PAGE_SIZE 256
#define PAGE_MASK (PAGE_SIZE - 1)

#define MAGIC_0 0x57524d45
#define MAGIC_1 0x4d424c4b

typedef enum {
	STATUS_free = 0,
	STATUS_allocated,
} StatusType;

typedef struct AllocationHeaderStruct AllocationHeaderType;

// this must be PAGE_SIZE bytes
struct AllocationHeaderStruct {
	uint8_t PostData[64];            // 64 protection against x[N] access

	uint32_t magic[2];               //  8
	AllocationHeaderType *next;      //  4
	AllocationHeaderType *previous;  //  4
	StatusType status;               //  4
	uint32_t AllocatedPages;         //  4
	char AllocatedBy[16];            // 16
	char FreedBy[16];                // 16
	size_t ByteSize;                 //  4
	uint32_t spare[1];               //  4  => 64

	uint8_t PreData[64];             // 64 protection agains x[-1] access
	uint8_t UserData[64];            // 64 start of data
};


static bool HaveMemory;
static uint32_t FirstPageAddress;
static uint32_t TotalPages;


void DisplayHeap(const char *format, ...) __attribute__((format (printf, 1, 2)));
void ConsolidateHeap(AllocationHeaderType *h);


void Memory_initialise(void)
{
	static bool initialised = false;
	if (!initialised) {
		// compile/link time check of thje structure size
		if (PAGE_SIZE != sizeof(AllocationHeaderType)) {
			void AllocationHeaderType_is_the_wrong_size(void);
			AllocationHeaderType_is_the_wrong_size();
		}
		HaveMemory = false;
		FirstPageAddress = 0;
		TotalPages = 0;
	}
}


void Memory_FreeAll(void)
{
	if (!HaveMemory) {
		return;
	}

	AllocationHeaderType *h = (AllocationHeaderType *)FirstPageAddress;
	memset(h, 0, sizeof(*h));
	h->magic[0] = MAGIC_0;
	h->magic[1] = MAGIC_1;
	h->AllocatedPages = TotalPages;  // includes page for header
	strncpy(h->AllocatedBy, "*SYSTEM*", sizeof(h->AllocatedBy));
}


void Memory_SetHeap(uint32_t FirstFreeAddress, uint32_t LastFreeAddress)
{

	FirstPageAddress = (FirstFreeAddress + PAGE_MASK) & ~PAGE_MASK; // round up to page size
	TotalPages = ((LastFreeAddress & ~PAGE_MASK) -                  // round down to page size
		      FirstPageAddress) / PAGE_SIZE;                    // convert to number of pages

	HaveMemory = TotalPages > 0;

	if (HaveMemory) {
		Memory_FreeAll();
	}
}


void *Memory_allocate(size_t size, const char *tag)
{
	if (!HaveMemory) {
		return NULL;
	}

	AllocationHeaderType *h = (AllocationHeaderType *)FirstPageAddress;
	uint32_t RequiredPages = 1;
	if (size > sizeof(h->UserData)) {
		RequiredPages = 1 + ((size - sizeof(h->UserData)) + PAGE_MASK) / PAGE_SIZE;
	}

	for (; NULL != h; h = h->next) {
		if (STATUS_free == h->status) {
			if (MAGIC_0 != h->magic[0] ||
			    MAGIC_0 != h->magic[0]) {
				DisplayHeap("Heap is corrupted: %s", tag);
				break;
			}

			ConsolidateHeap(h);  // combine all following free blocks
			if (h->AllocatedPages == RequiredPages) {
				h->status = STATUS_allocated;
				h->ByteSize = size;
				strncpy(h->AllocatedBy, tag, sizeof(h->AllocatedBy));
				return &(h->UserData);
			}
			if (h->AllocatedPages > RequiredPages) {
				AllocationHeaderType *NewHeader = &h[RequiredPages];

				memset(NewHeader, 0, sizeof(*NewHeader));
				NewHeader->magic[0] = MAGIC_0;
				NewHeader->magic[1] = MAGIC_1;
				strncpy(NewHeader->AllocatedBy, "*SYSTEM*", sizeof(h->AllocatedBy));
				NewHeader->AllocatedPages = h->AllocatedPages - RequiredPages;
				NewHeader->next = h->next;
				NewHeader->previous = h;

				h->next = NewHeader;
				h->status = STATUS_allocated;
				h->AllocatedPages = RequiredPages;
				h->ByteSize = size;
				strncpy(h->AllocatedBy, tag, sizeof(h->AllocatedBy));
				return &(h->UserData);
			}
		}
	}
	return NULL;
}


void Memory_free(void *address, const char *tag)
{
	if (!HaveMemory) {
		return;
	}

	AllocationHeaderType *h = (AllocationHeaderType *)((uint32_t)(address) & ~PAGE_MASK);

	if (MAGIC_0 != h->magic[0] ||
	    MAGIC_0 != h->magic[0]) {
		DisplayHeap("freeing: %p non-allocated memory: %s\n", address, tag);
	}

	if (STATUS_free == h->status) {
		DisplayHeap("freeing: %p already freed memory: %s\n", address, tag);
	} else {
		h->status = STATUS_free;
		strncpy(h->FreedBy, tag, sizeof(h->FreedBy));
	}
}


void Memory_debug(const char *message)
{
	DisplayHeap("\nMemory Debug: %s\n", message);
}


void DisplayHeap(const char *format, ...)
{
	va_list arguments;

	va_start(arguments, format);

	Serial_vuprintf(format, arguments);

	va_end(arguments);

	if (!HaveMemory) {
		Serial_print("ERROR: Heap is not set up\n\n");
		return;
	}

	Serial_print("\nAllocation List:\n\n");

	AllocationHeaderType *h = (AllocationHeaderType *)FirstPageAddress;
	uint32_t AllocatedPages = 0;
	uint32_t FreePages = 0;
	uint32_t ReservedPages = 0;

	for (; NULL != h; h = h->next) {
		if (MAGIC_0 != h->magic[0] ||
		    MAGIC_0 != h->magic[0]) {
			Serial_printf("%p: corrupted entry: magic={0x%08lx, 0x%08lx}\n", h, h->magic[0], h->magic[1]);
			break;
		}

		char ta[sizeof(h->AllocatedBy)];
		strncpy(ta, h->AllocatedBy, sizeof(h->AllocatedBy));
		ta[sizeof(ta) - 1] = '\0';

		char tf[sizeof(h->FreedBy)];
		strncpy(tf, h->FreedBy, sizeof(h->FreedBy));
		tf[sizeof(tf) - 1] = '\0';

		Serial_printf("%p: %s pages=%ld, allocated by: '%s', freed by: '%s'  requested = %d bytes\n",
			      h, h->status == STATUS_allocated ? "allocated" : "free",
			      h->AllocatedPages, ta, tf, h->ByteSize);
		switch (h->status) {
		case STATUS_free:
			FreePages += h->AllocatedPages;
			break;

		case STATUS_allocated:
			AllocatedPages += h->AllocatedPages;
			break;

		default:
			ReservedPages += h->AllocatedPages;
			break;
		}
	}
	Serial_PutChar('\n');
	Serial_print("\nHeap Summary:\n\n");
	Serial_printf("Start address      = 0x%08lx\n", FirstPageAddress);
	Serial_printf("Total pages        = %ld\n", TotalPages);
	Serial_printf("Free pages         = %ld\n", FreePages);
	Serial_printf("Allocated pages    = %ld\n", AllocatedPages);
	Serial_printf("Reserved pages     = %ld\n", ReservedPages);
	Serial_printf("Summed Total Pages = %ld\n", FreePages + AllocatedPages + ReservedPages);
}


void ConsolidateHeap(AllocationHeaderType *h)
{
	AllocationHeaderType *hConsolidate = h;

	for (h = h->next; NULL != h && STATUS_free == h->status; h = h->next) {
		hConsolidate->AllocatedPages += h->AllocatedPages;
	}
	hConsolidate->next = h;
	if (NULL != h) {
		h->previous = hConsolidate;
	}
}
