typedef struct{
   int16_t x1,y1,x2,y2;
} VRECT;

typedef struct{
	int16_t* contrl; // Pointer to contrl array
	int16_t* intin;  // Pointer to intin array
	int16_t* ptsin;  // Pointer to ptsin array
	int16_t* intout; // Pointer to intout array
	int16_t* ptsout; // Pointer to ptsout array
} VDIPB;

VDIPB pblock;
int16_t contrl[12];
int16_t ptsin[4];
int16_t ptsout[1];
int16_t intin[11];
int16_t intout[1];

static void vdi(void);

SHL int16_t vdi_if(
	int16_t opcode,
	int16_t num_pts_in,
	int16_t num_int_in)
{
	contrl[0]=opcode;
	contrl[1]=num_pts_in;
	contrl[3]=num_int_in;
	vdi();
	return(ptsout[0]);
}

SHL void v_pline(int16_t count,int16_t* px){
	int16_t* prev=pblock.ptsin;
	pblock.ptsin=px;
	vdi_if(6,count,0);
	pblock.ptsin=prev;
}

SHL void v_pmarker(int16_t count,int16_t* px){
	int16_t* prev=pblock.ptsin;
	pblock.ptsin=px;
	vdi_if(7,count,0);
	pblock.ptsin=prev;
}

SHL void v_gtext(int16_t x,int16_t y,char* string){
	ptsin[0]=x;
	ptsin[1]=y;
	int16_t i=0;
	while((intin[i++]=*string++)){};
	vdi_if(8,1,i-1);
}

SHL void v_bar(VRECT* vr){
	int16_t* prev=pblock.ptsin;
	pblock.ptsin=(int16_t*)vr;
	contrl[5]=1;
	vdi_if(11,2,0);
	pblock.ptsin=prev;
}

SHL void v_ellipse(int16_t* px){
	int16_t* prev=pblock.ptsin;
	pblock.ptsin=px;
	contrl[5]=5;
	vdi_if(11,2,0);
	pblock.ptsin=prev;
}

// Line attributes

SHL int16_t vsl_type(int16_t style){
	intin[0] = style;
	return vdi_if(15,0,1);
}

SHL int16_t vsl_width(int16_t width){
	ptsin[0] = width;
	ptsin[1] = 0;
	return vdi_if(16,1,0);
}

SHL int16_t vsl_color(int16_t color_index){
	intin[0]=color_index;
	return vdi_if(17,0,1);
}

SHL int16_t vsm_color(int16_t color_index){
	intin[0]=color_index;
	return vdi_if(20,0,1);
}

// Fill attributes

SHL int16_t vsf_interior(int16_t style){
	intin[0]=style;
	return vdi_if(23,0,1);
}

SHL int16_t vsf_style(int16_t style){
	intin[0]=style;
	return vdi_if(24,0,1);
}

SHL int16_t vsf_color(int16_t color){
	intin[0]=color;
	return vdi_if(25,0,1);
}

SHL int16_t vswr_mode(int16_t mode){
	intin[0]=mode;
	return vdi_if(32,0,1);
}

// Control

SHL void v_opnvwk(
	int16_t* work_in,
	int16_t* work_out,
	int16_t phys_hndl)
{
	pblock.contrl=contrl;
	pblock.intin =intin;
	pblock.ptsin =ptsin;

	int16_t l_intout[45];
	int16_t l_ptsout[12];
	pblock.intout=l_intout;
	pblock.ptsout=l_ptsout;

	for(int16_t i=0;i<11;++i){
		intin[i]=work_in[i];
	}
	contrl[6]=phys_hndl;
	vdi_if(100,0,11);
	for(int16_t i=0;i<45;++i){
		work_out[i]=l_intout[i];
	}
	for(int16_t i=0;i<12;++i){
		work_out[i+45]=l_ptsout[i];
	}
	pblock.intout=intout;
	pblock.ptsout=ptsout;
}

SHL void v_clsvwk(void){
	vdi_if(101,0,0);
}

SHL void vsl_ends(int16_t beg_style,int16_t end_style){
	intin[0]=beg_style;
	intin[1]=end_style;
	vdi_if(108,0,2);
}

SHL void vsl_udsty(int16_t pattern){
	intin[0] = pattern;
	vdi_if(113,0,1);
}

SHL void vr_recfl(int16_t* pxyarray){
	ptsin[0]=pxyarray[0];
	ptsin[1]=pxyarray[1];
	ptsin[2]=pxyarray[2];
	ptsin[3]=pxyarray[3];
	vdi_if(114,2,0);
}

SHL void v_show_c(int16_t reset){
	intin[0] = reset;
	vdi_if(122,0,1);
}

SHL void v_hide_c(void){
	vdi_if(123,0,0);
}

SHL void vs_clip(int16_t clip_flag,int16_t* px){
	intin[0]=clip_flag;
	ptsin[0]=px[0];
	ptsin[1]=px[1];
	ptsin[2]=px[2];
	ptsin[3]=px[3];
	vdi_if(129,2,1);
}

SHL void vs_clip_grect(int16_t clip_flag,GRECT* r){
	intin[0]=clip_flag;
	ptsin[0]=r->x;
	ptsin[1]=r->y;
	ptsin[2]=r->x+r->w-1;
	ptsin[3]=r->y+r->h-1;
	vdi_if(129,2,1);
}
