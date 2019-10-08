/**
 * Copyright (c) 2018 along All rights reserved.
 **/

#ifndef TINYPROXY_DATETIME_H
#define TINYPROXY_DATETIME_H

#include <string>
#include <boost/date_time/local_time/local_time.hpp>

class DateTime {
public:
    DateTime();
    DateTime(const std::string& path);

    // return datetime string
    std::string str();

    // update datetime from file mod date
    std::string from_file(const std::string& path);

    // parse datetime string
    void parse(const std::string& dt);

    // boolean equal operator
    friend bool operator==(const DateTime& left, const DateTime& right);

private:
    boost::local_time::local_date_time m_dt;
};


#endif //TINYPROXY_DATETIME_H
