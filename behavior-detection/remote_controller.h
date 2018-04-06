#ifndef REMOTE_CONTROLLER_H_
#define REMOTE_CONTROLLER_H_

#include "headers.h"
#include "udp_socket.h"
#include "tcp_server_socket.h"
#include "utility.h"
#include "data_model.h"
#include "packet_aggregator.h"

//this IP address changes when you change your lan cable.
const string kLocalIPForCar = "192.168.0.1";
const int kLocalPortForCar = 55555;

//this IP address is automotive shown when tethering on. When turning tethering on, this IP always changes.
const string kLocalIPForController = "192.168.42.118";
const int kLocalPortForController = 5000;

const double alpha = 0.3;
const double beta = 0.3;

class RemoteController {
private:
  //meta information, always there
  //mac address of node, and the node type
  bool use_gst_ {true};
  bool display_video_ {true};
  bool store_video_ {false};


  // frame data for gstreamer
  int gst_width_ {1024};
  int gst_height_ {768};


  int gst_frame_rate_ {15};
  const int gst_port_ {6666};
  string gst_h264_video_file_;
  string gst_h264_raw_data_;
  ofstream ofs_;

  bool running;
  int argc;
  char** argv;

  //buffer information, updated every cycle
  mutex mtx; 

  UdpSocket* udpsocketController_;
  UdpSocket* udpsocketCar_;

  TcpServerSocket* tcpServer_;
  // Car's IP address via the WIFI
  string remoteIPCar = "";
  int remotePortCar {5555};
  // track the latency difference
  bool consistentView {false};
  int frameCount {0};
  double latencyDifference {0.0};
  double latencyDeviation {0.0};

  PacketAggregator packetAggregator;

  int tcpClientSocket {0};
public:

  bool use_tcp_ {true};
  void trackLatencyDifference(long frameSendTime);
  void displayAndStoreVideo(FrameData& header, string& data);

  int getJsonHeaderIndex(string& data);

  RemoteController(int argc, char** argv) {
    // start two socket with different IP
    udpsocketController_ = new UdpSocket(kPacketSize);
    udpsocketController_->UdpSocketSetUp(kLocalIPForController, kLocalPortForController);

    udpsocketCar_ = new UdpSocket(kPacketSize);
    udpsocketCar_->UdpSocketSetUp(kLocalIPForCar, kLocalPortForCar);

    tcpServer_ = new TcpServerSocket();
    tcpServer_->TcpServerSetUp();

    running = true;
    cout<<"DataPool is runing"<<endl;

    long time = currentTimeMillis();
    gst_h264_raw_data_ = to_string(time) + ".raw";
    gst_h264_video_file_ = to_string(time) + ".h264";
    ofs_.open(gst_h264_raw_data_, ifstream::app);

    this->argc = argc;
    this->argv = argv;
  }
  ~RemoteController() {
    ofs_.close();
    delete udpsocketController_;
    delete udpsocketCar_;
    delete tcpServer_;
  }

  static void* UDPReceiverForCar(void* dataPool);
  static void* ControlPanel(void* dataPool);
  static void* GstreamerReceiver(void* dataPool);
  static void* VideoFrameProcesser(void* dataPool);
  static void* TCPReceiverForCar(void* dataPool);
};

#endif
