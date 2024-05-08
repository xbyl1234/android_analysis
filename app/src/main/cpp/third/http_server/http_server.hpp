#pragma once

#include <map>
#include <string>
#include <string.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "mongoose.hpp"

using std::map;
using std::string;
using std::string_view;

class HttpServer;

struct http_response {
    mg_http_message *raw;
    string url;
    map<string, string> query;
    const char *body;
    int body_len;
};

using ReqHandler = std::function<bool(HttpServer *server, mg_connection *c,
                                      const http_response &response)>;

string MgStringCopy(mg_str *str);

string_view MgStringView(mg_str *str);

class HttpServer {
public:
    HttpServer() {
        m_mgr = new mg_mgr();
        mg_mgr_init(m_mgr);
    }

    ~HttpServer() {
        if (m_mgr != nullptr)
            mg_mgr_free(m_mgr);
    }

    [[noreturn]] bool Start(const string &port);

    bool Close();

    void AddHandler(const std::string &url, ReqHandler req_handler);

    void RemoveHandler(const std::string &url);

    void BadRequest(mg_connection *connection, std::string resp);

    void SuccessRequest(mg_connection *connection, std::string resp);

private:
    static void
    HttpCallback(struct mg_connection *c, int ev, void *ev_data, void *fn_data);

    std::unordered_map<std::string, ReqHandler> handleMap;
    std::string m_port;
    mg_mgr *m_mgr;
};
