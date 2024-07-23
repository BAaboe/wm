#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "clients.h"

#include "config.h"

static Bool wm_deteced;

static WindowManager wm;

static dynamic_array* clients_;

int OnXError(Display* display, XErrorEvent* e){
	printf("Error: %u, request_code: %u, %lu\n", e->error_code, e->request_code, e->resourceid);
	return 0;
}

int OnWMDetected(Display* display, XErrorEvent* e){
	if((int)e->error_code == BadAccess){
		wm_deteced = True;
	}

	return 0;
}

void setup(){
	wm.dsp = XOpenDisplay(NULL);
	if (wm.dsp == NULL){
		exit(EXIT_FAILURE);
	}
	wm.root = DefaultRootWindow(wm.dsp);
	wm_deteced = False;

	XSetErrorHandler(&OnWMDetected);
	XSelectInput(wm.dsp, wm.root, SubstructureRedirectMask | SubstructureNotifyMask);
	//XSync(wm.dsp, false);

	if(wm_deteced){
		printf("Error: detected another window manager on display\n");
		return;
	}

	XSetErrorHandler(&OnXError);

	//Reparent and frame exsiting windows
	XGrabServer(wm.dsp);

	Window returned_root, returned_parent;
	Window* top_level_windows;
	unsigned int num_top_level_windows;

	if(!XQueryTree(
			wm.dsp,
			wm.root,
			&returned_root,
			&returned_parent,
			&top_level_windows,
			&num_top_level_windows)){return;}
	if(returned_root != wm.root){return;}

	for(unsigned int i = 0; i<num_top_level_windows; i++){
		Frame(top_level_windows[i], True);
	}

	XFree(top_level_windows);

	XUngrabServer(wm.dsp);
	

	//Displaying cursor
	Cursor cursor = XCreateFontCursor(wm.dsp, XC_left_ptr);
	XDefineCursor(wm.dsp, wm.root, cursor);
	XSync(wm.dsp, 0);

	//Startup script for startup programs
	system(STARTUP_SCRIPT);
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
			case MotionNotify:
				OnMotionNotify(&e.xmotion);
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
	Frame(e->window, False);

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

void OnButtonPressed(const XButtonEvent *e) {
	if(!containsClient(clients_, e->window)){return;}

	client client_, newClient;
	getClient(clients_, e->window, &client_);

	newClient = client_;

	XWindowAttributes fwa;
	XGetWindowAttributes(wm.dsp, client_.frame, &fwa);

	pos frame_pos;
	frame_pos.x = fwa.x;
	frame_pos.y = fwa.y;

	pos drage_start_pos;
	drage_start_pos.x = e->x_root;
	drage_start_pos.y = e->y_root;

	newClient.drag_start_pos = drage_start_pos;
	newClient.frame_start_pos = frame_pos;

	setClient(clients_, e->window, newClient);
}

void OnMotionNotify(const XMotionEvent *e){
	if(!containsClient(clients_, e->window)){return;}
	
	client client_;
	getClient(clients_, e->window, &client_);

	if(e->state & Button1Mask){
		pos new_pos;
		//caluclates the new position of the window by findig out how much it has moved and adds that to the last position
		new_pos.x = client_.frame_start_pos.x+(e->x_root-client_.drag_start_pos.x);
		new_pos.y = client_.frame_start_pos.y+(e->y_root-client_.drag_start_pos.y);

		XMoveWindow(wm.dsp, client_.frame, new_pos.x, new_pos.y);
	}
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

	XGrabButton(wm.dsp, Button1, Mod4Mask, w, False, ButtonPress|ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, None);

}

void UnFrame(Window w){
	client client_;
	getClient(clients_, w, &client_);
	
	XUnmapWindow(wm.dsp, client_.frame);
	XReparentWindow(
			wm.dsp,
			w,
			wm.root,
			0, 0);

	XRemoveFromSaveSet(wm.dsp, w);

	XDestroyWindow(wm.dsp, client_.frame);

	deleteFrame(clients_, w);
	
}
