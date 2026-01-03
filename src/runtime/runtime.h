// Displexity Runtime Library

#ifndef DISPLEXITY_RUNTIME_H
#define DISPLEXITY_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// String utilities
typedef struct {
    char* data;
    size_t length;
} dis_string;

dis_string dis_string_create(const char* s) {
    dis_string str;
    str.length = strlen(s);
    str.data = malloc(str.length + 1);
    strcpy(str.data, s);
    return str;
}

void dis_string_free(dis_string* str) {
    if (str->data) free(str->data);
}

// Array type
typedef struct {
    void* data;
    size_t length;
    size_t capacity;
    size_t elem_size;
} dis_array;

dis_array dis_array_create(size_t elem_size, size_t capacity) {
    dis_array arr;
    arr.elem_size = elem_size;
    arr.capacity = capacity ? capacity : 10;
    arr.length = 0;
    arr.data = malloc(arr.capacity * elem_size);
    return arr;
}

void dis_array_push(dis_array* arr, void* elem) {
    if (arr->length >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = realloc(arr->data, arr->capacity * arr->elem_size);
    }
    memcpy((char*)arr->data + arr->length * arr->elem_size, elem, arr->elem_size);
    arr->length++;
}

void* dis_array_get(dis_array* arr, size_t idx) {
    if (idx < arr->length) {
        return (char*)arr->data + idx * arr->elem_size;
    }
    return NULL;
}

void dis_array_set(dis_array* arr, size_t idx, void* elem) {
    if (idx < arr->length) {
        memcpy((char*)arr->data + idx * arr->elem_size, elem, arr->elem_size);
    }
}

void dis_array_free(dis_array* arr) {
    if (arr->data) free(arr->data);
}

// HTTP request (stub for now)
typedef struct {
    char* response;
} dis_http_response;

dis_http_response dis_http_get(const char* url) {
    dis_http_response resp;
    resp.response = malloc(1024);
    sprintf(resp.response, "(http stub: %s)", url);
    return resp;
}

void dis_http_response_free(dis_http_response* resp) {
    if (resp->response) free(resp->response);
}

// Graphics API (stub)
void dis_graphics_init() {
    printf("Graphics initialized\n");
}

void dis_graphics_clear() {
    // stub
}

void dis_graphics_draw_rect(int x, int y, int w, int h) {
    printf("Draw rect: (%d, %d) %dx%d\n", x, y, w, h);
}

void dis_graphics_close() {
    printf("Graphics closed\n");
}

#endif // DISPLEXITY_RUNTIME_H
