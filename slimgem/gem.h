#include <stdint.h>

#ifdef __GNUC__ // shut up gcc
#define SHL static __attribute__((__unused__))
#else
#define SHL static
#endif

typedef struct{
	int16_t x;
	int16_t y;
} PXY;

#include "aes.h"
#include "vdi.h"

#ifdef __m68k__
#include "glue_68k.h"
#endif
#ifdef _M_I86
#include "glue_x86.h"
#endif

#ifdef __MINT__
extern int16_t _app;
#else
static int16_t _app=1;
#endif

// Misc utils

// where can we find min/max
#ifdef __MINT__
#include <macros.h>
#else
#include <stdlib.h>
#endif

SHL int16_t rc_intersect(GRECT* a,GRECT* b){
	int16_t x1=max(a->x,b->x);
	int16_t y1=max(a->y,b->y);
	int16_t x2=min(a->x+a->w,b->x+b->w);
	int16_t y2=min(a->y+a->h,b->y+b->h);
	b->x=x1;
	b->y=y1;
	b->w=x2-x1;
	b->h=y2-y1;
	return(int16_t)((x2>x1)&&(y2>y1));
}
