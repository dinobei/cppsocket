#pragma once
#include <iostream>
#include <memory>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include "os_specific.h"
#include "native_socket.h"

namespace cppsocket
{
    static std::string __base_addr = "";
    static int __base_backlog = 10;
    static protocol_family __base_family = IPv4;

    class tcp_socket : public native_socket
    {
    public:
        tcp_socket(ushort port, std::string addr = __base_addr, int backlog = __base_backlog, protocol_family family = __base_family);
        tcp_socket(int sock);
        tcp_socket(protocol_family family = __base_family);

    public:
        std::shared_ptr<tcp_socket> accept();
        int event(int msec);

    private:
        bool initialize(int port, int backlog, std::string addr, protocol_family family);
    };
}
