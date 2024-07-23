#include <X11/X.h>
#include <X11/Xlib.h>



int OnWMDetected(Display* display, XErrorEvent* e);
int OnXError(Display* display, XErrorEvent* e);


typedef struct {
	Display* dsp;
	Window root;
} WindowManager;


void setup();
void run();

void OnCreateNotify(const XCreateWindowEvent* e);
void OnDestroyNotify(const XDestroyWindowEvent* e);
void OnConfigRequest(const XConfigureRequestEvent* e);
void OnConfigNotify(const XConfigureEvent* e);
void OnMapRequest(const XMapRequestEvent* e);
void OnMapNotify(const XMapEvent* e);
void OnUnmapNotify(const XUnmapEvent* e);

void OnButtonPressed(const XButtonEvent* e);
void OnMotionNotify(const XMotionEvent* e);

void Frame(Window w, Bool before_wm);
void UnFrame(Window w);
