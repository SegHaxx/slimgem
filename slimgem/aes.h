typedef struct{
	int16_t x,y,w,h;
} GRECT;

typedef struct{
	 int16_t ob_next;
	 int16_t ob_head;
	 int16_t ob_tail;
	uint16_t ob_type;
	uint16_t ob_flags;
	uint16_t ob_state;
	   void* ob_spec;
	   GRECT ob;
} OBJECT;

typedef struct{
	int16_t* cb_pcontrol;
	int16_t* cb_pglobal;
	int16_t* cb_pintin;
	int16_t* cb_pintout;
	   void* cb_padrin;
	   void* cb_padrout;
} AESPB;

AESPB aespb;
void* addr_in[8];
void* addr_out[2];
int16_t control[5];
int16_t int_in[16];
int16_t int_out[10];
int16_t aes_global[15];

int16_t gl_apid;

static void aes(void){
	__asm__ __volatile__(
		"move.l %0,d1\n\t"
		"move.w #200,d0\n\t"
		"trap #2\n\t"
		:: "g"(&aespb) :
		"d0","d1","d2","a0","a1","a2",
		"memory","cc");
}

static int16_t crys_if(
	int16_t opcode,
	int16_t num_int_in,
	int16_t num_int_out,
	int16_t num_addr_in)
{
	control[0]=opcode;
	control[1]=num_int_in;
	control[2]=num_int_out;
	control[3]=num_addr_in;
	aes();
	return(int_out[0]);
}

// Application Library

SHL int16_t appl_init(void){
	aespb.cb_pcontrol=control;
	aespb.cb_pglobal =aes_global;
	aespb.cb_pintin  =int_in;
	aespb.cb_pintout =int_out;
	aespb.cb_padrin  =addr_in;
	aespb.cb_padrout =addr_out;
	control[4]=0;
	gl_apid=crys_if(10,0,1,0);
	return gl_apid;
}

SHL int16_t appl_write(
	int16_t ap_wid,
	int16_t ap_wlength,
	void* const ap_wpbuff)
{
	int_in[0] =ap_wid;
	int_in[1] =ap_wlength;
	addr_in[0]=ap_wpbuff;
	return(crys_if(12,2,1,1));
}

SHL int16_t appl_exit(void){
	return(crys_if(19,0,1,0));
}

// Menu Library

#define MN_SELECTED 10

SHL int16_t menu_icheck(
	OBJECT* const me_ctree,
	int16_t me_citem,
	int16_t me_ccheck)
{
	int_in[0]  = me_citem;
	int_in[1]  = me_ccheck;
	addr_in[0] = me_ctree;
	return crys_if(31,2,1,1);
}

// Event Library

SHL int16_t evnt_timer(uint32_t ev_msec){
	uint16_t* msec=(uint16_t*)&ev_msec;
	int_in[0]=msec[1];
	int_in[1]=msec[0];
	return crys_if(24,2,1,0);
}

typedef struct {
	int16_t emi_flags;
	int16_t emi_bclicks;
	int16_t emi_bmask;
	int16_t emi_bstate;
	int16_t emi_m1leave;
	  GRECT emi_m1;
	int16_t emi_m2leave;
	  GRECT emi_m2;
	int16_t emi_tlow;
	int16_t emi_thigh;
} EVMULT_IN;

typedef struct {
	int16_t emo_events;
	    PXY emo_mouse;
	int16_t emo_mbutton;
	int16_t emo_kmeta;
	int16_t emo_kreturn;
	int16_t emo_mclicks;
} EVMULT_OUT;

// evnt_multi flags
#define MU_KEYBD	0x01
#define MU_BUTTON	0x02
#define MU_M1		0x04
#define MU_M2		0x08
#define MU_MESAG	0x10
#define MU_TIMER	0x20

#define WM_REDRAW 20
#define WM_TOPPED 21
#define WM_CLOSED 22
#define WM_FULLED 23
#define WM_SIZED  27
#define WM_MOVED  28
#define WM_NEWTOP 29
#define AC_OPEN   40
#define AC_CLOSE  41

SHL int16_t evnt_multi_fast(
	const EVMULT_IN* const em_in,
	int16_t msg[],
	const EVMULT_OUT* em_out)
{
	int16_t* prev_intin =aespb.cb_pintin;
	int16_t* prev_intout=aespb.cb_pintout;
	aespb.cb_pintin =(int16_t*)em_in;
	aespb.cb_pintout=(int16_t*)em_out;
	addr_in[0]=msg;
	crys_if(25,16,7,1);
	aespb.cb_pintin =prev_intin;
	aespb.cb_pintout=prev_intout;
	return em_out->emo_events;
}

// Menu Library

#define MENU_INSTALL 1

SHL int16_t menu_bar(OBJECT* const me_btree,int16_t me_bshow){
	int_in[0] =me_bshow;
	addr_in[0]=me_btree;
	return crys_if(30,1,1,1);
}	

SHL int16_t menu_tnormal(
	OBJECT* me_ntree,
	int16_t me_ntitle,
	int16_t me_nnormal)
{
	addr_in[0]=me_ntree;
	int_in[0] =me_ntitle;
	int_in[1] =me_nnormal;
	return crys_if(33,2,1,1);
}

SHL int16_t menu_register(int16_t me_rapid,const char* const me_rpstring){
	int_in[0]=me_rapid;
	addr_in[0]=(void*)me_rpstring;
	return crys_if(35,1,1,1);
}

// Object Library

SHL int16_t objc_draw(
	OBJECT* tree,
	int16_t start,
	int16_t depth,
	const GRECT* clip)
{
	addr_in[0]=tree;
	int_in[0]=start;
	int_in[1]=depth;
	int_in[2]=clip->x;
	int_in[3]=clip->y;
	int_in[4]=clip->w;
	int_in[5]=clip->h;
	return crys_if(42,6,1,1);
}

// Form Library

SHL int16_t form_do(OBJECT* fo_dotree,int16_t fo_dostartob){
	int_in[0]=fo_dostartob;
	addr_in[0]=fo_dotree;
	return crys_if(50,1,1,1);
}

#define FMD_START 0
#define FMD_GROW 1
#define FMD_SHRINK 2
#define FMD_FINISH 3

SHL int16_t form_dial_grect(
	int16_t fo_diflag,
	const GRECT* const fo_dilittl,
	const GRECT* const fo_dibig)
{
	int_in[0]  = fo_diflag;
	if(fo_dilittl){
		int_in[1]  = fo_dilittl->x;
		int_in[2]  = fo_dilittl->y;
		int_in[3]  = fo_dilittl->w;
		int_in[4]  = fo_dilittl->h;
	}
	int_in[5]  = fo_dibig->x;
	int_in[6]  = fo_dibig->y;
	int_in[7]  = fo_dibig->w;
	int_in[8]  = fo_dibig->h;
	return crys_if(51,9,1,1);
}

SHL int16_t form_alert(int16_t fo_adefbttn,const char* const fo_astring){
	int_in[0] =fo_adefbttn;
	addr_in[0]=(char*)fo_astring;
	return crys_if(52,1,1,1);
}

SHL int16_t form_center_grect(
	const OBJECT* const fo_ctree,
	GRECT* fo_c)
{
	addr_in[0]=(OBJECT*)fo_ctree;
	int16_t ret=crys_if(54,0,5,1);
	fo_c->x=int_out[1];
	fo_c->y=int_out[2];
	fo_c->w=int_out[3];
	fo_c->h=int_out[4];
	return(ret);
}

// Graphics Library

SHL int16_t graf_growbox(const GRECT* const start,const GRECT* const end){
	int_in[0] = start->x;
	int_in[1] = start->y;
	int_in[2] = start->w;
	int_in[3] = start->h;
	int_in[4] = end->x;
	int_in[5] = end->y;
	int_in[6] = end->w;
	int_in[7] = end->h;
	return crys_if(73,8,1,0);
}

SHL int16_t graf_shrinkbox(const GRECT* const start,const GRECT* const end){
	int_in[0] = start->x;
	int_in[1] = start->y;
	int_in[2] = start->w;
	int_in[3] = start->h;
	int_in[4] = end->x;
	int_in[5] = end->y;
	int_in[6] = end->w;
	int_in[7] = end->h;
	return crys_if(74,8,1,0);
}

SHL int16_t graf_handle(
	int16_t* gr_hwchar,
	int16_t* gr_hhchar,
	int16_t* gr_hwbox,
	int16_t* gr_hhbox)
{
	int16_t ret=crys_if(77,0,5,0);
	*gr_hwchar=int_out[1];
	*gr_hhchar=int_out[2];
	*gr_hwbox =int_out[3];
	*gr_hhbox =int_out[4];
	return ret;
}

typedef struct{
	int16_t mf_xhot;     // X-position hot-spot
	int16_t mf_yhot;     // Y-position hot-spot
	int16_t mf_nplanes;  // Number of planes
	int16_t mf_fg;       // Mask colour
	int16_t mf_bg;       // Pointer colour
	int16_t mf_mask[16]; // Mask form
	int16_t mf_data[16]; // Pointer form
} MFORM;

#define ARROW 0
#define USER_DEF 255
#define M_OFF 256
#define M_ON  257

SHL int16_t graf_mouse(int16_t gr_monumber){
	int_in[0]=gr_monumber;
	return crys_if(78,1,1,1);
}

SHL int16_t graf_mouse_set(MFORM* const gr_mofaddr){
	int_in[0]=USER_DEF;
	addr_in[0]=gr_mofaddr;
	return crys_if(78,1,1,1);
}

// Window Library

#define NAME		0x0001	/**< Window has a title bar */
#define CLOSER		0x0002	/**< Window has a close box */
#define FULLER		0x0004	/**< Window has a fuller box */
#define MOVER		0x0008	/**< Window may be moved by the user */
#define INFO		0x0010	/**< Window has an information line */
#define SIZER		0x0020	/**< Window has a sizer box */
#define UPARROW	0x0040	/**< Window has an up arrow */
#define DNARROW	0x0080	/**< Window has a down arrow */
#define VSLIDE		0x0100	/**< Window has a vertical slider */
#define LFARROW	0x0200	/**< Window has a left arrow */
#define RTARROW	0x0400	/**< Window has a right arrow */
#define HSLIDE		0x0800	/**< Window has a horizontal slider */
#define HOTCLOSEBOX	0x1000  /**< Window has "hot close box" box (GEM 2.x) */
#define MENUBAR	0x1000	/**< Window has a menu bar (XaAES) */
#define BACKDROP	0x2000	/**< Window has a backdrop box */
#define SMALLER	0x4000	/**< Window has an iconifier */
#define BORDER		0x8000	/**< Window has border-resizable capability (XaAES newer than Nov 8 2004) */
#define ICONIFIER	SMALLER

#define WC_BORDER 0
#define WC_WORK   1

SHL int16_t wind_create(int16_t wi_crkind,GRECT* gr){
	int_in[0]=wi_crkind;
	int_in[1]=gr->x;
	int_in[2]=gr->y;
	int_in[3]=gr->w;
	int_in[4]=gr->h;
	return crys_if(100,5,1,0);
}

SHL int16_t wind_open(int16_t handle,const GRECT* const gr){
	int_in[0]=handle;
	int_in[1]=gr->x;
	int_in[2]=gr->y;
	int_in[3]=gr->w;
	int_in[4]=gr->h;
	return crys_if(101,5,1,0);
}

SHL int16_t wind_close(int16_t wi_clhandle){
	int_in[0]=wi_clhandle;
	return crys_if(102,1,1,0);
}

SHL int16_t wind_delete(int16_t wi_dhandle){
	int_in[0]=wi_dhandle;
	return crys_if(103,1,1,0);
}

#define WF_KIND      1
#define WF_NAME      2
#define WF_INFO      3
#define WF_WORKXYWH  4
#define WF_CURRXYWH  5
#define WF_PREVXYWH  6
#define WF_FULLXYWH  7
#define WF_HSLIDE    8
#define WF_VSLIDE    9
#define WF_TOP       10
#define WF_FIRSTXYWH 11
#define WF_NEXTXYWH  12

SHL int16_t wind_get(
	int16_t handle,
	int16_t wi_gfield,
	int16_t* wi_gw1,
	int16_t* wi_gw2,
	int16_t* wi_gw3,
	int16_t* wi_gw4)
{
	int_in[0]=handle;
	int_in[1]=wi_gfield;
	int16_t ret=crys_if(104,2,5,0);
	*wi_gw1=int_out[1];
	*wi_gw2=int_out[2];
	*wi_gw3=int_out[3];
	*wi_gw4=int_out[4];
	return ret;
}

SHL int16_t wind_get_work(int16_t handle,GRECT* gr){
	int_in[0]=handle;
	int_in[1]=WF_WORKXYWH;
	int16_t ret=crys_if(104,2,5,0);
	gr->x=int_out[1];
	gr->y=int_out[2];
	gr->w=int_out[3];
	gr->h=int_out[4];
	return ret;
}

SHL int16_t wind_get_top(int16_t handle,int16_t* handle_top){
	int_in[0]=handle;
	int_in[1]=WF_TOP;
	int16_t ret=crys_if(104,2,5,0);
	handle_top=*(int16_t**)(int_in+2);
	return ret;
}

SHL int16_t wind_get_first(int16_t handle,GRECT* gr){
	int_in[0]=handle;
	int_in[1]=WF_FIRSTXYWH;
	int16_t ret=crys_if(104,2,5,0);
	gr->x=int_out[1];
	gr->y=int_out[2];
	gr->w=int_out[3];
	gr->h=int_out[4];
	return ret;
}

SHL int16_t wind_get_next(int16_t handle,GRECT* gr){
	int_in[0]=handle;
	int_in[1]=WF_NEXTXYWH;
	int16_t ret=crys_if(104,2,5,0);
	gr->x=int_out[1];
	gr->y=int_out[2];
	gr->w=int_out[3];
	gr->h=int_out[4];
	return ret;
}

SHL int16_t wind_set_name(int16_t handle,const char* const str){
	int_in[0]=handle;
	int_in[1]=WF_NAME;
	*(const char**)(int_in+2)=str;
	return crys_if(105,6,1,0);
}

SHL int16_t wind_set_current(int16_t handle,const GRECT* const gr){
	int_in[0]=handle;
	int_in[1]=WF_CURRXYWH;
	int_in[2]=gr->x;
	int_in[3]=gr->y;
	int_in[4]=gr->w;
	int_in[5]=gr->h;
	return crys_if(105,6,1,0);
}

SHL int16_t wind_set_top(int16_t wi_shandle){
	int_in[0]=wi_shandle;
	int_in[1]=WF_TOP;
	return crys_if(105,6,1,0);
}

SHL int16_t wind_update(int16_t wi_ubegend){
	int_in[0]=wi_ubegend;
	return crys_if(107,1,1,0);
}

SHL int16_t wind_calc(
	int16_t wi_ctype,
	int16_t wi_ckind,
	const GRECT* const in,
	GRECT* out)
{
	int_in[0]=wi_ctype;
	int_in[1]=wi_ckind;
	int_in[2]=in->x;
	int_in[3]=in->y;
	int_in[4]=in->w;
	int_in[5]=in->h;
	int16_t ret=crys_if(108,6,5,0);
	out->x=int_out[1];
	out->y=int_out[2];
	out->w=int_out[3];
	out->h=int_out[4];
	return ret;
}

// Resource Library

SHL int16_t rsrc_load(const char* const re_lpfname){
	addr_in[0]=(void*)re_lpfname;
	return crys_if(110,0,1,1);
}

#define R_TREE 0 // Object tree

SHL int16_t rsrc_gaddr(int16_t re_gtype,int16_t re_gindex,void* gaddr){
	int_in[0]=re_gtype;
	int_in[1]=re_gindex;
	//control[4]=1;
	int16_t ret=crys_if(112,2,1,0);
	//control[4]=0;
	*((void**)gaddr)=(void*)addr_out[0];
	return ret;
}
