#include <stdint.h>
#include <stdbool.h>

#ifdef __GNUC__ // shut up gcc
#define SHL static __attribute__((__unused__))
#else
#define SHL static
#endif

typedef struct{
	int16_t x;
	int16_t y;
} PXY;

// handy dandy int16x4 copy, using this saves some code size
#if defined(__GNUC__) && defined(__OPTIMIZE_SIZE__)
__attribute__ ((noinline)) // no gcc don't inline it
#endif
SHL void copy_i16x4(const void* src,void* dst){
	int32_t* s=(int32_t*)src;
	int32_t* d=(int32_t*)dst;
	int32_t t0,t1;
	t0=s[0];
	t1=s[1];
	d[0]=t0;
	d[1]=t1;
}

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
#define _app 1
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
