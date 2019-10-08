//
// Created by along on 17-11-12.
//

#ifndef HTTPSERVER_HTTPSERVER_H
#define HTTPSERVER_HTTPSERVER_H

#include <string>
#include <map>

/**
 * 支持的方法和协议版本
 */
enum METHOD { UNKNOWN, GET, POST };
enum VERSION { ZERO, ONE };
enum CONTENT_TYPE { ANY, HTML, JS, CSS, SVG, PNG, JPG, LESS };

/**
 * HTTP服务器类
 */
class HttpServer {
public:
    /**
     * 使用监听端口进行初始化
     * @param port
     */
    explicit HttpServer(const unsigned short &port = 0);
    ~HttpServer();

    void run();

private:
    /** 包装函数 */
    static void acceptRequestWrapper(HttpServer *server, int client);
    /** 监听端口 */
    unsigned short port;
    /** 接收请求，对请求进行处理 */
    void acceptRequest(int client);
    /** 套接字描述符,这里用于HttpServer */
    int serverSock;
};

/**
 * Http协议的解析类
 */
struct HttpParser {
public:
    explicit HttpParser(std::string Msg);
    ~HttpParser() = default;
    METHOD method;
    /** HTTP协议版本 */
    VERSION version;
    /** Cookie */
    std::string cookie;
    /** 请求资源地址 */
    std::string url;
    /** 附加信息长度 */
    unsigned long contentLen;
    /** 附加信息 */
    std::string content;
    /** 附加信息种类 */
    CONTENT_TYPE contentType;
    /** 是否需要执行CGI */
    bool cgi;
    /** CGI信息 */
    std::string queryString;
    //TODO 解析不够全，会出现问题。
private:
    void initRequest(const std::string &line);
    std::string getLine(std::string &msg);
};

/**
 * HTTP报文生成
 */
class HttpExecute {
public:
    explicit HttpExecute(HttpParser &parser);
    ~HttpExecute() = default;
    void execute(int &client);
private:
    const HttpParser &parser;
    /** 报文头 */
    std::string headers(unsigned int state, unsigned long contentLen, CONTENT_TYPE type = HTML);
    /** 读取网页文件 */
    std::string serveFile(std::istream &fileStream);
    /** 执行CGI */
    std::string executeCgi();
    /** CGI脚本执行失败 */
    std::string cannotExecute();
    /** 未实现的方法 */
    std::string unImplemented();
    /** 404 not found */
    std::string notFound();
};

#endif //HTTPSERVER_HTTPSERVER_H
