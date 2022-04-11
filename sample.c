// Slimgem single window app/accessory example
// Copyright 2022 Seg <seg@haxxed.com>

#include <stdbool.h>
#include "slimgem/gem.h"

GRECT	desk,old,work;	/* desktop and work areas */
int16_t gl_wbox,gl_hbox;	/* system sizes */
int16_t acc_menu_id ;	/* our menu id */
int16_t wi_handle;	/* window handle */
int16_t fulled;		/* current state of window */

static const char app_name[]="  Slimgem Sample ";

// your drawing stuff goes here
static void draw(GRECT* w){
	vsf_interior(2);
	vsf_color(0);
	VRECT r;
	r.x1=w->x;
	r.y1=w->y;
	r.x2=w->x+w->w-1;
	r.y2=w->y+w->h-1;
	v_bar(&r);
	vsf_interior(4);
	vsf_style(8);
	vsf_color(1);
	r.x2=w->w/2;
	r.y2=w->h/2;
	r.x1=r.x2+w->x;
	r.y1=r.y2+w->y;
	v_ellipse((int16_t*)&r);
}

// walk the dirty rectangle list and redraw
static void window_do_redraw(GRECT* dirty){
	graf_mouse(M_OFF);
	wind_update(true);
	GRECT rect;
	wind_get_first(wi_handle,&rect);
	while (rect.w && rect.h) {
	  if (rc_intersect(dirty,&rect)) {
		 vs_clip_grect(1,&rect);
		 draw(&work);
	  }
	  wind_get_next(wi_handle,&rect);
	}
	wind_update(false);
	graf_mouse(M_ON);
}

// send ourself a redraw message
static void window_redraw(GRECT* gr){
	int16_t msg[8];
	msg[0]=WM_REDRAW;
	msg[1]=gl_apid;
	//msg[2]=0;
	msg[3]=wi_handle;
	msg[4]=gr->x;
	msg[5]=gr->y;
	msg[6]=gr->w;
	msg[7]=gr->h;
	appl_write(gl_apid,16,&msg);
}

#define WI_KIND (SIZER|MOVER|FULLER|CLOSER|NAME)
#define NO_WINDOW (-1)

static void window_open(){
	wi_handle=wind_create(WI_KIND,&desk);
	wind_set_name(wi_handle,app_name+1);
	GRECT start;
	start.x=desk.x+desk.w/2;
	start.y=desk.y+desk.h/2;
	start.w=gl_wbox;
	start.h=gl_hbox;
	graf_growbox(&start,&desk);
	wind_open(wi_handle,&desk);
	wind_get_work(wi_handle,&work);
}

static void window_close(){
	wind_close(wi_handle);
	GRECT end;
	end.x=work.x+work.w/2;
	end.y=work.y+work.h/2;
	end.w=gl_wbox;
	end.h=gl_hbox;
	graf_shrinkbox(&end,&work);
	wind_delete(wi_handle);
	wi_handle=NO_WINDOW;
	v_clsvwk();
}

#define MIN_WIDTH  (2*gl_wbox)
#define MIN_HEIGHT (3*gl_hbox)

static int window_handle_event(int16_t* msg){
	switch (msg[0]){
		case WM_REDRAW:
			window_do_redraw((GRECT*)&msg[4]);
			break;

		case WM_NEWTOP:
		case WM_TOPPED:
			wind_set_top(wi_handle);
			break;

		case WM_CLOSED:
			window_close();
			return 1;

		case WM_SIZED:
			if(msg[6]<MIN_WIDTH)msg[6]=MIN_WIDTH;
			if(msg[7]<MIN_HEIGHT)msg[7]=MIN_HEIGHT;
			// AES tries to be smart but we need a full redraw
			window_redraw((GRECT*)&msg[4]);
		case WM_MOVED:
			wind_set_current(wi_handle,(GRECT*)&msg[4]);
			wind_get_work(wi_handle,&work);
			fulled=false;
			break;

		case WM_FULLED:
			if(fulled){
				wind_calc(WC_WORK,WI_KIND,&old,&work);
				wind_set_current(wi_handle,&old);
				window_redraw(&old); // force full redraw
			}else{
				wind_calc(WC_BORDER,WI_KIND,&work,&old);
				wind_calc(WC_WORK,  WI_KIND,&desk,&work);
				wind_set_current(wi_handle,&desk);
			}
			fulled^=true;
			break;
	}
	return 0;
}

static void open_vwork(){
	int16_t dummy;
	int16_t phys_handle=graf_handle(&dummy,&dummy,&gl_wbox,&gl_hbox);

	int16_t work_in[11];
	for(int16_t i=0;i<10;work_in[i++]=1);
	work_in[10]=2;
	int16_t work_out[57];
	v_opnvwk(work_in,work_out,phys_handle);
}

static void accessory_handle_event(int16_t* msg){
	switch (msg[0]) {
		case AC_OPEN:
			if(msg[4]==acc_menu_id){
				if(wi_handle == NO_WINDOW){
					open_vwork();
					window_open();
				}
				else wind_set_top(wi_handle);}
			break;
		case AC_CLOSE:
			if((msg[3]==acc_menu_id)&&(wi_handle != NO_WINDOW)){
				v_clsvwk();
				wi_handle = NO_WINDOW;
			}
			break;
	}
}

// only returns on user quit
static void handle_aes_events(){
	EVMULT_IN in={0};
	EVMULT_OUT out={0};
	in.emi_flags=MU_MESAG;
	while(1){
		int16_t msg[8];
		int16_t event=evnt_multi_fast(&in,msg,&out);
		//if(event & MU_MESAG){
			if(msg[3]==wi_handle)
				if(window_handle_event(msg))
					if(_app) return;

			if(!_app) accessory_handle_event(msg);
		//}
	}
}

int main(void){
	if(appl_init()<0) return 1;

	wind_get_work(0,&desk);
	wi_handle=NO_WINDOW;
	fulled=false;

	// libcmini sets _app if we are running as an application
	if(_app){
		// open the window now
		open_vwork();
		window_open();
		graf_mouse(ARROW);
	}else{
		// register in to the desk menu
		acc_menu_id=menu_register(gl_apid,app_name);
	}

	handle_aes_events();

	appl_exit();
}
