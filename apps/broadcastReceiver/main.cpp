#include <stdio.h>
#include <stdlib.h>
#include "cppsocket/udp_socket.h"

int main(int argc, char *argv[])
{
    int reuse = 1;
    int ms = 1000;
    cppsocket::udp_socket sock((ushort)9009);
    sock.option(cppsocket::SOCK_REUSE, (void *)&reuse);
    sock.option(cppsocket::SOCK_RCVTIMEO_MS, (void *)&ms);
    std::cout << "udp server-socket info, " << sock.sock_ip << ":" << sock.sock_port << std::endl;

    cppsocket::peer peer;
    const int BUF_SIZE = 1024;
    while(true) {
        char buf[BUF_SIZE] = {0,};
        auto readBytes = sock.recvfrom(&peer, buf, BUF_SIZE);
        if(readBytes > 0) {
            std::cout << "message received: " << buf << std::endl;
            sock.sendto(&peer, buf, strlen(buf));
            std::cout << "message sent: " << buf << std::endl;
        }
        else {
            std::cout << "timeout..." << std::endl;
        }
        __sleep(1);
    }
    
    sock.close();
    return 0;
}
