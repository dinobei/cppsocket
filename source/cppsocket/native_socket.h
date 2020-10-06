#pragma once
#include <iostream>
#include <chrono>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include "os_specific.h"
#include "peer.h"

#ifdef __CPPSOCKET_WINDOWS__
#include <winsock2.h>
int writev(int sock_id, const struct iovec *iov, const int nvecs);
int readv(const int sock_id, const struct iovec *iov, int nvecs);

typedef char SOCKOPT_TYPE;
#else
#include <sys/uio.h>

typedef void SOCKOPT_TYPE;
#endif

namespace cppsocket
{
#if defined(__CPPSOCKET_WINDOWS__)
    class win_sock_helper
    {
    private:
        WSADATA wsa;

    public:
        win_sock_helper() { WSAStartup(MAKEWORD(2, 2), &wsa); }
        ~win_sock_helper() { WSACleanup(); }
    };
    static win_sock_helper __helper;
#endif

    class native_socket
    {

    public:
        native_socket(int sock_id);
        native_socket(protocol_family family = IPv4, socket_type type = tcp);

        ~native_socket() { this->close(); }

        void close();
        bool close(enum close_type type);

    public:
        bool safe_send(char *buffer, const int bytes);
        bool safe_send(char *buffer, const int offset, const int bytes, const int opt = 0);
        bool safe_recv(char *buffer, const int bytes);
        bool safe_recv(char *buffer, const int offset, const int bytes, const int opt = 0);

        int writev(const struct iovec *iov, const int iovcount);
        int readv(const struct iovec *iov, const int iovcount);

        bool connect(std::string hostname, std::string service, int ms = 1000);
        bool bind(int port, cppsocket::protocol_family family, std::string hostaddr);

    protected:
        void create_socket();

    public:
        // for common udp
        ssize_t sendto(peer *peer, char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t sendto(peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt = 0);
        ssize_t recvfrom(peer *peer, char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t recvfrom(peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt = 0);

        // for connected udp
        ssize_t send(char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t send(char *buffer, const int offset, const unsigned int bytes, const int opt = 0);
        ssize_t recv(char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t recv(char *buffer, const int offset, const unsigned int bytes, const int opt = 0);

    public:
        bool option(socket_option_type type, void *arg);

    public:
        int get_socket_identifier() { return this->sock_id; }
        void set_socket_identifier(int sock_id);
        socket_type get_socket_type();
        protocol_family get_protocol_family();

        static int domain2address(const char *host, address *addr); // Support IPv4, IPv6

    private:
        void assign_peer_info(int sock_id);
        std::string get_sock_ip();
        std::string get_sock_port();
        std::string get_peer_ip();
        std::string get_peer_port();
#if !defined(__CPPSOCKET_WINDOWS__)
        void delay_if_needed(long int timeout_ms);
        int connect_nonblock(const struct sockaddr *saptr, int salen, const int ms);
#endif

    public:
        std::string sock_ip;
        std::string sock_port;
        std::string peer_ip;
        std::string peer_port;

    private:
        std::chrono::milliseconds last = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    protected:
        int sock_id;
        protocol_family family;
        socket_type type;
    };

}
