#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "clients.h"

#include "config.h"

#define true 1
#define false 0



static Bool wm_deteced;

static WindowManager wm;

static dynamic_array* clients_;

int OnXError(Display* display, XErrorEvent* e){
	printf("Error: %u, request_code: %u, %lu\n", e->error_code, e->request_code, e->resourceid);
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
	wm_deteced = false;

	XSetErrorHandler(&OnWMDetected);
	XSelectInput(wm.dsp, wm.root, SubstructureRedirectMask | SubstructureNotifyMask);
	//XSync(wm.dsp, false);

	if(wm_deteced){
		printf("Error: detected another window manager on display\n");
		return;
	}

	XSetErrorHandler(&OnXError);

	//Reparent and frame exsiting windows
	//XGrabServer(wm.dsp);

	//Window returned_root, returned_parent;
	//Window* top_level_windows;
	//unsigned int num_top_level_windows;

	//if(!XQueryTree(
	//		wm.dsp,
	//		wm.root,
	//		&returned_root,
	//		&returned_parent,
	//		&top_level_windows,
	//		&num_top_level_windows)){return;}
	//if(returned_root != wm.root){return;}

	//for(unsigned int i = 0; i<num_top_level_windows; i++){
	//	Frame(top_level_windows[i], true);
	//}

	//XFree(top_level_windows);

	//XUngrabServer(wm.dsp);
	
	KeySym ks = XStringToKeysym("a");
	KeyCode kc = XKeysymToKeycode(wm.dsp, ks);

	int code = XGrabKey(wm.dsp, kc, ShiftMask, wm.root, 0,  GrabModeAsync, GrabModeAsync);
	printf("%d\n", code==AlreadyGrabbed);
	XSync(wm.dsp, 0);

	//Displaying cursor
	Cursor cursor = XCreateFontCursor(wm.dsp, XC_left_ptr);
	XDefineCursor(wm.dsp, wm.root, cursor);
	XSync(wm.dsp, 0);

	//Startup script for startup programs
	//system(STARTUP_SCRIPT);
}

void run(){
	
	XEvent e;
	while(1){
		XNextEvent(wm.dsp, &e);
		
		switch(e.type){
			case CreateNotify:
				OnCreateNotify(&e.xcreatewindow);
				break;
			case DestroyNotify:
				OnDestroyNotify(&e.xdestroywindow);
				break;
			case ConfigureRequest:
				OnConfigRequest(&e.xconfigurerequest);
				break;
			case ConfigureNotify:
				OnConfigNotify(&e.xconfigure);
				break;
			case MapRequest:
				OnMapRequest(&e.xmaprequest);
				break;
			case MapNotify:
				OnMapNotify(&e.xmap);
				break;
			case UnmapNotify:
				OnUnmapNotify(&e.xunmap);
				break;
			case ButtonPress:
				OnButtonPressed(&e.xbutton);
				break;
			case KeyPress:
				printf("keypressed\n");
				break;
		}

	}
}

int main(){
	initArray(&clients_);

	setup();

	run();

	freeArray(clients_);	
	XCloseDisplay(wm.dsp);
	return 0;
}


void OnCreateNotify(const XCreateWindowEvent* e) {}

void OnDestroyNotify(const XDestroyWindowEvent *e){}

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
	Frame(e->window, false);

	XMapWindow(wm.dsp, e->window);
}

void OnMapNotify(const XMapEvent* e){}

void OnUnmapNotify(const XUnmapEvent* e){
	if(e->event == wm.root){
		return;
	}

	if(!containsClient(clients_, e->window)){
		return;
	}
	UnFrame(e->window);	
}

void OnButtonPressed(const XButtonPressedEvent *e) {
	printf("ButtonPressed\n");
}

void Frame(Window w, Bool before_wm){
	XWindowAttributes x_w_attrs;
	XGetWindowAttributes(wm.dsp, w, &x_w_attrs);
	
	if(before_wm){
		if(x_w_attrs.override_redirect || x_w_attrs.map_state != IsViewable){
			return;
		}
	}

	const Window frame = XCreateSimpleWindow(
			wm.dsp,
			wm.root,
			x_w_attrs.x,
			x_w_attrs.y,
			x_w_attrs.width,
			x_w_attrs.height,
			BORDER_WIDTH,
			BORDER_COLOR,
			BG_COLOR);

	
	XSelectInput(wm.dsp, frame, SubstructureRedirectMask | SubstructureNotifyMask);	
	XAddToSaveSet(wm.dsp, w);

	XReparentWindow(wm.dsp, w, frame, 0, 0);

	XMapWindow(wm.dsp, frame);

	addClient(clients_, w, frame);

}

void UnFrame(Window w){
	Window frame;
	getFrame(clients_, w, &frame);
	
	XUnmapWindow(wm.dsp, frame);
	XReparentWindow(
			wm.dsp,
			w,
			wm.root,
			0, 0);

	XRemoveFromSaveSet(wm.dsp, w);

	XDestroyWindow(wm.dsp, frame);

	deleteFrame(clients_, w);
	
}
