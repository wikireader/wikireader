#include <stddef.h>
#include <stdbool.h>

#include "tff.h"


// 31-25: Year(0-127 +1980)
// 24-21: Month(1-12)
// 20-16: Day(1-31)
// 15-11: Hour(0-23)
// 10- 5: Minute(0-59)
//  4- 0: Second(0-29 *2)
#define YEAR   (2009 - 1980)
#define MONTH  3
#define DAY    30
#define HOUR   17
#define MINUTE 58
#define SECOND 41

DWORD get_fattime (void)
{
	return
		(YEAR	 << 25) |
		(MONTH	 << 21) |
		(DAY	 << 16) |
		(HOUR	 << 11) |
		(MINUTE	 <<  5) |
		(SECOND	 <<  0);
}
