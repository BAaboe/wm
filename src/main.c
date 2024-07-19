#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

#include "config.h"

#define true 1
#define false 0



static Bool wm_deteced;

static WindowManager wm;

int OnXError(Display* display, XErrorEvent* e){
	printf("%u\n", e->error_code);
	return 0;
}

int OnWMDetected(Display* display, XErrorEvent* e){
	if((int)e->error_code == BadAccess){
		wm_deteced = true;
	}

	return 0;
}

void setup(){
	wm.dsp = XOpenDisplay(NULL);
	if (wm.dsp == NULL){
		exit(EXIT_FAILURE);
	}
	wm.root = DefaultRootWindow(wm.dsp);
}

void run(){
	wm_deteced = false;

	XSetErrorHandler(&OnWMDetected);
	XSelectInput(wm.dsp, wm.root, SubstructureRedirectMask | SubstructureNotifyMask);
	XSync(wm.dsp, false);

	if(wm_deteced){
		printf("Error: detected another window manager on display\n");
		return;
	}

	XSetErrorHandler(&OnXError);
	
	while(1){
		XEvent e;
		XNextEvent(wm.dsp, &e);
		
		switch(e.type){
			case CreateNotify:
				OnCreateNotify(&e.xcreatewindow);
				break;
			case ConfigureRequest:
				OnConfigRequest(&e.xconfigurerequest);
				break;
			case ConfigureNotify:
				OnConfigNotify(&e.xconfigure);
			case MapRequest:
				OnMapRequest(&e.xmaprequest);
				break;
			case MapNotify:
				OnMapNotify(&e.xmap);
				break;
		}
	}
}

int main(){
	setup();

	run();


	XCloseDisplay(wm.dsp);
	return 0;
}


void OnCreateNotify(const XCreateWindowEvent* e) {}


void OnConfigRequest(const XConfigureRequestEvent *e) {
	XWindowChanges changes;
	
	changes.x = e->x;
	changes.y = e->y;
	changes.width = e->width;
	changes.height = e->height;
	changes.sibling = e->above;
	changes.border_width = e->border_width;
	changes.stack_mode = e->detail;

	XConfigureWindow(wm.dsp, e->window, e->value_mask, &changes);
}

void OnConfigNotify(const XConfigureEvent *e) {}

void OnMapRequest(const XMapRequestEvent *e){
	Frame(e->window);

	XMapWindow(wm.dsp, e->window);
}

void OnMapNotify(const XMapEvent* e){}


void Frame(Window w){
	XWindowAttributes x_w_attrs;
	XGetWindowAttributes(wm.dsp, w, &x_w_attrs);

	const Window f = XCreateSimpleWindow(
			wm.dsp,
			wm.root,
			x_w_attrs.x,
			x_w_attrs.y,
			x_w_attrs.width,
			x_w_attrs.height,
			BORDER_WIDTH,
			BORDER_COLOR,
			BG_COLOR);

	
	XSelectInput(wm.dsp, f, SubstructureRedirectMask | SubstructureNotifyMask);

	XAddToSaveSet(wm.dsp, w);

	XReparentWindow(wm.dsp, w, f, 0, 0);

	XMapWindow(wm.dsp, f);
}

