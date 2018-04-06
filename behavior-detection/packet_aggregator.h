/*
 * packet_aggregator.h
 *
 *  Created on: Nov 13, 2017
 *      Author: lkang
 */

#ifndef PACKET_AGGREGATOR_H_
#define PACKET_AGGREGATOR_H_

#include "data_model.h"
#include "fec.h"
#include <set>


/**
 * Assume frame comes in order, i.e., when the first packet of frame x + 1 comes,
 * we aggregate the packets for frame x, no matter there is enough packets
 * If we get enough packets for frame x, we will start aggregate, and ignore future packets
 * of frame x
 */

using PacketAndData = pair<FramePacket, string>;
using FrameAndData = pair<FrameData, string>;
struct classComp {
  bool operator() (const PacketAndData& a, const PacketAndData& b) const
  { return a.first.index < b.first.index; }
};


class PacketAggregator {
private:
  pair<int, int> sequenceCounter;
  set<PacketAndData, classComp> videoPackets;

  int duration {1000}; // ms
  uint64_t lastRecord {0}; // time stamp
  uint64_t dataReceived {0}; // byte
  double bandwidth {0.0}; //mbps

  //zw
  //uint64_t parketeventStart{0};
  //uint64_t parketeventEnd{0};
  //string parketeventType{""};

  //
public:
  deque<pair<FrameData, string>> videoFrames;

  PacketAggregator();
  ~PacketAggregator();
  string generateFrame(FramePacket header, unsigned char **data);
  void insertPacket(FramePacket& header, string& data);
  void trackBandwidth(const FramePacket& header, const string& data);
  void aggregatePackets(set<PacketAndData, classComp>& videoPackets, int sequence);

  vector<PacketAndData> deaggregatePackets(FrameData& frameData, string& payload, double loss);
};


#endif /* PACKET_AGGREGATOR_H_ */
