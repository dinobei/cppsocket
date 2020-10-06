#pragma once
#include <iostream>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

namespace cppsocket
{
    typedef struct
    {
        char host[255];   // host length up to 253
        char address[40]; // address length up to 39 (IPv6)
        int ns_family;
    } NSAddress_D;

    enum NS_RESULT
    {
        NS_SUCCESS = 0,
        NS_INVALID_PARAMETERS = -1,
        NS_INVALID_HOST_NAME = -2,
        NS_UNKNOWN_ERROR = -3,

        NS_SOCKET_ERROR = -4,
        NS_CONNECT_ERROR = -5,
        NS_INVALID_PORT = -6,
    };

    enum ProtocolFamily
    {
        IPv4 = 0,
        IPv6,
    };

    enum SocketOptionType
    {
        SOCK_REUSE = 0,
        SOCK_NODELAY = 1,
        SOCK_LINGER = 2,
        SOCK_RCVTIMEO_MS = 3,
        SOCK_SNDTIMEO_MS = 4,
        SOCK_RCVBUFFER = 5,
        SOCK_SNDBUFFER = 6,
        SOCK_BROADCAST = 7,
        SOCK_IP_MULTICAST_TTL = 8,
        SOCK_IP_ADD_MEMBERSHIP = 9,
    };

    enum SocketType
    {
        tcp = 0,
        udp,
    };

    enum CloseType
    {
        read = 0,
        write,
        both,
    };

    // Family
#define NS_FAMILY_INET 0
#define NS_FAMILY_INET6 1

    class Peer
    {
    public:
        Peer() : sockLen(sizeof(this->sockAddr))
        {
            memset(&sockAddr, 0, sizeof(sockAddr));
            sockAddr.sin_family = AF_INET;
        }

        Peer(std::string ip, std::string port) : sockLen(sizeof(this->sockAddr))
        {
            memset(&sockAddr, 0, sizeof(sockAddr));
            sockAddr.sin_family = AF_INET;

            inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr.s_addr);
            sockAddr.sin_port = htons(atoi(port.c_str()));
        }

        Peer(ProtocolFamily family) : sockLen(sizeof(this->sockAddr))
        {
            memset(&sockAddr, 0, sizeof(sockAddr));
            if (family == ProtocolFamily::IPv4)
                sockAddr.sin_family = AF_INET;
            else
                sockAddr.sin_family = AF_INET6;
        }

        Peer(ProtocolFamily family, std::string ip, std::string port) : sockLen(sizeof(this->sockAddr))
        {
            memset(&sockAddr, 0, sizeof(sockAddr));
            if (family == ProtocolFamily::IPv4)
                sockAddr.sin_family = AF_INET;
            else
                sockAddr.sin_family = AF_INET6;

            inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr.s_addr);
            sockAddr.sin_port = htons(atoi(port.c_str()));
        }

        std::string getIP()
        {
            char addr[INET6_ADDRSTRLEN] = {
                0,
            };
            inet_ntop(AF_INET, &sockAddr.sin_addr, addr, INET6_ADDRSTRLEN);
            return addr;
        }

        void setIP(std::string ip)
        {
            inet_pton(AF_INET, ip.c_str(), &sockAddr.sin_addr.s_addr);
        }

        int getPort()
        {
            return ntohs(sockAddr.sin_port);
        }

        void setPort(std::string port)
        {
            sockAddr.sin_port = htons(atoi(port.c_str()));
        }

        ProtocolFamily getProtocolFamily()
        {
            if (sockAddr.sin_family == AF_INET)
            {
                return IPv4;
            }
            else if (sockAddr.sin_family == AF_INET)
            {
                return IPv6;
            }
            else
            {
                assert(false);
            }
        }

        void setProtocolFamily(ProtocolFamily family)
        {
            sockAddr.sin_family = (family == IPv4) ? IPv4 : IPv6;
        }

        std::string getKey()
        {
            return getIP() + ":" + std::to_string(getPort());
        }

    public:
        sockaddr_in sockAddr;
        socklen_t sockLen;
    };
}
