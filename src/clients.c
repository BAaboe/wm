#include "clients.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>

void initArray(dynamic_array **arr_ptr){
	dynamic_array* container;
	container = (dynamic_array*)malloc(sizeof(dynamic_array));

	if(!container){
		printf("Malloc failed\n");
	}

	container->size = 0;
	container->capacity = MIN_NUM_CAP;
	container->array = (client*)malloc(container->capacity*sizeof(client));

	if(!container->array){
		printf("Malloc failes\n");
	}
	
	*arr_ptr = container;
}

void freeArray(dynamic_array *arr_ptr){
	free(arr_ptr->array);
	free(arr_ptr);
}

void addClientFromStruct(dynamic_array *arr_ptr, client client_){
	if(arr_ptr->capacity == arr_ptr->size){
		arr_ptr->capacity <<= 1;
		arr_ptr->array = (client*)realloc(arr_ptr->array, arr_ptr->capacity*sizeof(client));
		if(arr_ptr->array == NULL){
			printf("Failed allocating memmory");
			exit(EXIT_FAILURE);
		}
	}

	arr_ptr->array[arr_ptr->size++] = client_;
}

int getClientFromIndex(dynamic_array *arr_ptr, int index, client* returnClient){
	if(index >= arr_ptr->capacity){
		printf("Out of bounds\n");
		return 1;
	}
	*returnClient = arr_ptr->array[index];
	return 0; 
}

void deleteClientFromIndex(dynamic_array* arr_ptr, int index){
	if(index >= arr_ptr->size){
		printf("Out of bounds\n");
		return;
	}

	for(int i = index; i < arr_ptr->size; i++){
		arr_ptr->array[i] = arr_ptr->array[i + 1];
	}
	
	if(arr_ptr->capacity >> 1 == arr_ptr->size){
		if(arr_ptr->capacity > MIN_NUM_CAP){
			arr_ptr->capacity >>= 1;

			arr_ptr->array = (client*)realloc(arr_ptr->array, arr_ptr->capacity*sizeof(client));
		}
	}
	arr_ptr->size--;
}

void deleteFrame(dynamic_array *arr_ptr, Window window){
	int index = indexClient(arr_ptr, window);
	deleteClientFromIndex(arr_ptr, index);
}


int getClient(dynamic_array *arr_ptr, Window window, client* returnClient){
	client client_;
	for(int i = 0; i < arr_ptr->size; i++){
		if(getClientFromIndex(arr_ptr, i, &client_) == 1){
			return -1;
		}
		if(client_.window == window){
			*returnClient = client_;
			return i;
		}
	}
	return -1;
}

int setClient(dynamic_array *arr_ptr, Window window, client client_){
	int index = indexClient(arr_ptr, window);
	if(index == -1){
		return -1;
	}
	arr_ptr->array[index] = client_;
	return 0;
}

Bool containsClient(dynamic_array *arr_ptr, Window window){
	client temp;
	if(getClient(arr_ptr, window, &temp) == -1){
		return 0;
	}
	return 1;
}

int indexClient(dynamic_array *arr_ptr, Window window){
	client temp;
	return getClient(arr_ptr, window, &temp);
}

void addClient(dynamic_array *arr_ptr, Window window, Window frame){
	client temp;
	temp.window = window;
	temp.frame = frame; 
	addClientFromStruct(arr_ptr, temp);
}

//used to debuging
void printPos(pos pos_){
	printf("x: %d, y: %d\n", pos_.x, pos_.y);
}
