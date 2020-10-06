#include "udp_socket.h"

cppsocket::udp_socket::udp_socket(protocol_family family) : native_socket(family, cppsocket::udp)
{
    this->create_socket();
}

cppsocket::udp_socket::udp_socket(int port, std::string addr) : cppsocket::native_socket(IPv4, cppsocket::udp)
{
    this->create_socket();
    if (!initialize(IPv4, port, addr))
        exit(0);
}

cppsocket::udp_socket::udp_socket(protocol_family family, int port, std::string addr) : cppsocket::native_socket(family, cppsocket::udp)
{
    this->create_socket();
    if (!initialize(family, port, addr))
        exit(0);
}

cppsocket::udp_socket::~udp_socket()
{
    this->close();
}

bool cppsocket::udp_socket::initialize(int port, std::string addr)
{
    return initialize(cppsocket::IPv4, port, addr);
}

bool cppsocket::udp_socket::initialize(protocol_family family, int port, std::string addr)
{
    if (!bind(port,
              this->get_protocol_family(),
              addr))
    {
        std::cout << "Bind Failed" << std::endl;
        return false;
    }

    return true;
}
