/**
 * Copyright (c) 2018 along All rights reserved.
 **/

#include "Http.h"
#include <cstring>
#include <string>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace boost;

Request::Request(const char *request, size_t length) {
    parse(request, length);
}

std::string Request::headers() {
    string headers = str(format("%1% %2% HTTP/%3%\r\n") % method() % url() % version());
    for (auto &header:data())
        headers += str(format("%1%: %2%\r\n") % header.first % header.second);
    headers += "\r\n";
    return headers;
}

Request &Request::parse(const char *request, size_t length) {
    port_ = HTTP_PORT;
    char *content = strstr(request, "\r\n\r\n");
    if (content == NULL) {
        return *this;
    }
    content_ = std::string(content + 4, length - (content + 4 - request));
    string header(request, content - request);

    vector<string> lines;
    split_regex(lines, header, regex("\r\n"));

    int line_num = 1;
    for (auto &line:lines) {
        vector<string> heads;
        if (line_num == 1) {
            split(heads, line, is_any_of(" "));
            method_ = heads[0];
            url_ = heads[1];
            version_ = heads[2].substr(5);
        } else {
            split_regex(heads, line, regex(": "));
            if (heads.size() == 2)
                data_[heads[0]] = heads[1];
            else {
                BOOST_LOG_TRIVIAL(warning) << "line(" << line_num << ") parse error!(" << line << ")";
            }
        }
        line_num++;
    }

    auto pos = data_["host"].find(':');
    if (pos == string::npos) {
        host_ = data_["host"];
    } else {
        host_ = data_["host"].substr(0, pos);
        port_ = (unsigned short) std::stoi(data_["host"].substr(pos + 1));
    }
    return *this;
}

Request &Request::update(const std::string &key, const std::string &value) {
    utiles::CaseInsensitiveComparator cmp;
    if (!cmp(key, "METHOD")) {
        method_ = value;
    } else if (!cmp(key, "URL")) {
        url_ = value;
    } else if (!cmp(key, "VERSION")) {
        version_ = value;
    } else {
        auto before = data_[key];
        data_[key] = value;
        if (!cmp(key, "host")) {
            url_ = "/";
            auto pos = data_["host"].find(':');
            if (pos == string::npos) {
                host_ = data_["host"];
            } else {
                host_ = data_["host"].substr(0, pos);
                port_ = (unsigned short) std::stoi(data_["host"].substr(pos + 1));
            }
        }
    }
    return *this;
}

std::string Request::source() {
    char buf[headers().length() + content().size()];
    memcpy(buf, headers().data(), headers().length());
    memcpy(buf + headers().length(), content().data(), content().size());
    return string(buf, headers().length() + content().size());
}

Response::Response(const char *reponse, size_t length) {
    char *content = strstr(reponse, "\r\n\r\n");
    content_len_ = length - (content + 4 - reponse);
    content_ = std::string(content + 4, content_len_);
    string header(reponse, content - reponse);

    vector<string> lines;
    split_regex(lines, header, regex("\r\n"));

    int line_num = 1;
    for (auto &line:lines) {
        vector<string> heads;
        if (line_num == 1) {
            split(heads, line, is_any_of(" "));
            version_ = heads[0].substr(5);
            status_ = (unsigned short) std::stoi(heads[1]);
            info_ = heads[2];
        } else {
            split_regex(heads, line, regex(": "));
            if (heads.size() == 2)
                data_[heads[0]] = heads[1];
            else {
                BOOST_LOG_TRIVIAL(warning) << "line(" << line_num << ") parse error!(" << line << ")";
            }
        }
        line_num++;
    }

    unsigned long len = std::stoul(data_["Content-Length"]);
    if (len != content_len_)
        BOOST_LOG_TRIVIAL(warning) << "Content-length error!";
    content_len_ = len;
}
