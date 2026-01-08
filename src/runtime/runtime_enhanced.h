// Enhanced Displexity Runtime with FFI Support
// File: src/runtime/runtime_enhanced.h

#ifndef DISPLEXITY_RUNTIME_ENHANCED_H
#define DISPLEXITY_RUNTIME_ENHANCED_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <dlfcn.h>  // For dynamic loading

// ============================================
// DYNAMIC LIBRARY LOADING (FFI)
// ============================================

typedef void* (*ffi_function_t)();

typedef struct {
    void* handle;
    const char* name;
    const char* path;
} dis_library;

dis_library* dis_library_open(const char* path) {
    dis_library* lib = malloc(sizeof(dis_library));
    lib->path = path;
    lib->name = strrchr(path, '/') ? strrchr(path, '/') + 1 : path;
    
    #ifdef _WIN32
        lib->handle = LoadLibraryA(path);
    #else
        lib->handle = dlopen(path, RTLD_LAZY);
    #endif
    
    return lib->handle ? lib : NULL;
}

ffi_function_t dis_library_get_function(dis_library* lib, const char* funcname) {
    if (!lib || !lib->handle) return NULL;
    
    #ifdef _WIN32
        return (ffi_function_t)GetProcAddress(lib->handle, funcname);
    #else
        return (ffi_function_t)dlsym(lib->handle, funcname);
    #endif
}

void dis_library_close(dis_library* lib) {
    if (!lib) return;
    if (lib->handle) {
        #ifdef _WIN32
            FreeLibrary(lib->handle);
        #else
            dlclose(lib->handle);
        #endif
    }
    free(lib);
}

// ============================================
// ASYNC/AWAIT SUPPORT
// ============================================

typedef struct {
    void* value;
    int resolved;
    void (*callback)(void*);
} dis_promise;

dis_promise* dis_promise_create() {
    dis_promise* p = malloc(sizeof(dis_promise));
    p->value = NULL;
    p->resolved = 0;
    p->callback = NULL;
    return p;
}

void dis_promise_resolve(dis_promise* p, void* value) {
    if (!p || p->resolved) return;
    p->value = value;
    p->resolved = 1;
    if (p->callback) {
        p->callback(value);
    }
}

void* dis_promise_await(dis_promise* p) {
    if (!p) return NULL;
    // Simple polling (real implementation would use proper async)
    int max_wait = 10000;  // 10 second timeout
    while (!p->resolved && max_wait--) {
        // In real impl, would yield or use event loop
    }
    return p->value;
}

void dis_promise_free(dis_promise* p) {
    if (p) free(p);
}

// ============================================
// ERROR HANDLING (Try/Catch)
// ============================================

typedef struct {
    const char* message;
    int code;
    int line;
    const char* file;
} dis_error;

typedef struct {
    dis_error* error;
    int in_catch;
} dis_error_context;

dis_error_context __dis_error_ctx = {NULL, 0};

dis_error* dis_error_create(const char* msg, int code, int line, const char* file) {
    dis_error* err = malloc(sizeof(dis_error));
    err->message = malloc(strlen(msg) + 1);
    strcpy((char*)err->message, msg);
    err->code = code;
    err->line = line;
    err->file = file;
    return err;
}

void dis_error_free(dis_error* err) {
    if (err) {
        if (err->message) free((void*)err->message);
        free(err);
    }
}

void dis_throw(const char* msg) {
    __dis_error_ctx.error = dis_error_create(msg, -1, 0, "");
}

int dis_in_catch() {
    return __dis_error_ctx.in_catch;
}

// ============================================
// JSON SUPPORT (LIGHTWEIGHT)
// ============================================

typedef struct dis_json_value {
    int type;  // 0=null, 1=bool, 2=number, 3=string, 4=array, 5=object
    union {
        int bool_val;
        double num_val;
        char* str_val;
        void* array_val;
        void* object_val;
    } data;
} dis_json_value;

dis_json_value* dis_json_null() {
    dis_json_value* v = malloc(sizeof(dis_json_value));
    v->type = 0;
    return v;
}

dis_json_value* dis_json_bool(int val) {
    dis_json_value* v = malloc(sizeof(dis_json_value));
    v->type = 1;
    v->data.bool_val = val;
    return v;
}

dis_json_value* dis_json_number(double val) {
    dis_json_value* v = malloc(sizeof(dis_json_value));
    v->type = 2;
    v->data.num_val = val;
    return v;
}

dis_json_value* dis_json_string(const char* val) {
    dis_json_value* v = malloc(sizeof(dis_json_value));
    v->type = 3;
    v->data.str_val = malloc(strlen(val) + 1);
    strcpy(v->data.str_val, val);
    return v;
}

void dis_json_free(dis_json_value* v) {
    if (!v) return;
    if (v->type == 3 && v->data.str_val) {
        free(v->data.str_val);
    }
    // TODO: deep free for arrays and objects
    free(v);
}

// ============================================
// HTTP/REST SUPPORT
// ============================================

typedef struct {
    int status_code;
    char* body;
    char* headers;
} dis_http_response;

dis_http_response* dis_http_get(const char* url) {
    dis_http_response* resp = malloc(sizeof(dis_http_response));
    resp->status_code = 200;
    resp->body = malloc(1024);
    resp->headers = malloc(512);
    
    // TODO: Implement actual HTTP request with curl or wininet
    sprintf(resp->body, "{\"url\": \"%s\", \"method\": \"GET\"}", url);
    
    return resp;
}

dis_http_response* dis_http_post(const char* url, const char* body) {
    dis_http_response* resp = malloc(sizeof(dis_http_response));
    resp->status_code = 200;
    resp->body = malloc(2048);
    resp->headers = malloc(512);
    
    // TODO: Implement actual HTTP POST
    sprintf(resp->body, "{\"url\": \"%s\", \"method\": \"POST\", \"body_len\": %zu}", 
            url, strlen(body));
    
    return resp;
}

void dis_http_response_free(dis_http_response* resp) {
    if (resp) {
        if (resp->body) free(resp->body);
        if (resp->headers) free(resp->headers);
        free(resp);
    }
}

// ============================================
// WEBSOCKET SUPPORT
// ============================================

typedef struct {
    const char* url;
    int connected;
    void* socket_handle;
} dis_websocket;

dis_websocket* dis_websocket_create(const char* url) {
    dis_websocket* ws = malloc(sizeof(dis_websocket));
    ws->url = url;
    ws->connected = 0;
    ws->socket_handle = NULL;
    return ws;
}

int dis_websocket_connect(dis_websocket* ws) {
    if (!ws) return 0;
    // TODO: Implement WebSocket handshake
    ws->connected = 1;
    return 1;
}

int dis_websocket_send(dis_websocket* ws, const char* message) {
    if (!ws || !ws->connected) return 0;
    // TODO: Send WebSocket frame
    return 1;
}

char* dis_websocket_receive(dis_websocket* ws) {
    if (!ws || !ws->connected) return "";
    // TODO: Receive WebSocket frame
    return "";
}

void dis_websocket_close(dis_websocket* ws) {
    if (!ws) return;
    ws->connected = 0;
    // TODO: Send close frame
}

void dis_websocket_free(dis_websocket* ws) {
    if (ws) {
        if (ws->connected) dis_websocket_close(ws);
        free(ws);
    }
}

// ============================================
// MODULE/PLUGIN SYSTEM
// ============================================

typedef void (*module_init_t)();
typedef void (*module_cleanup_t)();

typedef struct {
    const char* name;
    dis_library* lib;
    module_init_t init;
    module_cleanup_t cleanup;
} dis_module;

dis_module* dis_module_load(const char* name, const char* path) {
    dis_module* mod = malloc(sizeof(dis_module));
    mod->name = name;
    mod->lib = dis_library_open(path);
    
    if (!mod->lib) {
        free(mod);
        return NULL;
    }
    
    mod->init = (module_init_t)dis_library_get_function(mod->lib, "module_init");
    mod->cleanup = (module_init_t)dis_library_get_function(mod->lib, "module_cleanup");
    
    if (mod->init) mod->init();
    
    return mod;
}

void dis_module_unload(dis_module* mod) {
    if (!mod) return;
    if (mod->cleanup) mod->cleanup();
    if (mod->lib) dis_library_close(mod->lib);
    free(mod);
}

#endif // DISPLEXITY_RUNTIME_ENHANCED_H
