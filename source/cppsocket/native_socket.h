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
int writev(int sockId, const struct iovec *iov, const int nvecs);
int readv(const int sockId, const struct iovec *iov, int nvecs);

typedef char SOCKOPT_TYPE;
#else
#include <sys/uio.h>

typedef void SOCKOPT_TYPE;
#endif

namespace cppsocket
{
#if defined(__CPPSOCKET_WINDOWS__)
    class WinSockHelper
    {
    private:
        WSADATA wsa;

    public:
        WinSockHelper() { WSAStartup(MAKEWORD(2, 2), &wsa); }
        ~WinSockHelper() { WSACleanup(); }
    };
    static WinSockHelper __helper;
#endif

    class NativeSocket
    {

    public:
        NativeSocket(int sockId);
        NativeSocket(ProtocolFamily family = IPv4, SocketType type = tcp);

        ~NativeSocket() { this->close(); }

        void close();
        bool close(enum CloseType type);

    public:
        bool safeSend(char *buffer, const int bytes);
        bool safeSend(char *buffer, const int offset, const int bytes, const int opt = 0);
        bool safeRecv(char *buffer, const int bytes);
        bool safeRecv(char *buffer, const int offset, const int bytes, const int opt = 0);

        int writev(const struct iovec *iov, const int iovcount);
        int readv(const struct iovec *iov, const int iovcount);

        bool connect(std::string hostname, std::string service, int ms);
        bool bind(int port, cppsocket::ProtocolFamily family, std::string hostaddr);

    protected:
        void createSocket();

    public:
        // for common udp
        ssize_t sendTo(Peer *peer, char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t sendTo(Peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt = 0);
        ssize_t recvFrom(Peer *peer, char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t recvFrom(Peer *peer, char *buffer, const unsigned int offset, const unsigned int bytes, const int opt = 0);

        // for connected udp
        ssize_t send(char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t send(char *buffer, const int offset, const unsigned int bytes, const int opt = 0);
        ssize_t recv(char *buffer, const unsigned int bytes, const int opt = 0);
        ssize_t recv(char *buffer, const int offset, const unsigned int bytes, const int opt = 0);

    public:
        bool option(SocketOptionType type, void *arg);

    public:
        int getSocketIdentifier() { return this->sockId; }
        void setSocketIdentifier(int socketId);
        SocketType getSocketType();
        ProtocolFamily getProtocolFamily();

        static int Domain2address(const char *host, NSAddress_D *addr); // Support IPv4, IPv6

    private:
        void assignPeerInfo(int sockId);
        std::string getSockIP();
        std::string getSockPort();
        std::string getPeerIP();
        std::string getPeerPort();
#if !defined(__CPPSOCKET_WINDOWS__)
        void delayIfNeeded(long int timeout_ms);
        int connectNonblock(const struct sockaddr *saptr, int salen, const int ms);
#endif

    public:
        std::string sockIP;
        std::string sockPort;
        std::string peerIP;
        std::string peerPort;

    private:
        std::chrono::milliseconds last = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

    protected:
        int sockId;
        ProtocolFamily family;
        SocketType type;
    };

}
