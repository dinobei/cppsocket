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
    static ProtocolFamily __base_family = IPv4;

    class TCPSocket : public NativeSocket
    {
    public:
        TCPSocket(ushort port, std::string addr = __base_addr, int backlog = __base_backlog, ProtocolFamily family = __base_family);
        TCPSocket(int sock);
        TCPSocket(ProtocolFamily family = __base_family);

    public:
        std::shared_ptr<TCPSocket> accept();
        int event(int msec);

    private:
        bool initialize(int port, int backlog, std::string addr, ProtocolFamily family);
    };
}
