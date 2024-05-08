#include <utility>
#include "http_server.hpp"
#include "../utils/log.h"

string MgStringCopy(mg_str *str) {
    return string(str->ptr, str->len);
}

string_view MgStringView(mg_str *str) {
    return std::string_view(str->ptr, str->len);
}

bool parse_query(const char *query, int queryLen, map<string, string> &ret) {
    int buffLen = queryLen * 2;
    char *buff = new char[buffLen];
    buff[0] = 0;
    defer([&]() {
        delete[]buff;
    });
    int retLen = mg_url_decode(query, queryLen, buff, buffLen, 0);
    if (retLen == -1) {
        return false;
    }

    const auto &sp = string_split(buff, "&");
    for (const auto &item: sp) {
        auto params = string_split(item, "=");
        if (params.size() != 2) {
            continue;
        }
        ret[params[0]] = params[1];
    }
    return true;
}

void
HttpServer::HttpCallback(mg_connection *connection, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        auto *hm = (struct mg_http_message *) ev_data;
        auto *server = (HttpServer *) fn_data;

        http_response response;
        response.url = MgStringCopy(&hm->uri);

        if (hm->query.len > 0) {
            if (!parse_query(hm->query.ptr, hm->query.len, response.query)) {
                logd("http request %s decode query error!", response.url.c_str());
                server->BadRequest(connection, "{\"error\":\"decode query error!\"}");
                return;
            }
        }

        response.body = hm->body.ptr;
        response.body_len = hm->body.len;
        response.raw = hm;

        logd("http recv url: %s data: %s", response.url.c_str(), MgStringCopy(&hm->query).c_str());
        auto handle = server->handleMap.find(response.url);
        if (handle == server->handleMap.end()) {
            logd("http request %s not find handle", response.url.c_str());
            server->BadRequest(connection, "{\"error\":\"url error\"}");
            return;
        }

        handle->second(server, connection, response);
    }

}

void HttpServer::BadRequest(mg_connection *connection, std::string resp) {
    mg_http_reply(connection, 404, "", resp.c_str());
}

void HttpServer::SuccessRequest(mg_connection *connection, std::string resp) {
    mg_http_reply(connection, 200, "", resp.c_str());
}

bool HttpServer::Start(const string &port) {
    m_port = port;
    mg_connection *connect = mg_http_listen(m_mgr, ("http://0.0.0.0:" + port).c_str(),
                                            &HttpServer::HttpCallback, this);
    if (connect == nullptr) {
        return false;
    }
    while (true)
        mg_mgr_poll(m_mgr, 500);
    return true;
}

void HttpServer::AddHandler(const std::string &url, ReqHandler req_handler) {
    if (handleMap.find(url) != handleMap.end())
        return;
    handleMap.insert(std::make_pair(url, req_handler));
}

void HttpServer::RemoveHandler(const std::string &url) {
    auto it = handleMap.find(url);
    if (it != handleMap.end())
        handleMap.erase(it);
}

bool HttpServer::Close() {
    if (m_mgr != nullptr) {
        mg_mgr_free(m_mgr);
        m_mgr = nullptr;
    }
    return true;
}