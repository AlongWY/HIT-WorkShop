//
// Created by along on 17-11-12.
//
#include "HttpServer.h"

// 用于字符串处理
#include <iostream>
#include <cstring>

// 用于套接字
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// 用于接收用户的终止命令
#include <csignal>

// 多线程处理
#include <thread>

// 日志处理
#include <boost/log/trivial.hpp>

// 对接收的数据进行处理
#include <vector>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <fstream>
#include <wait.h>

using namespace std;

// 宏定义，用于控制缓冲区大小
#define BUF_SIZE     (4096)

// 宏定义，HTTP服务器版本信息
#define SERVER_STRING "Server: miniHttpServer/0.1.0\r\n"

/** 用于接收退出信号 */
void signalHandle(int signum);

HttpServer::HttpServer(const unsigned short &u_port) : port(u_port), serverSock(0) {
    sockaddr_in name{};
    // 初始化
    // 流式 Socket（SOCK_STREAM）是一种面向连接的 Socket，针对于面向连接的 TCP 服务应用
    serverSock = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == serverSock) {
        BOOST_LOG_TRIVIAL(error) << "Socket Init Failed!";
        exit(EXIT_FAILURE);
    } else
        BOOST_LOG_TRIVIAL(info) << "Socket Init Successfully!";
    memset(&name, 0, sizeof(name));

    // AF_INET 决定了要用 ipv4 地址（32位的）与端口号（16位的）的组合
    name.sin_family = AF_INET;
    // 使用htons进行转序，本机的字节序转换成网路字节序
    name.sin_port = htons(port);
    // 接受任何信息
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    // 地址族中的特定地址赋给socket。
    // 例如对应 AF_INET、AF_INET6就是把一个 ipv4 或 ipv6 地址和端口号组合赋给socket
    if (bind(serverSock, (sockaddr *) &name, sizeof(name)) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Bind Failed!";
        exit(EXIT_FAILURE);
    } else
        BOOST_LOG_TRIVIAL(info) << "Socket Bind Successfully!";

    // port = 0时进行动态分配地址
    if (port == 0) {
        socklen_t nameLen = sizeof(name);
        if (getsockname(serverSock, (sockaddr *) &name, &nameLen) == -1) {
            BOOST_LOG_TRIVIAL(error) << "Get Sock Name Failed!";
            exit(EXIT_FAILURE);
        } else {
            port = ntohs(name.sin_port);
            BOOST_LOG_TRIVIAL(info) << "Dynamically allocating a port!";
        }
    }
    // 对套接字进行监听
    if (listen(serverSock, 5) < 0) {
        BOOST_LOG_TRIVIAL(error) << "Listen Failed!";
        exit(EXIT_FAILURE);
    } else
        BOOST_LOG_TRIVIAL(info) << "Start Listening(port:" << port << ")!";

    // 注册退出程序
    signal(SIGINT, signalHandle);
}

HttpServer::~HttpServer() {
    close(serverSock);
}

void HttpServer::run() {
    sockaddr_in clientName{};
    socklen_t clientNameLen = sizeof(clientName);
    // 建立连接进行处理
    int clientSock = -1;
    while (true) {
        clientSock = accept(serverSock, (sockaddr *) &clientName, &clientNameLen);
        if (-1 == clientSock)
            BOOST_LOG_TRIVIAL(error) << "Accept failed";
        // 开启新进程进行数据处理
        thread requestHandle(acceptRequestWrapper, this, clientSock);
        requestHandle.join();
    }

}

void HttpServer::acceptRequest(int client) {
    /** 缓冲区 */
    char buf[BUF_SIZE];
    memset(buf, 0, sizeof(buf));
    ssize_t msgSize;
    BOOST_LOG_TRIVIAL(info) << "Link(" << client << ") connect";
    msgSize = read(client, buf, sizeof(buf));

    if (msgSize == -1 && errno == EINTR) {
        BOOST_LOG_TRIVIAL(error) << "Read message error";
        exit(EXIT_FAILURE);
    }
    if (msgSize > 0) {
#ifndef NDEBUG
        cout << buf << endl;
#endif
        // 解析报文数据
        HttpParser parser(string(buf, static_cast<unsigned long>(msgSize)));
        HttpExecute execute(parser);
        execute.execute(client);
    }
    close(client);

#ifndef NDEBUG
    BOOST_LOG_TRIVIAL(info) << "Link(" << client << ") closed";
#endif
}

void HttpServer::acceptRequestWrapper(HttpServer *server, int client) {
    server->acceptRequest(client);
}

void signalHandle(int signum) {
    BOOST_LOG_TRIVIAL(info) << "Interrupt signal (" << signum << ") received.";
    exit(signum);
}

HttpParser::HttpParser(std::string msg)
    : method(UNKNOWN), version(ONE), url("./webDocs"), content(""),
      cookie(""), contentType(HTML), cgi(false), contentLen(0) {
    initRequest(getLine(msg));
    for (string line = getLine(msg); !line.empty(); line = getLine(msg)) {
        vector<string> tokens;
        boost::split_regex(tokens, line, boost::regex(": "));
        if (tokens.size() > 1) {
            if (tokens[0] == "Cookie")
                cookie = tokens[1];
            else if (tokens[0] == "Content-Length")
                contentLen = stoul(tokens[1]);
        } else {
            cgi = true;
            content = line;
        }
    }
    if (string::npos != url.find('?')) {
        cgi = true;
        auto pos = url.find('?');
        queryString = url.substr(pos + 1, string::npos);
        url.resize(pos);
    }
    content = msg;
    if (content.size() > 0)
        cgi = true;
    BOOST_LOG_TRIVIAL(info) << "HTML头解析完毕";
}

void HttpParser::initRequest(const string &line) {
    vector<string> tokens;
    boost::algorithm::split(tokens, line, boost::is_any_of(" "));

    // 处理请求方法
    if (tokens[0] == "GET")
        method = GET;
    else if (tokens[0] == "POST")
        method = POST;

    url += tokens[1];
    if (url.at(url.size() - 1) == '/')
        url.append("index.html");
    version = tokens[2] == "1.1" ? ONE : ZERO;
}

/** 从消息中读取一行并把这一行删除掉 */
std::string HttpParser::getLine(std::string &msg) {
    size_t pos = msg.find("\r\n");
    string res = msg.substr(0, pos);
    msg.erase(0, pos + 2);
    return res;
}

HttpExecute::HttpExecute(HttpParser &httpParser) : parser(httpParser) {}

void HttpExecute::execute(int &client) {
    string message;
    ifstream file;
    CONTENT_TYPE contentType;
    if (parser.url.find(".html") != string::npos || parser.url.find(".cgi") != string::npos)
        contentType = HTML;
    else if (parser.url.find(".js") != string::npos)
        contentType = JS;
    else if (parser.url.find(".css") != string::npos)
        contentType = CSS;
    else if (parser.url.find(".svg") != string::npos)
        contentType = SVG;
    else if (parser.url.find(".png") != string::npos)
        contentType = PNG;
    else if (parser.url.find(".jpeg") != string::npos)
        contentType = JPG;
    else if (parser.url.find(".less") != string::npos)
        contentType = LESS;
    else
        contentType = ANY;

    cout << parser.url << endl;

    // 处理各种方法
    switch (parser.method) {
    case GET:
        if (!parser.cgi) {
            file.open(parser.url, ios::binary);
            if (!file) {
                message = notFound();
                message = headers(404, message.size()) + message;
                break;
            }
            message = serveFile(file);
        } else {
            message = executeCgi();
        }
        message = headers(200, message.size(), contentType) + message;
        break;
    case POST:
        if (!parser.cgi) {
            message = notFound();
        } else {
            message = executeCgi();
        }
        message = headers(200, message.size()) + message;
        break;
    default: message = unImplemented();
        message = headers(501, message.size()) + message;
        break;
    }

#ifndef NDEBUG
    cout << message << endl;
#endif
    send(client, message.c_str(), message.size(), 0);
}

string HttpExecute::unImplemented() {
    string msg("<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    msg += "</TITLE></HEAD>\r\n";
    msg += "<BODY><P>HTTP request method not supported.\r\n";
    msg += "</BODY></HTML>\r\n";
    return msg;
}

string HttpExecute::notFound() {
    string msg("<HTML><TITLE>Not Found</TITLE>\r\n");
    msg += "<BODY><P>The server could not fulfill\r\n";
    msg += "your request because the resource specified\r\n";
    msg += "is unavailable or nonexistent.\r\n";
    msg += "</BODY></HTML>\r\n";
    return msg;
}

std::string HttpExecute::headers(unsigned int state, unsigned long contentLen, CONTENT_TYPE type) {
    string header("HTTP/1.1 ");
    switch (state) {
    case 200:header += "200 OK\r\n";
        break;
    case 404:header += "404 NOT FOUND\r\n";
        break;
    case 400:header += "400 BAD REQUEST\r\n";
        break;
    case 501:header += "500 Internal Server Error\r\n";
        break;
    default:header += "501 Method Not Implemented\r\n";
    }
    switch (type) {
    default:
    case HTML:header += "Content-Type: text/html\r\n";
        break;
    case JS:header += "Content-Type: text/javascript\r\n";
        break;
    case CSS:header += "Content-Type: text/css\r\n";
        break;
    case SVG:header += "Content-Type: image/svg\r\n";
        break;
    case PNG:header += "Content-Type: image/png\r\n";
        break;
    case JPG:header += "Content-Type: image/jpeg\r\n";
        break;
    case LESS:header += "Content-Type: text/less\r\n";
        break;
    case ANY:header += "Content-Type: */*\r\n";
        break;
    }
    header += "Content-Length: " + to_string(contentLen) + "\r\nConnection: close\r\n\r\n";
    return header;
}

std::string HttpExecute::serveFile(std::istream &file) {
    string msg;
    char aByte;
    while (!file.eof()) {
        file.read(&aByte, sizeof(char));
        msg += aByte;
    }
    return msg;
}

std::string HttpExecute::cannotExecute() {
    return "<P>Error prohibited CGI execution.\r\n";
}

std::string HttpExecute::executeCgi() {
    pid_t pid;
    // cgi管道重定向
    int cgiInput[2];
    int cgiOutput[2];

    int status;

    string msg;

    if ((pipe(cgiInput) < 0) || (pipe(cgiOutput) < 0) || ((pid = fork()) < 0))
        return cannotExecute();

    if (0 == pid) { /** 子进程 */
        dup2(cgiOutput[1], 1);
        dup2(cgiInput[0], 0);
        close(cgiOutput[0]);
        close(cgiInput[1]);
        execl(parser.url.c_str(), parser.queryString.c_str(), nullptr);

        BOOST_LOG_TRIVIAL(info) << "CGI执行完毕";

        exit(EXIT_SUCCESS);
    } else { /** 父进程 */
        close(cgiOutput[1]);
        close(cgiInput[0]);

        if (parser.method == POST) {
            write(cgiInput[1], parser.content.c_str(), parser.content.size());
        }
        char ch;
        while (read(cgiOutput[0], &ch, 1) > 0)
            msg += ch;
        close(cgiOutput[0]);
        close(cgiInput[1]);
        waitpid(pid, &status, 0);
    }

    return msg;
}

