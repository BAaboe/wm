#include <X11/X.h>
#include <X11/Xlib.h>
#define true 1
#define false 0

int OnWMDetected(Display* display, XErrorEvent* e);
int OnXError(Display* display, XErrorEvent* e);


typedef struct {
	Display* dsp;
	Window root;
} WindowManager;


void setup();
void run();

void OnCreateNotify(const XCreateWindowEvent* e);
void OnConfigRequest(const XConfigureRequestEvent* e);
void OnConfigNotify(const XConfigureEvent* e);
void OnMapRequest(const XMapRequestEvent* e);
void OnMapNotify(const XMapEvent* e);

void Frame(Window w);
