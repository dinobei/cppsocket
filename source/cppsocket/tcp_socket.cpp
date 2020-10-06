#include "tcp_socket.h"

cppsocket::tcp_socket::tcp_socket(ushort port, std::string addr, int backlog, protocol_family family) : cppsocket::native_socket(family, cppsocket::tcp)
{
    if (!initialize(port, backlog, addr, family))
        exit(0);
}

cppsocket::tcp_socket::tcp_socket(int sock) : native_socket(sock)
{
    assert(this->get_socket_type() == cppsocket::tcp);
}

cppsocket::tcp_socket::tcp_socket(protocol_family family) : native_socket(family, cppsocket::tcp) {}

std::shared_ptr<cppsocket::tcp_socket> cppsocket::tcp_socket::accept()
{
    struct sockaddr_in client_addr;
    int size = sizeof(struct sockaddr_in);

    int csock;
#if defined(__CPPSOCKET_WINDOWS__)
    csock = ::accept(sock_id, (struct sockaddr *)&client_addr, (int *)&size);
#else
    csock = ::accept(this->sock_id, (struct sockaddr *)&client_addr, (socklen_t *)&size);
#endif

    char buffer[INET6_ADDRSTRLEN] = {
        0,
    };
    inet_ntop(AF_INET, &client_addr.sin_addr, buffer, INET6_ADDRSTRLEN);
    auto clientSocket = std::shared_ptr<cppsocket::tcp_socket>(new cppsocket::tcp_socket(csock));

    return clientSocket;
}

bool cppsocket::tcp_socket::initialize(int port, int backlog, std::string addr, protocol_family family)
{
    this->create_socket();
    int option = 1;
    this->option(cppsocket::SOCK_REUSE, (void *)&option);
    if (!bind(port,
              this->get_protocol_family(),
              addr))
    {
        std::cout << "Bind Failed" << std::endl;
        return false;
    }

    int retv = ::listen(this->sock_id, backlog);
    if (retv != 0)
    {
        std::cout << "Listen Failed - " << retv << std::endl;
        return false;
    }

    sock_ip = addr;
    sock_port = std::to_string(port);

    return true;
}

int cppsocket::tcp_socket::event(int msec)
{
    fd_set fdset;

    FD_ZERO(&fdset);
    FD_SET(this->sock_id, &fdset);

    struct timeval tv;

    tv.tv_sec = (int)(msec / 1000);
    tv.tv_usec = (msec % 1000) * 1000;

    return select(this->sock_id + 1, &fdset, NULL, NULL, &tv);
}
