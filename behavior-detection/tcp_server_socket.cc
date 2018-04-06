#include "tcp_server_socket.h"

/*================================================TCP Server Socket==================================================*/

TcpServerSocket::TcpServerSocket(int32_t port/*=kTcpServerPort*/)
{
	//Prepare the sockaddr_in structure
	tcp_server_addr_.sin_family = AF_INET;
	tcp_server_addr_.sin_addr.s_addr = INADDR_ANY;
	tcp_server_addr_.sin_port = htons(port);
	tcp_socket_id_ = socket(AF_INET , SOCK_STREAM , 0);
	if (tcp_socket_id_ == -1) {
		printf("Could not create socket");
	}
	
}

TcpServerSocket::~TcpServerSocket()
{
  close(tcp_socket_id_);
}

int32_t TcpServerSocket::TcpServerRead(int32_t id, char* data, int32_t len)
{
  int32_t nread = read(id, data, len);
  if (nread < 0) {
    printf("%d\n%d:%s\n", nread, len, data);
    printf("TcpServerRead failed\n");
    assert(0);
  }
  return nread;
}

int32_t TcpServerSocket::TcpServerReadN(int32_t id, char* data, int32_t len)
{
  int32_t rest = len, nread = -1;
	while(rest) {
    if ((nread = read(id, data, rest)) == 0) {
      return 0; /*on success*/
    } else if (nread < 0) {
      exit(1); /*on failed*/
    } else { /*on partial read*/
      rest -= nread;
      data += nread;
    }
	}
  return len;
}


int32_t TcpServerSocket::TcpServerWrite(int32_t id, const char* data, int32_t len)
{
  int32_t nwrite = write(id, data, len);
	assert(nwrite >= 0);
  return nwrite;
}


void TcpServerSocket::TcpServerSetUp(int num_client)
{


  int optval = 1;
	/* avoid EADDRINUSE error on bind() */
  if(setsockopt(tcp_socket_id_, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval)) < 0) {
    perror("setsockopt()");
    exit(1);
  }
	//Bind
	if( bind(tcp_socket_id_,(struct sockaddr *)&tcp_server_addr_ , sizeof(tcp_server_addr_)) < 0) {
		cout<<"socket:"<<tcp_socket_id_<<" Bind to port:"<<ntohs(tcp_server_addr_.sin_port)<<" failed"<<endl;
	}
	listen(tcp_socket_id_ , num_client);
	cout<<"TcpServerSetUp: tcp socket "<<tcp_socket_id_<<" bind and listen to port:"<<ntohs(tcp_server_addr_.sin_port)<<endl;	
	
}

int32_t TcpServerSocket::Accept()
{
  int32_t c = sizeof(struct sockaddr_in);
  struct sockaddr_in client_addr;
  int32_t new_socket = accept(tcp_socket_id_, (struct sockaddr *)&client_addr, (socklen_t*)&c);
	if (new_socket<0) {
		perror("accept failed");
  }
  string ip = string(inet_ntoa(client_addr.sin_addr));
  int port = ntohs(client_addr.sin_port);
  cout<<"Accept connection from: "<<ip<<" port:"<<port<<" "<<endl;
  return new_socket;
}


