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

extern short _app;

// Misc utils

SHL int16_t rc_intersect(GRECT* a,GRECT* b){
	int16_t min(int16_t a,int16_t b){return (a<b)?a:b;}
	int16_t max(int16_t a,int16_t b){return (a>b)?a:b;}
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
