#include <i86.h>

#ifdef __WATCOMC__

static void vdi_asm(unsigned int,unsigned int);
#pragma aux vdi_asm = \
"mov cx,0x473 "\
"int 0xef "\
modify [ ax bx cx ] \
parm [ ds ] [ dx ];
SHL void vdi(void){vdi_asm(FP_SEG(&pblock),FP_OFF(&pblock));}

static void aes_asm(unsigned int,unsigned int);
#pragma aux aes_asm = \
"mov cx,200 "\
"int 0xef "\
modify [ ax cx dx ] \
parm [ es ] [ bx ];
SHL void aes(void){aes_asm(FP_SEG(&aespb),FP_OFF(&aespb));}

#else // vaguely portable code?
SHL void vdi(void){
	union REGS r;
	struct SREGS s;

	s.ds   = FP_SEG(&pblock);
	r.x.dx = FP_OFF(&pblock);
	r.x.cx = 0x473;

	int86x(0xef,&r,&r,&s);
}
SHL void aes(void){
	union REGS r;
	struct SREGS s;

	s.es   = FP_SEG(&aespb);
	r.x.bx = FP_OFF(&aespb);
	r.x.cx = 200;
	r.x.dx = 0;

	int86x(0xef,&r,&r,&s);
}
#endif
