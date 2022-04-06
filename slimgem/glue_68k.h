#ifdef __GNUC__
SHL void vdi(void){
	__asm__ __volatile__(
		"move.l %0,d1\n\t"
		"move.w #115,d0\n\t"
		"trap #2\n\t"
		:: "g"(&pblock) :
		"d0","d1","d2","a0","a1","a2",
		"memory","cc");
}
SHL void aes(void){
	__asm__ __volatile__(
		"move.l %0,d1\n\t"
		"move.w #200,d0\n\t"
		"trap #2\n\t"
		:: "g"(&aespb) :
		"d0","d1","d2","a0","a1","a2",
		"memory","cc");
}
#endif
