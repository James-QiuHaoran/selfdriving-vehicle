#include "udp_socket.h"

#include "remote_controller.h"


UdpSocket::UdpSocket(int32_t pkt_sz/* = kPacketSize*/)
{
  udp_socket_id_ = socket(AF_INET, SOCK_DGRAM, 0);
  assert (udp_socket_id_ != -1);  
  broadcast_server = 0;
  read_packet_size_ = pkt_sz;
}

UdpSocket::~UdpSocket()
{
  close(udp_socket_id_);
}

int32_t UdpSocket::SetNonblocking()
{
  int flags;
  if (-1 == (flags = fcntl(udp_socket_id_, F_GETFL, 0))) {
    flags = 0;
  }
  return fcntl(udp_socket_id_, F_SETFL, flags | O_NONBLOCK);
}

  
void UdpSocket::UdpSocketSetUp(const string& ip, const int32_t& port)
{

  std::cout<<"starting UDP socket at ip: "<<ip<<" port:"<<port<<std::endl;
  /*UDP server part*/
  udp_port_ = port;

  udp_addr_.sin_family = AF_INET;
  udp_addr_.sin_port = htons(port);
  udp_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  bzero(&(udp_addr_.sin_zero),8);

  int32_t res_bind = bind(udp_socket_id_, (struct sockaddr *)&udp_addr_, sizeof(struct sockaddr));
  if(-1==res_bind) {
    perror("Error: UdpSocket bind failed in UdpSocketSetUp()");
  }

  int32_t multi_use = 1;
  int32_t check_mu = setsockopt(udp_socket_id_,  SOL_SOCKET,  SO_REUSEADDR,  &multi_use,  sizeof(multi_use));  
  if(-1 == check_mu) {
    perror("Error: UdpSocket set multiple usage failed in UdpSocketSetUp()");
  }
}

void UdpSocket::UdpSocketSetUpBroadcast(const string& ip, const int32_t& port)
{
  broadcast_server = 1;
  udp_broadcast_port_ = port;

  udp_broadcast_addr_.sin_family = AF_INET;
  udp_broadcast_addr_.sin_port = htons(port);
  udp_broadcast_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  bzero(&(udp_broadcast_addr_.sin_zero),8);

  int32_t broadcastOn = 1;
  int32_t res = setsockopt(udp_socket_id_, SOL_SOCKET, SO_BROADCAST, &broadcastOn, sizeof(broadcastOn));
  if(res == -1) {
		perror("Error: setsockopt call failed in UdpSocketEnableBroadcast()");
  }
}

int32_t UdpSocket::UdpSocketBroadcast(const string& send_data)
{
  assert(1==broadcast_server);
#ifdef DEBUG_UDP_SOCKET
  cout<<"in UdpSocket::SendTo: "<<" dest:"<<string(inet_ntoa(udp_broadcast_addr_.sin_addr))<<" port:"<<ntohs(udp_broadcast_addr_.sin_port)<<" data size:"<<send_data.size()<<endl;
  size_t sz = send_data.size();
  if(sz <= 20) cout<<"data: "<<send_data<<endl;
  else cout<<"data first 10 bytes:"<<send_data.substr(0, 10)<<" data last 10 bytes"<<send_data.substr(sz - 10, 10)<<endl;
#endif
  return sendto(udp_socket_id_, send_data.c_str(), send_data.size(), 0, (struct sockaddr *)&udp_broadcast_addr_, sizeof(struct sockaddr));  
}

int32_t UdpSocket::SendTo(const string& ip, const int32_t& port, const string& send_data)
{
  udp_client_addr_.sin_family = AF_INET;
  udp_client_addr_.sin_port = htons(port);
  udp_client_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  bzero(&(udp_client_addr_.sin_zero),8);
#ifdef DEBUG_UDP_SOCKET
  cout<<"in UdpSocket::SendTo: "<<" dest:"<<string(inet_ntoa(udp_client_addr_.sin_addr))<<" port:"<<ntohs(udp_client_addr_.sin_port)<<" data size:"<<send_data.size()<<endl;
  size_t sz = send_data.size();
  if(sz <= 20) cout<<"data: "<<send_data<<endl;
  else cout<<"data first 10 bytes:"<<send_data.substr(0, 10)<<" data last 10 bytes"<<send_data.substr(sz - 10, 10)<<endl;
#endif
  return sendto(udp_socket_id_, send_data.c_str(), send_data.size(), 0, (struct sockaddr *)&udp_client_addr_, sizeof(struct sockaddr));  
}

int32_t UdpSocket::SendByteTo(const string& ip, const int32_t port, char *data, int len)
{
  udp_client_addr_.sin_family = AF_INET;
  udp_client_addr_.sin_port = htons(port);
  udp_client_addr_.sin_addr.s_addr = inet_addr(ip.c_str());
  bzero(&(udp_client_addr_.sin_zero),8);
  return sendto(udp_socket_id_, data, len, 0, (struct sockaddr *)&udp_client_addr_, sizeof(struct sockaddr));
}


int32_t UdpSocket::ReceiveFrom(string& ip, int32_t& port, string& data)
{
  int32_t addr_len = sizeof(struct sockaddr);
  char recv_data[kPacketSize+1];
  int32_t bytes_read = recvfrom(udp_socket_id_,recv_data, kPacketSize, 0, (struct sockaddr *)&udp_client_addr_, (socklen_t*)&addr_len);
  recv_data[bytes_read] = '\0';
  ip = string(inet_ntoa(udp_client_addr_.sin_addr));
  port = ntohs(udp_client_addr_.sin_port);
  data = string(recv_data, bytes_read);
#ifdef DEBUG_UDP_SOCKET
  cout<<"in UdpSocket::ReceiveFrom: "<<" source:"<<ip<<" port:"<<port<<" data size:"<<data.size()<<"\t"<<bytes_read<<endl;
  size_t sz = data.size();
  if(sz <= 20) cout<<"data: "<<data<<endl;
  else cout<<"data first 10 bytes:"<<data.substr(0, 10)<<" data last 10 bytes"<<data.substr(sz - 10, 10)<<endl;
#endif
  return bytes_read;  
}


/*
 *
const std::vector<unsigned char> ReceiverSocket::GetPacket() const {
  // Get the data from the next incoming packet.
  sockaddr_in remote_addr;
  socklen_t addrlen = sizeof(remote_addr);
  const int num_bytes = recvfrom(socket_handle_, (void *)buffer_, buffer_size_,
                                 0, (sockaddr *)&remote_addr, &addrlen);
  // Copy the data (if any) into the data vector.
  std::vector<unsigned char> data;
  if (num_bytes > 0) {
    data.insert(data.end(), &buffer_[0], &buffer_[num_bytes]);
  }
  return data;
}
 */



