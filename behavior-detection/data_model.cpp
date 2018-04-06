/*
 * data_model.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: lkang
 */

#include "data_model.h"


RawFrame::RawFrame() {

}
RawFrame::~RawFrame() {

}

string RawFrame::toJson() {
  Json::Value jsonData;
  jsonData["captureTime"] = (Json::Value::UInt64)this->captureTime;
  jsonData["dataSize"] = (Json::Value::UInt)this->dataSize;
  Json::FastWriter fastWriter;
  std::string output = fastWriter.write(jsonData);
  return output;
}

void RawFrame::fromJson(const string& json) {
  Json::Value parsedFromString;
  Json::Reader reader;
  assert(reader.parse(json, parsedFromString));

   //parse json data and read more data
  this->captureTime = parsedFromString["captureTime"].asUInt64();
  this->dataSize = parsedFromString["dataSize"].asUInt();
 }

FrameData::FrameData() {

}
FrameData::~FrameData() {

}
int counter = 0;//wei
long length = 0;//wei

void FrameData::extractFromFramePacket(const FramePacket& framePacket, int numReceived) {
  this->frameSendTime = framePacket.packetSendTime;
  this->transmitSequence = framePacket.frameSequence;
  this->K = framePacket.k;
  this->N = framePacket.n;
  //zw
   this->eventStart = framePacket.parketeventStart;
   this->eventEnd = framePacket.parketeventEnd;
   this->eventType = framePacket.parketeventType;
   cout << "event start: " << framePacket.parketeventStart <<endl;
   cout << "event type: " << framePacket.parketeventType <<endl;

   ///////
  if (numReceived < this->K) {
    this->lossRate = 1.0 - double(numReceived)/double(this->N);
  } else {
    this->lossRate = 1.0 - double(this->K) / double(framePacket.index + 1);
  }
  ////////////wei

  if (K != N){
	  counter++;
	  cout<<"value k!= n: " << counter <<endl;
  }
  ////////////wei
}


string FrameData::toJson() {
  Json::Value jsonData;
  jsonData["frameSendTime"] = (Json::Value::UInt64)this->frameSendTime;
  jsonData["serverTime"] = (Json::Value::UInt64)currentTimeMillis();
  jsonData["transmitSequence"] = Json::Value(this->transmitSequence);
  jsonData["isIFrame"] = Json::Value(this->isIFrame);
  jsonData["originalDataSize"] = Json::Value(this->originalDataSize);
  jsonData["compressedDataSize"] = Json::Value(this->compressedDataSize);
  jsonData["N"] = Json::Value(this->N);
  jsonData["K"] = Json::Value(this->K);
  jsonData["lossRate"] = Json::Value(this->lossRate);
  jsonData["bandwidth"] = Json::Value(this->bandwidth);
  jsonData["type"] = Json::Value(utility::FrameDataFromServer);

  //zw
  jsonData["eventStart"] = (Json::Value::UInt64)this->eventStart;
  jsonData["eventEnd"] = (Json::Value::UInt64)this->eventEnd;
  jsonData["eventType"] = Json::Value(this->eventType);

  // cout<<jsonData.toStyledString()<<endl;
  // write JSON object to a string
  Json::FastWriter fastWriter;
  std::string output = fastWriter.write(jsonData);
  return output;
}

void FrameData::fromJson(const std::string& json) {
  Json::Value parsedFromString;
  Json::Reader reader;
  assert(reader.parse(json, parsedFromString));

  //parse json data and read more data
  this->frameSendTime = parsedFromString["frameSendTime"].asUInt64();
  this->transmitSequence = parsedFromString["transmitSequence"].asUInt();
  this->isIFrame = parsedFromString["isIFrame"].asBool();
  this->originalDataSize = parsedFromString["originalDataSize"].asUInt();
  this->compressedDataSize = parsedFromString["compressedDataSize"].asUInt();
  this->N = parsedFromString["N"].asUInt();
  this->K = parsedFromString["K"].asUInt();
  this->lossRate = parsedFromString["lossRate"].asDouble();
  this->bandwidth = parsedFromString["bandwidth"].asDouble();
  //zw
  this->eventStart = parsedFromString["eventStart"].asUInt64();
  this->eventEnd = parsedFromString["eventEnd"].asUInt64();
  this->eventType = parsedFromString["eventType"].asString();

}


ControlCommand::ControlCommand() {

}

ControlCommand::~ControlCommand() {

}

string ControlCommand::toJson() {
  Json::Value jsonData;
  jsonData["timeStamp"] = (Json::Value::UInt64)this->timeStamp;
  jsonData["steering"] = Json::Value(this->steering);
  jsonData["throttle"] = Json::Value(this->throttle);
  jsonData["type"] = utility::ControlMessageFromServer;
  // cout<<jsonData.toStyledString()<<endl;
  Json::FastWriter fastWriter;
  std::string output = fastWriter.write(jsonData);
  return output;
}

void ControlCommand::fromJson(const string& json) {
  Json::Value parsedFromString;
  Json::Reader reader;
  assert(reader.parse(json, parsedFromString));
  //parse json data and read more data
  this->timeStamp = parsedFromString["timeStamp"].asUInt64();
  this->steering = parsedFromString["steering"].asDouble();
  this->throttle = parsedFromString["throttle"].asDouble();

}


FramePacket::FramePacket() {

}

FramePacket::~FramePacket() {

}

string FramePacket::toJson() {
  uint32_t frameSequence{0}; // uniquely identify a frame
  short k;
  short n;
  uint64_t packetIndex{0}; // 0 ... n - 1


  Json::Value jsonData;
  jsonData["packetSendTime"] = (Json::Value::UInt64)this->packetSendTime;
  jsonData["frameSequence"] = Json::Value(this->frameSequence);
  jsonData["index"] = (Json::Value)this->index;
  jsonData["k"] = Json::Value(this->k);
  jsonData["n"] = Json::Value(this->n);
  jsonData["packetLength"] = Json::Value(this->packetLength);

  Json::FastWriter fastWriter;
  std::string output = fastWriter.write(jsonData);
  return output;

}

void FramePacket::fromJson(const string& json) {
  Json::Value parsedFromString;
  Json::Reader reader;
  assert(reader.parse(json, parsedFromString));
   //parse json data and read more data
  this->packetSendTime = parsedFromString["packetSendTime"].asUInt64();
  this->frameSequence = parsedFromString["frameSequence"].asUInt();
  this->index = parsedFromString["index"].asUInt();
  this->k = parsedFromString["k"].asInt();
  this->n = parsedFromString["n"].asInt();
  this->packetLength = parsedFromString["packetLength"].asInt();

  //zw
  this->parketeventStart = parsedFromString["parketeventStart"].asUInt64();
  this->parketeventEnd = parsedFromString["parketeventEnd"].asUInt64();
  this->parketeventType = parsedFromString["parketeventType"].asString();

}



