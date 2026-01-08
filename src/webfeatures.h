// Displexity Web Features Module
// HTTP, WebSocket, REST API support
// File: src/webfeatures.h

#ifndef DISPLEXITY_WEBFEATURES_H
#define DISPLEXITY_WEBFEATURES_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace disp {

// HTTP Method enumeration
enum class HTTPMethod {
    GET,
    POST,
    PUT,
    DELETE,
    PATCH,
    HEAD,
    OPTIONS
};

// HTTP Status codes
enum class HTTPStatus {
    OK = 200,
    CREATED = 201,
    ACCEPTED = 202,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    INTERNAL_SERVER_ERROR = 500,
    SERVICE_UNAVAILABLE = 503
};

// HTTP Request/Response
struct HTTPRequest {
    HTTPMethod method;
    std::string url;
    std::map<std::string, std::string> headers;
    std::string body;
    int timeout;
    
    HTTPRequest(HTTPMethod m, const std::string& u)
        : method(m), url(u), timeout(30) {}
};

struct HTTPResponse {
    int statusCode;
    std::string statusText;
    std::map<std::string, std::string> headers;
    std::string body;
    double responseTime;
    bool success;
    
    HTTPResponse() : statusCode(0), responseTime(0.0), success(false) {}
};

// JSON support (lightweight)
class JSONValue {
public:
    enum Type { NULL_TYPE, BOOL, NUMBER, STRING, ARRAY, OBJECT };
    
    Type type;
    
    JSONValue() : type(NULL_TYPE) {}
    explicit JSONValue(bool b) : type(BOOL), boolValue(b) {}
    explicit JSONValue(double n) : type(NUMBER), numValue(n) {}
    explicit JSONValue(const std::string& s) : type(STRING), strValue(s) {}
    
    static JSONValue array() {
        JSONValue v;
        v.type = ARRAY;
        return v;
    }
    
    static JSONValue object() {
        JSONValue v;
        v.type = OBJECT;
        return v;
    }
    
    bool getBool() const { return boolValue; }
    double getNumber() const { return numValue; }
    std::string getString() const { return strValue; }
    
    void add(const JSONValue& val) {
        if (type == ARRAY) arrayValue.push_back(val);
    }
    
    void set(const std::string& key, const JSONValue& val) {
        if (type == OBJECT) objectValue[key] = val;
    }
    
    std::string toString() const;
    static JSONValue parse(const std::string& json);
    
private:
    bool boolValue;
    double numValue;
    std::string strValue;
    std::vector<JSONValue> arrayValue;
    std::map<std::string, JSONValue> objectValue;
};

// WebSocket support
class WebSocket {
private:
    std::string url;
    int port;
    bool connected;
    
public:
    WebSocket(const std::string& u, int p = 80)
        : url(u), port(p), connected(false) {}
    
    bool connect() {
        // Would implement WebSocket handshake
        connected = true;
        return true;
    }
    
    bool send(const std::string& message) {
        if (!connected) return false;
        // Would implement WebSocket frame sending
        return true;
    }
    
    std::string receive() {
        if (!connected) return "";
        // Would implement WebSocket frame receiving
        return "";
    }
    
    void close() {
        connected = false;
    }
    
    bool isConnected() const { return connected; }
};

// REST API Client
class RESTClient {
private:
    std::string baseURL;
    std::map<std::string, std::string> defaultHeaders;
    
public:
    RESTClient(const std::string& baseUrl)
        : baseURL(baseUrl) {
        defaultHeaders["Content-Type"] = "application/json";
        defaultHeaders["Accept"] = "application/json";
    }
    
    void setHeader(const std::string& key, const std::string& value) {
        defaultHeaders[key] = value;
    }
    
    HTTPResponse get(const std::string& endpoint);
    HTTPResponse post(const std::string& endpoint, const std::string& body);
    HTTPResponse put(const std::string& endpoint, const std::string& body);
    HTTPResponse deleteResource(const std::string& endpoint);
    
private:
    HTTPResponse request(HTTPMethod method, const std::string& endpoint, const std::string& body = "");
};

// Simple HTTP Server
class SimpleHTTPServer {
private:
    int port;
    std::string hostname;
    bool running;
    
    struct Route {
        std::string path;
        HTTPMethod method;
        std::string handler;
    };
    
    std::vector<Route> routes;
    
public:
    SimpleHTTPServer(int p = 8080, const std::string& host = "localhost")
        : port(p), hostname(host), running(false) {}
    
    void addRoute(const std::string& path, HTTPMethod method, const std::string& handler) {
        routes.push_back({path, method, handler});
    }
    
    bool start() {
        running = true;
        return true;
    }
    
    void stop() {
        running = false;
    }
    
    bool isRunning() const { return running; }
};

// GraphQL support
class GraphQLClient {
private:
    std::string endpoint;
    
public:
    GraphQLClient(const std::string& url) : endpoint(url) {}
    
    HTTPResponse query(const std::string& queryString, const std::string& variables = "");
    HTTPResponse mutation(const std::string& mutationString, const std::string& variables = "");
};

} // namespace disp

#endif // DISPLEXITY_WEBFEATURES_H
