/*
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
*/

#include "headers.h"

#include "remote_controller.h"
#include "udp_socket.h"
#include "utility.h"
#include "packet_aggregator.h"


void startThreads(int argc, char** argv);

void testPacketAggregator();
void creatDir(string file_path);
string GetCurrentWorkingDir();
int main( int argc, char** argv )
{
  startThreads(argc, argv);


	/*Start a gstreamer in command line first
	 * gst-launch-1.0 udpsrc port=6666 !  video/x-h264,width=640,height=480,framerate=10/1,aligment=au,
	 * stream-format=avc ! avdec_h264 ! avimux ! filesink location=video.h264*/
  //string path = string("/home/wei/Documents/videos/") + string("LTE_1280_client.raw");
  //utility::convertFileToVideo(path, 0.0);

  return 0;
}


void startThreads(int argc, char** argv) {
  /*if there is an error about bind address or address already used
   *reconnect the tethering mode on the phone(turn off then turn on)
  */

  RemoteController* dataPool = new RemoteController(argc, argv);

  int num = 3;
  pthread_t threads[num];

  pthread_create (&(threads[0]), NULL, &RemoteController::GstreamerReceiver, dataPool);
  pthread_create (&(threads[1]), NULL, &RemoteController::ControlPanel, dataPool);
  pthread_create (&(threads[2]), NULL, &RemoteController::VideoFrameProcesser, dataPool);
  if (dataPool->use_tcp_) {
    pthread_create (&(threads[3]), NULL, &RemoteController::TCPReceiverForCar, dataPool);
  } else {
    pthread_create (&(threads[3]), NULL, &RemoteController::UDPReceiverForCar, dataPool);
  }
  for(int i = 0; i < num; ++i) {
    pthread_join(threads[i], NULL);
  }

  delete dataPool;
}





//////////////////////////backup////////////////////////////


//get the local file path
std::string GetCurrentWorkingDir( void ) {
  char buff[FILENAME_MAX];
  GetCurrentDir( buff, FILENAME_MAX );
  std::string current_working_dir(buff);
  return current_working_dir;
}

//creat a new directory
void creatDir(string file_path){
	std::string dir = "mkdir -p " + GetCurrentWorkingDir() + file_path;
	const int dir_err = system(dir.c_str());
	if (-1 == dir_err)
	{
	    printf("Error creating directory!n");
	    exit(1);
	}
}


void testPacketAggregator() {
  PacketAggregator packetAggregator;
  int frameLen = 38000;
  string payload(frameLen, 'x');

  FrameData frameData;
  frameData.compressedDataSize = frameLen;
  frameData.frameSendTime = 123;
  frameData.transmitSequence = 1;
  frameData.compressedDataSize = payload.size();


  vector<PacketAndData> packets = packetAggregator.deaggregatePackets(frameData, payload, 0.04);

  for (int i = 0; i < packets.size(); ++i) {
    PacketAndData cur = packets[i];
    packetAggregator.insertPacket(cur.first, cur.second);
  }
  for (int i = 0; i < packetAggregator.videoFrames.size(); ++i) {
    FrameAndData frameAndData = packetAggregator.videoFrames[i];
  }
}








