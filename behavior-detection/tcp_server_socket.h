#ifndef TCP_SERVER_SOCKET_
#define TCP_SERVER_SOCKET_

#include "headers.h"

class TcpServerSocket {
private:
  int32_t tcp_socket_id_;
  struct sockaddr_in tcp_server_addr_;

public:
  TcpServerSocket(int32_t port = 55555);
  ~TcpServerSocket();
  void TcpServerSetUp(int32_t num_client = 1);


  int32_t Accept();

  /**
   * read some data from the TCP server socket
   */
  int32_t TcpServerRead(const int32_t id, char* data, int32_t len);
  /**
   * read exactly len data from TCP server socket
   */
  int32_t TcpServerReadN(const int32_t id, char* data, int32_t len);
 
  int32_t TcpServerWrite(const int32_t id, const char* data, int32_t len);
};


#endif
