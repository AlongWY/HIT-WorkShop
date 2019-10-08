/**
 * Copyright (c) 2018 along All rights reserved.
 **/

#ifndef TINYPROXY_REQUESTHEADER_H
#define TINYPROXY_REQUESTHEADER_H

#include <string>
#include <cstring>
#include <map>

#define HTTP_PORT 80  // http 服务器端口

namespace utiles {
    struct CaseInsensitiveComparator {
        bool operator()(const std::string &a, const std::string &b) const noexcept {
            return strcasecmp(a.c_str(), b.c_str()) < 0;
        }
    };
}

template<typename T>
using CaseInsensitiveMap = std::map<std::string, T, utiles::CaseInsensitiveComparator>;

class Request {
public:
    const std::string &method() { return method_; };

    const std::string &version() { return version_; };

    const std::string &url() { return url_; };

    const std::string &host() { return host_; };

    const std::string &content() { return content_; };

    const unsigned short &port() { return port_; };

    const CaseInsensitiveMap<std::string> &data() { return data_; };

    explicit Request(const char *request, size_t length);

    Request &parse(const char *request, size_t length);

    Request &update(const std::string &key, const std::string &value);

    std::string headers();

    std::string source();

private:
    std::string method_;            // POST 或者 GET,注意有些为 CONNECT
    std::string url_;               // 请求的 url
    std::string version_;           // HTTP版本

    std::string host_;              // 目标主机
    unsigned short port_;

    std::string content_;           // 额外的数据，比如POST的数据
    CaseInsensitiveMap<std::string> data_;
};

class Response {
public:
    const std::string &version() { return version_; };

    const std::string &info() { return info_; };

    const unsigned short &status() { return status_; };

    const std::string &content() { return content_; };

    const CaseInsensitiveMap<std::string> &data() { return data_; };

    explicit Response(const char *reponse, size_t length);

private:
    std::string version_;           // HTTP版本
    std::string info_;              // 状态信息
    unsigned short status_;         // 状态码
    std::string content_;           // 额外的数据
    unsigned long content_len_;     // 消息体长度
    CaseInsensitiveMap<std::string> data_;
};


#endif //TINYPROXY_REQUESTHEADER_H
