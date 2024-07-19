#include <X11/X.h>
#include <X11/Xlib.h>

typedef struct {
	Window frame;
	Window window;
}client;

typedef struct {
	size_t size;
	size_t capacity;
	client* array;
}dynamic_array;


void initArray(dynamic_array** arr_ptr);
void freeArray(dynamic_array* arr_ptr);

void addClient(dynamic_array* arr_ptr, client client_);
client getClientFromIndex(dynamic_array* arr_ptr, int index);
void deleteClient(dynamic_array* arr_ptr, int index);

client getClient(dynamic_array* arr_ptr, Window window);
