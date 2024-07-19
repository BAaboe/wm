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
	container->capacity = 0;
	container->array = (client*)malloc(container->capacity*sizeof(client*));

	if(!container->array){
		printf("Malloc failes\n");
	}
	
	*arr_ptr = container;
}

void freeArray(dynamic_array *arr_ptr){
	free(arr_ptr->array);
	free(arr_ptr);
}

void addClient(dynamic_array *arr_ptr, client client_){
	arr_ptr->capacity++;
	arr_ptr->array = (client*)realloc(arr_ptr->array, arr_ptr->capacity*sizeof(client*));

	arr_ptr->array[arr_ptr->size++] = client_;
}

client getClientFromIndex(dynamic_array *arr_ptr, int index){
	if(index >= arr_ptr->capacity){
		printf("Out of bounds\n");
		client client_;
		return client_;
	}
	return arr_ptr->array[index];
}

client getClient(dynamic_array *arr_ptr, Window window){
	client client_;
	for(int i = 0; i < arr_ptr->size; i++){
		client_ = getClientFromIndex(arr_ptr, i);
		if(client_.window == window){
			return client_;
		}
	}
	client empty;
	return empty;
}

void deleteClient(dynamic_array* arr_ptr, int index){
	if(index >= arr_ptr->size){
		printf("Out of bounds\n");
		return;
	}

	for(int i = index; i < arr_ptr->size; i++){
		arr_ptr->array[i] = arr_ptr->array[i + 1];
	}
	
	arr_ptr->capacity--;
	arr_ptr->array = (client*)realloc(arr_ptr->array, arr_ptr->capacity*sizeof(client*));
	arr_ptr->size--;
}
