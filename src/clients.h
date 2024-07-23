#include <X11/X.h>
#include <X11/Xlib.h>

#define MIN_NUM_CAP 1 

typedef struct{
	int x;
	int y;
} pos;

typedef struct {
	Window frame;
	Window window;
	pos drag_start_pos;
	pos frame_start_pos;
}client;

typedef struct {
	size_t size;
	size_t capacity;
	client* array;
}dynamic_array;


void initArray(dynamic_array** arr_ptr);
void freeArray(dynamic_array* arr_ptr);

void addClientFromStruct(dynamic_array* arr_ptr, client client_);
int getClientFromIndex(dynamic_array* arr_ptr, int index, client* returnClient);
void deleteClientFromIndex(dynamic_array* arr_ptr, int index);

int getClient(dynamic_array* arr_ptr, Window window, client* returnClient);
int setClient(dynamic_array* arr_ptr, Window window, client client_);
void addClient(dynamic_array* arr_ptr, Window window, Window frame);
void deleteFrame(dynamic_array *arr_ptr, Window window);
Bool containsClient(dynamic_array* arr_ptr, Window window);
int indexClient(dynamic_array *arr_ptr, Window window);

void printPos(pos pos_);
