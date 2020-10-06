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
    class udp_socket : public native_socket
    {
    public:
        udp_socket(protocol_family family = IPv4);
        udp_socket(int port, std::string addr = std::string());
        udp_socket(protocol_family family, int port, std::string addr = std::string());
        ~udp_socket();

    private:
        bool initialize(int port, std::string addr = std::string());
        bool initialize(protocol_family family, int port, std::string addr = std::string());
    };
}
