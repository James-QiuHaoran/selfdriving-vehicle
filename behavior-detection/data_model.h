/*
 * data_model.h
 *
 *  Created on: Oct 24, 2017
 *      Author: lkang
 */

#ifndef DATA_MODEL_H_
#define DATA_MODEL_H_


#include "utility.h"

/**
 * used for offline processing
 */
struct RawFrame {
  uint64_t captureTime {0};
  uint32_t dataSize {0};
  static const int requiredSpace = 500;
  RawFrame();
  ~RawFrame();
  string toJson();
  void fromJson(const std::string& json);
};

struct FramePacket {
  uint64_t packetSendTime{0};
  uint32_t frameSequence{0}; // uniquely identify a frame
  int packetLength{0};
  int k{0};
  int n{0};
  int index{0}; // 0 ... n - 1
  static const int requiredSpace = 500;
  //zw
  uint64_t parketeventStart{0};
  uint64_t parketeventEnd{0};
  string parketeventType{""};

  FramePacket();
  FramePacket(uint64_t sendTime, uint32_t frameSequence, int len, int k, int n, int index):
    packetSendTime(sendTime), frameSequence(frameSequence), packetLength(len), k(k), n(n), index(index) {}
  ~FramePacket();
  string toJson();
  void fromJson(const std::string& json);
};

struct FrameData {
  uint64_t frameSendTime{0};
  uint32_t transmitSequence{0};
  bool isIFrame{false};
  uint32_t originalDataSize{0};
  uint32_t compressedDataSize{0};
  uint64_t serverTime{0};
  int N {0};
  int K {0};
  double lossRate{0.0};
  double bandwidth{0.0};

  //zw
  uint64_t eventStart{0};
  uint64_t eventEnd{0};
  string eventType{""};

  FrameData();
  ~FrameData();
  void extractFromFramePacket(const FramePacket& framePacket, int numReceived);
  string toJson();
  void fromJson(const std::string& json);
};

struct ControlCommand {
  uint64_t timeStamp{0};
  double steering{0.5};
	double throttle{0.5};

	ControlCommand();
	~ControlCommand();
	string toJson();
	void fromJson(const std::string& json);
};


#endif /* DATA_MODEL_H_ */
