/**
 * Copyright (c) 2018 along All rights reserved.
 **/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <libnet.h>

#include <unistd.h>
#include <errno.h>

#include <boost/log/trivial.hpp>
#include <boost/format.hpp>
#include <cpptoml.h>
#include <iostream>
#include <set>
#include <thread>

#include "Http.h"
#include "utils.h"
#include "DateTime.h"

#define MAXSIZE 65507 // 发送数据报文的最大长度
#define HTTP_PORT 80  // http 服务器端口

using namespace ::boost;
namespace toml = cpptoml;

void signal_handler(int);

bool InitProxy(int port);

bool ConnectToServer(int *server_fd, const std::string &host, const unsigned short &port);

bool Authorizate(int client_fd, char *buffer, Request &request, std::shared_ptr<toml::table> &info);

void ProxyThread(int client_fd);

//代理相关参数
int ProxySocketFd;
sockaddr_in ProxyServerAddr;
bool isContinue = true;

// 全局设置
std::shared_ptr<toml::table> config;

int main(int argc, char *argv[]) {
    config = toml::parse_file("config.toml");
    const int &port = *config->get_qualified_as<int>("server.port");

    signal(SIGINT, signal_handler);
    BOOST_LOG_TRIVIAL(info) << "代理服务器正在启动";
    BOOST_LOG_TRIVIAL(info) << "初始化...";

    if (!InitProxy(port)) {
        BOOST_LOG_TRIVIAL(error) << "socket 初始化失败";
        return -1;
    }
    BOOST_LOG_TRIVIAL(info) << "代理服务器正在运行,监听端口" << port;
    int client_fd = 0;

    //代理服务器不断监听
    while (isContinue) {
        client_fd = accept(ProxySocketFd, nullptr, nullptr);
        std::thread t(ProxyThread, client_fd);
        t.detach();
    }

    close(ProxySocketFd);
    return 0;
}

void ProxyThread(int client_fd) {
    bool is_connect = true;
    int server_fd;
    char buffer[MAXSIZE];
    ssize_t recv_len, sent_state;
    fd_set read_fds;              // 使用select监控的套接字集合
    timeval timeout = {3, 0};    // select等待3秒，3秒轮询，要非阻塞就置0

    // 从客户端获取 Http 数据报文
    bzero(buffer, MAXSIZE);
    recv_len = recv(client_fd, buffer, MAXSIZE, 0);

    if (recv_len > 0) {
        Request request(buffer, static_cast<size_t>(recv_len));

        std::shared_ptr<toml::table> client_info;
        if (!Authorizate(client_fd, buffer, request, client_info) || !client_info) {
            close(client_fd);
            return;
        }

        // 网站过滤
        auto filter = client_info->get_array_of<std::string>("host");
        if (filter)
            for (const auto &iter:*filter) {
                if (iter == request.host()) {
                    std::string authed = str(format("HTTP/%s 403 Forbidden\r\n"
                                                    "Date: %s\r\n\r\n")
                                             % request.version()
                                             % DateTime().str());
                    sent_state = send(client_fd, authed.c_str(), authed.length(), 0);
                    is_connect = false;

                    BOOST_LOG_TRIVIAL(info) << "用户到 " << request.host() << " 的访问被拒绝";
                    break;
                }
            }

        // 网站引导
        auto mapping = client_info->get_table_array("map");
        if (mapping)
            for (const auto &table : *mapping) {
                const auto src = table->get_as<std::string>("src");
                if (*src == request.host()) {
                    const auto dst = table->get_as<std::string>("dst");
                    BOOST_LOG_TRIVIAL(info) << *dst;
                    BOOST_LOG_TRIVIAL(info) << "用户到 " << request.host() << " 的访问被导向 " << *dst;
                    request.update("host", *dst);
                    break;
                }
            }

        // 认证成功
        if (is_connect && request.method() == "CONNECT") {  // HTTPS隧道代理
            std::string authed = str(format("HTTP/%s 200 Connection Established\r\n"
                                            "Poryx-Agent: Tinyproxy/0.1\r\n\r\n") % request.version());
            sent_state = send(client_fd, authed.c_str(), authed.length(), 0);
            if (sent_state < 0) is_connect = false;
        }

        // 认证成功，双方交互
        if (is_connect && ConnectToServer(&server_fd, request.host(), request.port())) {
            BOOST_LOG_TRIVIAL(info) << "代理连接主机" << request.host() << "成功";
            if (request.method() != "CONNECT")
                send(server_fd, request.source().data(), request.source().size(), 0);

            while (is_connect) {
                FD_ZERO(&read_fds); //每次循环都要清空集合，否则不能检测描述符变化
                FD_SET(client_fd, &read_fds);  // 添加描述符
                FD_SET(server_fd, &read_fds);  // 同上

                int maxfdp = client_fd > server_fd ? client_fd + 1 : server_fd + 1;  //描述符最大值加1

                switch (select(maxfdp, &read_fds, nullptr, nullptr, &timeout)) {  //select使用
                    case -1: //select错误，退出程序
                        BOOST_LOG_TRIVIAL(error) << format("select error: %s(errno: %d)") % strerror(errno) % errno;
                        is_connect = false;
                        break;
                    case 0:  //超时
                        break;
                    default:
                        // Client --> Server
                        if (FD_ISSET(client_fd, &read_fds)) {
                            recv_len = recv(client_fd, buffer, MAXSIZE, 0);
                            if (recv_len > 0) {
                                sent_state = send(server_fd, buffer, (size_t) recv_len, 0);
                                if (sent_state < recv_len) {
                                    BOOST_LOG_TRIVIAL(error)
                                        << format("cs proxy error: %s(errno: %d)") % strerror(errno) % errno;
                                    is_connect = false;
                                }
                            } else      // Client关闭链接
                                is_connect = false;
                            bzero(buffer, MAXSIZE);
                        }
                        // Server --> Client
                        if (FD_ISSET(server_fd, &read_fds)) {
                            recv_len = recv(server_fd, buffer, MAXSIZE, 0);
                            if (recv_len > 0) {
                                sent_state = send(client_fd, buffer, (size_t) recv_len, 0);
                                if (sent_state < recv_len) {
                                    BOOST_LOG_TRIVIAL(error)
                                        << format("sc proxy error: %s(errno: %d)") % strerror(errno) % errno;
                                    is_connect = false;
                                }
                            } else      // Server关闭链接
                                is_connect = false;
                            bzero(buffer, MAXSIZE);
                        }
                }
            }
        }
        BOOST_LOG_TRIVIAL(info) << "代理到 " << request.host() << " 的连接已断开";
        close(client_fd);
        close(server_fd);
    }
}

bool InitProxy(int port) {
    if ((ProxySocketFd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        BOOST_LOG_TRIVIAL(error) << format("create socket error: %s(errno: %d)") % strerror(errno) % errno;
        exit(0);
    }
    //初始化
    memset(&ProxyServerAddr, 0, sizeof(ProxyServerAddr));
    ProxyServerAddr.sin_family = AF_INET;
    ProxyServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ProxyServerAddr.sin_port = htons(port);


    // 设置端口复用
    int on = 1;
    if (setsockopt(ProxySocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
        BOOST_LOG_TRIVIAL(error) << format("set socketopt: %s(errno: %d)") % strerror(errno) % errno;
        exit(0);
    }

    // 设置超时时间
    struct timeval timeout = {3, 0};//3s
    if (setsockopt(ProxySocketFd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        BOOST_LOG_TRIVIAL(error) << format("set socketopt: %s(errno: %d)") % strerror(errno) % errno;
        exit(0);
    }

    //将本地地址绑定到所创建的套接字上
    if (bind(ProxySocketFd, (struct sockaddr *) &ProxyServerAddr, sizeof(ProxyServerAddr)) == -1) {
        BOOST_LOG_TRIVIAL(error) << format("bind socket error: %s(errno: %d)") % strerror(errno) % errno;
        exit(0);
    }
    //开始监听是否有客户端连接
    if (listen(ProxySocketFd, 10) == -1) {
        BOOST_LOG_TRIVIAL(error) << format("listen socket error: %s(errno: %d)") % strerror(errno) % errno;
        exit(0);
    }
    return true;
}

bool ConnectToServer(int *server_fd, const std::string &host, const unsigned short &port) {
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    hostent *hostent = gethostbyname(host.c_str());
    if (!hostent) return false;
    in_addr Inaddr = *((in_addr *) *hostent->h_addr_list);
    server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(Inaddr));;

    if ((*server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        return false;
    }
    if (connect(*server_fd, (struct sockaddr *) (&server_addr), sizeof(server_addr)) < 0) {
        BOOST_LOG_TRIVIAL(error) << format("connect error: %s(errno: %d)") % strerror(errno) % errno;
        close(*server_fd);
        return false;
    }
    return true;
}

void signal_handler(int) {
    BOOST_LOG_TRIVIAL(info) << "服务器正在关闭";
    isContinue = false;
    exit(EXIT_SUCCESS);
}

bool Authorizate(int client_fd, char *buffer, Request &request, std::shared_ptr<toml::table> &authinfo) {
    struct timeval timeout = {3, 0};//3s
    ssize_t recv_len, sent_state;
    fd_set fds;              // 使用select监控的套接字集合
    auto it = request.data().find("Proxy-Authorization");

    // 没有认证信息
    if (it == request.data().end()) {
        std::string auth_required = str(format("HTTP/%s 407 Proxy Authentication Required\r\n"
                                               "Date: %s\r\n"
                                               "Proxy-Authenticate: Basic\r\n\r\n")
                                        % request.version()
                                        % DateTime().str());

        FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(client_fd, &fds);  // 添加描述符
        int maxfdp = client_fd + 1;  //描述符最大值加1

        switch (select(maxfdp, nullptr, &fds, nullptr, &timeout)) {
            case -1: //select错误，退出程序
                BOOST_LOG_TRIVIAL(error) << format("select error: %s(errno: %d)") % strerror(errno) % errno;
                return false;
            case 0:  //超时
                BOOST_LOG_TRIVIAL(debug) << "发送超时";
                return false;
            default:
                // 要求认证
                sent_state = send(client_fd, auth_required.c_str(), auth_required.length(), 0);
                if (sent_state != auth_required.length()) {
                    BOOST_LOG_TRIVIAL(warning) << "要求认证信息发送失败";
                    return false;
                }
                break;
        }

        FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(client_fd, &fds);  // 添加描述符
        maxfdp = client_fd + 1;  //描述符最大值加1

        switch (select(maxfdp, &fds, nullptr, nullptr, &timeout)) {
            case -1: //select错误，退出程序
                BOOST_LOG_TRIVIAL(error) << format("select error: %s(errno: %d)") % strerror(errno) % errno;
                return false;
            case 0:  //超时
                return false;
            default:
                // 解析新请求
                bzero(buffer, MAXSIZE);
                recv_len = recv(client_fd, buffer, MAXSIZE, 0);
                if (recv_len >= 0) {
                    request.parse(buffer, static_cast<size_t>(recv_len));
                    it = request.data().find("Proxy-Authorization");
                } else {
                    return false;
                }
        }
    }

    if (it != request.data().end()) {
        std::string auth = decode64(it->second.substr(6));
        unsigned long pos = auth.find_first_of(":");
        const auto &pwd = config->get_qualified_as<std::string>(auth.substr(0, pos) + ".pwd");

        if (!pwd || *pwd != auth.substr(pos + 1)) {
            return false;
        } else {
            authinfo = config->get_table(auth.substr(0, pos));
            return true;
        }
    } else {
        return false;
    }
}
