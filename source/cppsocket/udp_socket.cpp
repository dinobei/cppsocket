#include "udp_socket.h"

cppsocket::UDPSocket::UDPSocket(ProtocolFamily family) : NativeSocket(family, cppsocket::udp)
{
    this->createSocket();
}

cppsocket::UDPSocket::UDPSocket(int port, std::string addr) : cppsocket::NativeSocket(IPv4, cppsocket::udp)
{
    this->createSocket();
    if (!initialize(IPv4, port, addr))
        exit(0);
}

cppsocket::UDPSocket::UDPSocket(ProtocolFamily family, int port, std::string addr) : cppsocket::NativeSocket(family, cppsocket::udp)
{
    this->createSocket();
    if (!initialize(family, port, addr))
        exit(0);
}

cppsocket::UDPSocket::~UDPSocket()
{
    this->close();
}

bool cppsocket::UDPSocket::initialize(int port, std::string addr)
{
    return initialize(cppsocket::IPv4, port, addr);
}

bool cppsocket::UDPSocket::initialize(ProtocolFamily family, int port, std::string addr)
{
    if (!bind(port,
              this->getProtocolFamily(),
              addr))
    {
        std::cout << "Bind Failed" << std::endl;
        return false;
    }

    return true;
}
