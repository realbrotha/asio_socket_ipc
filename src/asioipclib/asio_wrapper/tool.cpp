//
// Created by realbro on 19. 6. 21.
//

#include "tool.h"
#include <unistd.h>
#include <errno.h>

namespace network { namespace tool {

bool check_ip6_support(boost::system::error_code* ec)
{
    if (access("/proc/net/if_inet6", F_OK) == 0)
    {
        if (ec) {
            ec->assign(0, boost::system::generic_category());
        }
        return true;
    }
    else if (ec)
    {
        ec->assign(errno, boost::system::generic_category());
    }
    return false;
}


bool check_interval_over(uint32_t target, uint32_t interval)
{
    if (time(NULL) - target >= interval)
    {
        return true;
    }
    return false;
}

} // namespace tool
} // namespace network
