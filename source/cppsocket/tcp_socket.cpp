#include "tcp_socket.h"

cppsocket::TCPSocket::TCPSocket(ushort port, std::string addr, int backlog, ProtocolFamily family) : cppsocket::NativeSocket(family, cppsocket::tcp)
{
    if (!initialize(port, backlog, addr, family))
        exit(0);
}

cppsocket::TCPSocket::TCPSocket(int sock) : NativeSocket(sock)
{
    assert(this->getSocketType() == cppsocket::tcp);
}

cppsocket::TCPSocket::TCPSocket(ProtocolFamily family) : NativeSocket(family, cppsocket::tcp) {}

std::shared_ptr<cppsocket::TCPSocket> cppsocket::TCPSocket::accept()
{
    struct sockaddr_in client_addr;
    int size = sizeof(struct sockaddr_in);

    int csock;
#if defined(__CPPSOCKET_WINDOWS__)
    csock = ::accept(sockId, (struct sockaddr *)&client_addr, (int *)&size);
#else
    csock = ::accept(this->sockId, (struct sockaddr *)&client_addr, (socklen_t *)&size);
#endif

    char buffer[INET6_ADDRSTRLEN] = {
        0,
    };
    inet_ntop(AF_INET, &client_addr.sin_addr, buffer, INET6_ADDRSTRLEN);
    auto clientSocket = std::shared_ptr<cppsocket::TCPSocket>(new cppsocket::TCPSocket(csock));

    return clientSocket;
}

bool cppsocket::TCPSocket::initialize(int port, int backlog, std::string addr, ProtocolFamily family)
{
    this->createSocket();
    int option = 1;
    this->option(cppsocket::SOCK_REUSE, (void *)&option);
    if (!bind(port,
              this->getProtocolFamily(),
              addr))
    {
        std::cout << "Bind Failed" << std::endl;
        return false;
    }

    int retv = ::listen(this->sockId, backlog);
    if (retv != 0)
    {
        std::cout << "Listen Failed - " << retv << std::endl;
        return false;
    }

    sockIP = addr;
    sockPort = std::to_string(port);

    return true;
}

int cppsocket::TCPSocket::event(int msec)
{
    fd_set nFDSet;

    FD_ZERO(&nFDSet);
    FD_SET(this->sockId, &nFDSet);

    struct timeval tTimeValue;

    tTimeValue.tv_sec = (int)(msec / 1000);
    tTimeValue.tv_usec = (msec % 1000) * 1000;

    return select(this->sockId + 1, &nFDSet, NULL, NULL, &tTimeValue);
}
