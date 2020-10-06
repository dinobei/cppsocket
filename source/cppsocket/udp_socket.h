#pragma once
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include "native_socket.h"

namespace cppsocket
{
    class UDPSocket : public NativeSocket
    {
    public:
        UDPSocket(ProtocolFamily family = IPv4);
        UDPSocket(int port, std::string addr = std::string());
        UDPSocket(ProtocolFamily family, int port, std::string addr = std::string());
        ~UDPSocket();

    private:
        bool initialize(int port, std::string addr = std::string());
        bool initialize(ProtocolFamily family, int port, std::string addr = std::string());
    };
}
