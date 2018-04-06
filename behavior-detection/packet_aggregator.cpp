

#include "packet_aggregator.h"

PacketAggregator::PacketAggregator() {
  FEClib::fec_init();
  sequenceCounter.first = -1;
  sequenceCounter.second = 0;
}

PacketAggregator::~PacketAggregator() {

}

string PacketAggregator::generateFrame(FramePacket header, unsigned char **data) {
  int len = header.packetLength;
  int k = header.k;
  string payload = "";
  for (int i = 0; i < k; ++i) {
    payload += string(reinterpret_cast<char*>(data[i]), len);
  }
  return payload;
}


void PacketAggregator::aggregatePackets(set<PacketAndData, classComp>& videoPackets, int sequence) {
  int sz = videoPackets.size();
  assert(sz > 0);
  // it must be the last received to calculate loss rate
  FramePacket samplePkt = (*videoPackets.rbegin()).first;
  cout<<samplePkt.packetSendTime<<samplePkt.parketeventType<<endl;
  FrameData frameData;
  frameData.extractFromFramePacket(samplePkt, videoPackets.size());
  frameData.bandwidth = this->bandwidth;

  int k = samplePkt.k;
  unsigned int len = samplePkt.packetLength;

  int dataCnt = 0;
  int fecCnt = 0;
  // survey
  for (PacketAndData curPkt: videoPackets) {
    FramePacket packet = curPkt.first;
    assert (sequence == packet.frameSequence);
    int index = packet.index;
    if (index < k) {
      dataCnt ++;
    } else {
      fecCnt ++;
    }
  }

  if (fecCnt == 0 || fecCnt + dataCnt < k) {
    // process data blocks since all orignal packets are received or
    string payload = "";
    for (PacketAndData curPkt: videoPackets) {
      FramePacket packet = curPkt.first;
      int index = packet.index;
      if (index < k) {
        payload += curPkt.second;
      }
    }
    this->videoFrames.push_back(make_pair(frameData, payload));
    //zzw
/*    cout<<"event_start_time: "<<frameData.eventStart<<endl;
    cout<<"event_end_time: "<<frameData.eventEnd<<endl;
    cout<<"event_type: "<<frameData.eventType<<endl;
    cout<<"frameSendTime: "<<frameData.frameSendTime<<endl;
    cout<<"transmitSequence: "<<frameData.transmitSequence<<endl;*/


    //
    return;
  }

  // recover needed
  unsigned char *data_blocks[k];
  for (int i = 0; i < k; ++i) {
    data_blocks[i] = new unsigned char[len];
  }

  set<int> dataIndexes;

  unsigned int * fec_block_nos = new unsigned int[fecCnt];
  unsigned int * erased_blocks = new unsigned int[k - dataCnt];

  unsigned char *fec_blocks[fecCnt];
  for (int i = 0; i < fecCnt; ++i) {
    fec_blocks[i] = new unsigned char[len];
  }
  int j = 0;
  // program ends here
  for (PacketAndData curPkt: videoPackets) {
    FramePacket packet = curPkt.first;
    int index = packet.index;
    if (index < k) {
      memcpy((void*)data_blocks[index], (void*)curPkt.second.c_str(), len);
      dataIndexes.insert(index);
    } else {
      fec_block_nos[j] = index - k;
      memcpy((void*)fec_blocks[j], (void*)curPkt.second.c_str(), len);
      ++ j;
    }
  }

  for (int i = 0, j = 0; i < k; ++i) {
    if (dataIndexes.count(i) == 0) {
      erased_blocks[j++] = i;
    }
  }
  FEClib::fec_decode(len, data_blocks, k, fec_blocks, fec_block_nos, erased_blocks, fecCnt);

  string payload = generateFrame(samplePkt, data_blocks);
  this->videoFrames.push_back(make_pair(frameData, payload));

  for (int i = 0; i < k; ++i) {
    delete [] data_blocks[i];
  }
  for (int i = 0; i < fecCnt; ++i) {
    delete [] fec_blocks[i];
  }
  delete [] fec_block_nos;
  delete [] erased_blocks;
}

void PacketAggregator::trackBandwidth(const FramePacket& header, const string& data) {
  // record bandwidth
  uint64_t now = currentTimeMillis();
  this->dataReceived += data.size();
  if (this->lastRecord == 0) {
    this->lastRecord = now;
  }
  if (now - this->lastRecord >= this->duration) {
    this->bandwidth = double(this->dataReceived * 8.0) / double(1e3 * (now - this->lastRecord));
    this->lastRecord = now;
    this->dataReceived = 0;
  }
}

void PacketAggregator::insertPacket(FramePacket& header, string& data) {

  trackBandwidth(header, data);

  int sequence = header.frameSequence;
  int k = header.k;
  //zw
  //cout<<header.toJson()<<" "<<endl;
  if (sequenceCounter.first > sequence) {
    return;
  }
  if (sequenceCounter.first == sequence) {
    this->videoPackets.insert(make_pair(header, data));
    sequenceCounter.second ++;

  	////
    //cout<<"value sequenceCounter.second: " <<sequenceCounter.second<<endl;
    //////
   // cout<<"value n: " <<header.n<<endl;
    //cout<<"package size: " <<data.length()<<endl;
    //////
    ////
    if (sequenceCounter.second == k) {
      // aggregate current one
      aggregatePackets(this->videoPackets, sequence);


      this->videoPackets.clear();
      sequenceCounter.first ++;
      sequenceCounter.second = 0;
    }
  } else {
    if (sequenceCounter.second != 0 && !this->videoPackets.empty()) {
      // aggregate last one
      aggregatePackets(this->videoPackets, this->sequenceCounter.first);
    }
    this->videoPackets.clear();
    this->videoPackets.insert(make_pair(header, data));
    sequenceCounter.first = sequence;
    sequenceCounter.second = 1;
  }
  //cout<<"value sequenceCounter.first: " <<sequenceCounter.first<<endl;



}

vector<PacketAndData> PacketAggregator::deaggregatePackets(FrameData& frameData, string& payload, double loss) {
  int sz = frameData.compressedDataSize;
  uint64_t sendTime = frameData.frameSendTime;
  const int referencePktSize = 4000;
  // minimize padding
  int k = sz / referencePktSize + (sz % referencePktSize == 0 ? 0 : 1);
  int blockSize = sz / k + (sz % k == 0 ? 0 : 1);
  payload += string(k * blockSize - sz, '0');

  int n = round(k * double(1.0 + loss * 5.0));
  vector<PacketAndData> res;
  if (payload.size() == 0) {
    return res;
  }

  for (int i = 0; i < k; ++i) {
    FramePacket framePacket(sendTime, frameData.transmitSequence, blockSize, k, n, i);
    string data = payload.substr(i * blockSize, blockSize);
    res.push_back(make_pair(framePacket, data));
  }
  if (k == n) {
    return res;
  }

  unsigned char *data_blocks[k];
  for (int i = 0; i < k; ++i) {
    data_blocks[i] = new unsigned char[blockSize];
    memcpy(data_blocks[i], res[i].second.c_str(), blockSize);
  }

  int fecCnt = n - k;
  unsigned char *fec_blocks[fecCnt];
  for (int i = 0; i < fecCnt; ++i) {
    fec_blocks[i] = new unsigned char[blockSize];
  }
  FEClib::fec_encode(blockSize, data_blocks, k, fec_blocks, n - k);
  for (int i = 0; i < fecCnt; ++i) {
    FramePacket framePacket(sendTime, frameData.transmitSequence, blockSize, k, n, k + i);
    string data = string(reinterpret_cast<char*>(fec_blocks[i]), blockSize);
    res.push_back(make_pair(framePacket, data));
  }
  for (int i = 0; i < k; ++i) {
    delete [] data_blocks[i];
  }
  for (int i = 0; i < fecCnt; ++i) {
    delete [] fec_blocks[i];
  }
  return res;
}


