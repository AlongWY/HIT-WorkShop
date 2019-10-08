/**
 * Copyright (c) 2018 along All rights reserved.
 **/

#include "DateTime.h"

#include <sstream>
#include <boost/date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/operations.hpp>

DateTime::DateTime()
        : m_dt(boost::local_time::local_sec_clock::local_time(boost::local_time::time_zone_ptr())) {
}

DateTime::DateTime(const std::string &path)
        : m_dt(boost::local_time::local_sec_clock::local_time(boost::local_time::time_zone_ptr())) {
    from_file(path);
}

std::string DateTime::str() {
    std::string result;
    boost::local_time::local_time_facet *lf(new boost::local_time::local_time_facet("%a, %d %b %Y %H:%M:%S GMT"));
    try {
        std::stringstream ss;
        ss.imbue(std::locale(ss.getloc(), lf));
        ss << m_dt;
        result = ss.str();
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    return result;
}

std::string DateTime::from_file(const std::string &path) {
    try {
        boost::filesystem::path p(path);
        boost::posix_time::ptime pt = boost::posix_time::from_time_t(
                boost::filesystem::last_write_time(p));
        m_dt = boost::local_time::local_date_time(pt, boost::local_time::time_zone_ptr());
    }
    catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    return str();
}

void DateTime::parse(const std::string &dt) {
    boost::local_time::local_time_input_facet *lif(
            new boost::local_time::local_time_input_facet("%a, %d %b %Y %H:%M:%S GMT"));
    std::stringstream ss(dt);
    ss.imbue(std::locale(std::locale::classic(), lif));
    ss >> m_dt;
}

bool operator==(const DateTime &left, const DateTime &right) {
    return (left.m_dt == right.m_dt);
}