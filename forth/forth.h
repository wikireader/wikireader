// interface to forth

#if !defined(_FORTH_H_)
#define _FORTH_H_ 1

// the 32 bit forth cell
typedef unsigned long Forth_CellType;

// must be 32 bits like Forth_CellType
typedef void *Forth_PointerType;

// this allows a return of two cells to the interpreter
typedef struct {
	Forth_CellType result;
	Forth_CellType rc;
} Forth_ReturnType;


#endif
