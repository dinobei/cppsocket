# cppsocket

cross-platform socket library in c++11, which facilitates using sockets in c++ as it provides a simple interface for the socket.

`cppsocket` can use on various platforms with the same interface

## Feature

- use cmake to build
- no third-party library dependency
- support tcp/udp communications
- support multicast feature
- target OS: Windows, macOS, Ubuntu, iOS, Android
- support IPv4, IPv6 both

## Quickstart

**How to build**

```
# git clone https://github.com/dinobei/cppsocket
# cd cppsocket
# mkdir build
# cd build
# cmake ..
# make
```

**Code preview**

```c++
// Echo Server
cppsocket::tcp_socket sock((ushort)9000);
auto client = sock.accept();
char buf[255] = {0,};
client->safe_recv(buf, 10);
client->safe_send(buf, sizeof(buf));

// Echo Client
cppsocket::tcp_socket sock;
sock.connect("127.0.0.1", "9000");
char send_buf[11] = "1234567890";
char recv_buf[11] = {0,};
sock.safe_send(send_buf, 10);
sock.safe_recv(recv_buf, 10);
```

## LICENSE

MIT
