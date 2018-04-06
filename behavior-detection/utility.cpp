/*
 * utility.cpp
 *
 *  Created on: Sep 6, 2017
 *      Author: lkang
 */
#include "utility.h"
#include "packet_aggregator.h"

namespace utility {


// It requires another thread or terminal to start gstreamer
void convertFileToVideoFEC(string raw, double loss_percent) {
  std::ifstream ifs (raw.c_str(), std::ifstream::in);
  PacketAggregator packetAggregator;

  UdpSocket* udpsocket = new UdpSocket(kPacketSize);
  //udpsocket->UdpSocketSetUp("127.0.0.1", 4444);
  udpsocket->UdpSocketSetUp("192.168.0.106", 4444);
  long packetSize = 1500;
  char* header = new char [RawFrame::requiredSpace];
  char* buffer = new char [1000000];
  char* assemble = new char [1000000];
  // the end char is 255
  int sum = 0, i = 0;
  while (ifs.good() && ifs.peek() != char(255)) {
    ifs.read(header, RawFrame::requiredSpace);
    RawFrame rawFrame;
    rawFrame.fromJson(string(header));
    int sz = rawFrame.dataSize;
    if (sz == 0) {
      continue;
    }
    ifs.read(buffer, sz);

    FrameData frameData;
    frameData.compressedDataSize = sz;
    frameData.transmitSequence = i ++;
    string payload(buffer, sz);
    vector<PacketAndData> packets = packetAggregator.deaggregatePackets(frameData, payload, loss_percent);
    for (int i = 0; i < packets.size(); ++i) {
      if (getRandomNumber() < loss_percent * 100.0) {
        continue;
      }
      PacketAndData cur = packets[i];
      packetAggregator.insertPacket(cur.first, cur.second);
    }
    while (!packetAggregator.videoFrames.empty()) {
      FrameAndData frameAndData = packetAggregator.videoFrames.front();
      packetAggregator.videoFrames.pop_front();
      string data = frameAndData.second;
      //udpsocket->SendTo("127.0.0.1", 12345, data);
      udpsocket->SendTo("192.168.0.171", 6666, data);
      usleep(10*1000);
    }
  }
  ifs.close();
  delete header;
  delete buffer;
  delete assemble;
}

// It requires another thread or terminal to start gstreamer
void convertFileToVideo(string raw, double loss_percent) {
  std::ifstream ifs (raw.c_str(), std::ifstream::in);

  UdpSocket* udpsocket = new UdpSocket(kPacketSize);
  udpsocket->UdpSocketSetUp("127.0.0.1", 4444);
  long packetSize = 1500;
  char* header = new char [RawFrame::requiredSpace];
  char* buffer = new char [1000000];
  char* assemble = new char [1000000];
  // the end char is 255
  while (ifs.good() && ifs.peek() != char(255)) {
    ifs.read(header, RawFrame::requiredSpace);
    RawFrame rawFrame;
    rawFrame.fromJson(string(header));
    int sz = rawFrame.dataSize;
    if (sz == 0) {
      continue;
    }
    ifs.read(buffer, sz);
    int numPackets = (sz / packetSize) + (sz % packetSize == 0 ? 0 : 1);
    int lostSize = 0;
    for (int i = 0, newIndex = 0; i < numPackets; ++i) {
      int curLen = std::min(sz - packetSize * i, packetSize);
      if (getRandomNumber() <= loss_percent * 100.0) {
        lostSize += curLen;
      } else {
        memcpy(assemble + newIndex * packetSize, buffer + i * packetSize, curLen);
        newIndex++;
      }
    }
    udpsocket->SendByteTo("127.0.0.1", 6666, assemble, sz - lostSize);
    usleep(10*1000);
  }
  ifs.close();
  delete header;
  delete buffer;
  delete assemble;
}




int getRandomNumber() {
  return uint_dist100(randomNumberGenerator);
}


/*
double blurDetection(cv::Mat& src) {
  cv::Mat gray;
  cvtColor(src, gray, CV_BGR2GRAY);
  cv::Mat lap;
  cv::Laplacian(gray, lap, CV_64F);
  cv::Scalar mu, sigma;
  cv::meanStdDev(lap, mu, sigma);
  double focusMeasure = sigma.val[0]*sigma.val[0];
  return focusMeasure;
}

double blurDetection_test(Mat& src) {
  cout<<"start"<<endl;
  assert (!src.empty());
  Mat gray;
  cvtColor(src, gray, CV_BGR2GRAY);
  Mat lap;
  Laplacian(gray, lap, CV_64F);
  Scalar mu, sigma;
  meanStdDev(lap, mu, sigma);
  double focusMeasure = sigma.val[0]*sigma.val[0];
  cout<<"focusMeasure"<<focusMeasure<<endl;

  return focusMeasure;
}

int adjustTest(Mat& src)
{
  assert (!src.empty());
  Mat src_gray;
  int thresh = 100;
  cvtColor( src, src_gray, COLOR_BGR2GRAY );
  const char* source_window = "Source";
  namedWindow( source_window, WINDOW_AUTOSIZE );
  imshow( source_window, src );
  createTrackbar("Canny thresh:", "Source", &thresh, 255, thresh_callback, &src_gray);
  thresh_callback(thresh, &src_gray);
  waitKey(0);
  return(0);
}
void thresh_callback(int thresh, void* gray)
{
  Mat* src_gray = (Mat*) gray;
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  Canny(*src_gray, canny_output, thresh, thresh*2);
  findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ ) {
    drawContours( drawing, contours, (int)i, Scalar(0,0,255), FILLED);
  }
  namedWindow( "Contours", WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
}


// http://docs.opencv.org/2.4/doc/tutorials/highgui/video-input-psnr-ssim/video-input-psnr-ssim.html#image-similarity-psnr-and-ssim

double getPSNR(const Mat& I1, const Mat& I2)
{
 Mat s1;
 absdiff(I1, I2, s1);       // |I1 - I2|
 s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
 s1 = s1.mul(s1);           // |I1 - I2|^2

 Scalar s = sum(s1);         // sum elements per channel

 double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

 if( sse <= 1e-10) // for small values return zero
     return 0;
 else
 {
     double  mse =sse /(double)(I1.channels() * I1.total());
     double psnr = 10.0*log10((255*255)/mse);
     return psnr;
 }
}

Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
 const double C1 = 6.5025, C2 = 58.5225;
 **************************** INITS *********************************
 int d     = CV_32F;

 Mat I1, I2;
 i1.convertTo(I1, d);           // cannot calculate on one byte large values
 i2.convertTo(I2, d);

 Mat I2_2   = I2.mul(I2);        // I2^2
 Mat I1_2   = I1.mul(I1);        // I1^2
 Mat I1_I2  = I1.mul(I2);        // I1 * I2

 **********************PRELIMINARY COMPUTING *****************************

 Mat mu1, mu2;   //
 GaussianBlur(I1, mu1, Size(11, 11), 1.5);
 GaussianBlur(I2, mu2, Size(11, 11), 1.5);

 Mat mu1_2   =   mu1.mul(mu1);
 Mat mu2_2   =   mu2.mul(mu2);
 Mat mu1_mu2 =   mu1.mul(mu2);

 Mat sigma1_2, sigma2_2, sigma12;

 GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
 sigma1_2 -= mu1_2;

 GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
 sigma2_2 -= mu2_2;

 GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
 sigma12 -= mu1_mu2;

 ///////////////////////////////// FORMULA ////////////////////////////////
 Mat t1, t2, t3;

 t1 = 2 * mu1_mu2 + C1;
 t2 = 2 * sigma12 + C2;
 t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

 t1 = mu1_2 + mu2_2 + C1;
 t2 = sigma1_2 + sigma2_2 + C2;
 t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

 Mat ssim_map;
 divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

 Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
 return mssim;
}
*/


}



