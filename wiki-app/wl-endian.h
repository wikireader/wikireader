#ifndef WL_ENDIAN_H
#define WL_ENDIAN_H

#if BYTE_ORDER == LITTLE_ENDIAN
	#define WL_LTONL(val)	(val)
	#define WL_LTONS(val)	(val)
#else
	#define WL_LTOBL(val)	(((((val) >> 24) & 0xff) << 0)  |	\
				 ((((val) >> 16) & 0xff) << 8)  |	\
				 ((((val) >> 8)  & 0xff) << 16) |	\
				 ((((val) >> 0)  & 0xff) << 24))
	#define WL_LTOBL(val)	(((((val) >> 8)  & 0xff) << 0)  | 	\
				 ((((val) >> 0)  & 0xff) << 8))
#endif

#endif /* WL_ENDIAN_H */

