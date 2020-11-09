#include <stdio.h>
#include <stdlib.h>
#include "cppsocket/udp_socket.h"

int main(int argc, char *argv[])
{
    int reuse = 1;
    int brdOpt = 1;
    int ms = 1000;
    cppsocket::udp_socket sock;
    sock.option(cppsocket::SOCK_REUSE, (void *)&reuse);
    sock.option(cppsocket::SOCK_BROADCAST, (void *)&brdOpt);
    sock.option(cppsocket::SOCK_RCVTIMEO_MS, (void *)&ms);
    
    char *buf = "broadcast message";
    char rcvBuf[1024] = {0,};
    int tryCnt = 10;
    while(tryCnt--) {
        cppsocket::peer peer("255.255.255.255", "9009");
        sock.sendto(&peer, buf, strlen(buf));
        std::cout << "message sent: " << buf << std::endl;
        sock.recvfrom(&peer, rcvBuf, strlen(buf));
        std::cout << "message recvived: " << rcvBuf << std::endl;
        __sleep(1);
    }
    
    sock.close();
    return 0;
}
