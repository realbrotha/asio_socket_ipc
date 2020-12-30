//
// Created by realbro on 19. 6. 21.
//

#ifndef UTIL_TOOL_H
#define UTIL_TOOL_H

#include <boost/system/error_code.hpp>
#include <stdint.h>

namespace network { namespace tool {
//    std::vector<std::string> ip_list;
//    ip_list.push_back("127.0.0.1"); // ipv4
//    ip_list.push_back("::1"); // ipv6
//
//    for (size_t i=0; i<ip_list.size(); ++i) {
//        if (SetIP(ip_list[i])) {
//            continue;
//        }
//        break;
//    }
//    return ec_;
bool check_ip6_support(boost::system::error_code* ec);
bool check_interval_over(uint32_t target, uint32_t interval);


} // namespace tool
} // namespace network

#endif //UTIL_TOOL_H
