#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#else
#include <curl/curl.h>
#endif

// HTTP runtime
#ifdef _WIN32
char* __disp_http_get(const char* url) {
    static char buffer[65536];
    HINTERNET hInternet = InternetOpenA("DISP", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return "";
    HINTERNET hUrl = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (!hUrl) { InternetCloseHandle(hInternet); return ""; }
    DWORD bytesRead = 0;
    InternetReadFile(hUrl, buffer, sizeof(buffer)-1, &bytesRead);
    buffer[bytesRead] = 0;
    InternetCloseHandle(hUrl);
    InternetCloseHandle(hInternet);
    return buffer;
}
#else
static size_t __curl_write(void* p, size_t s, size_t n, void* u) {
    strncat((char*)u, (char*)p, s*n);
    return s*n;
}
char* __disp_http_get(const char* url) {
    static char buffer[65536] = {0};
    CURL* curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __curl_write);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return buffer;
}
#endif



int main(int argc, char** argv) {
    printf("%s\n", "Fetching data from httpbin.org...");
    char* response = __disp_http_get("https://httpbin.org/get");
    printf("%s\n", response);
    return 0;
}
