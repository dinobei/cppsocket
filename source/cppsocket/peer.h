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
    } address;

    enum result
    {
        NS_SUCCESS = 0,
        NS_INVALID_PARAMETERS = -1,
        NS_INVALID_HOST_NAME = -2,
        NS_UNKNOWN_ERROR = -3,

        NS_SOCKET_ERROR = -4,
        NS_CONNECT_ERROR = -5,
        NS_INVALID_PORT = -6,
    };

    enum protocol_family
    {
        IPv4 = 0,
        IPv6,
    };

    enum socket_option_type
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

    enum socket_type
    {
        tcp = 0,
        udp,
    };

    enum close_type
    {
        read = 0,
        write,
        both,
    };

    // Family
#define NS_FAMILY_INET 0
#define NS_FAMILY_INET6 1

    class peer
    {
    public:
        peer() : sock_len(sizeof(this->sock_addr))
        {
            memset(&sock_addr, 0, sizeof(sock_addr));
            sock_addr.sin_family = AF_INET;
        }

        peer(std::string ip, std::string port) : sock_len(sizeof(this->sock_addr))
        {
            memset(&sock_addr, 0, sizeof(sock_addr));
            sock_addr.sin_family = AF_INET;

            inet_pton(AF_INET, ip.c_str(), &sock_addr.sin_addr.s_addr);
            sock_addr.sin_port = htons(atoi(port.c_str()));
        }

        peer(protocol_family family) : sock_len(sizeof(this->sock_addr))
        {
            memset(&sock_addr, 0, sizeof(sock_addr));
            if (family == protocol_family::IPv4)
                sock_addr.sin_family = AF_INET;
            else
                sock_addr.sin_family = AF_INET6;
        }

        peer(protocol_family family, std::string ip, std::string port) : sock_len(sizeof(this->sock_addr))
        {
            memset(&sock_addr, 0, sizeof(sock_addr));
            if (family == protocol_family::IPv4)
                sock_addr.sin_family = AF_INET;
            else
                sock_addr.sin_family = AF_INET6;

            inet_pton(AF_INET, ip.c_str(), &sock_addr.sin_addr.s_addr);
            sock_addr.sin_port = htons(atoi(port.c_str()));
        }

        std::string get_ip()
        {
            char addr[INET6_ADDRSTRLEN] = {
                0,
            };
            inet_ntop(AF_INET, &sock_addr.sin_addr, addr, INET6_ADDRSTRLEN);
            return addr;
        }

        void set_ip(std::string ip)
        {
            inet_pton(AF_INET, ip.c_str(), &sock_addr.sin_addr.s_addr);
        }

        int get_port()
        {
            return ntohs(sock_addr.sin_port);
        }

        void set_port(std::string port)
        {
            sock_addr.sin_port = htons(atoi(port.c_str()));
        }

        protocol_family get_protocol_family()
        {
            if (sock_addr.sin_family == AF_INET)
            {
                return IPv4;
            }
            else if (sock_addr.sin_family == AF_INET)
            {
                return IPv6;
            }
            else
            {
                assert(false);
            }
        }

        void set_protocol_family(protocol_family family)
        {
            sock_addr.sin_family = (family == IPv4) ? IPv4 : IPv6;
        }

        std::string get_key()
        {
            return get_ip() + ":" + std::to_string(get_port());
        }

    public:
        sockaddr_in sock_addr;
        socklen_t sock_len;
    };
}
