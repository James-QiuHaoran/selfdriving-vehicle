#ifndef HEADERS_H_
#define HEADERS_H_


//#define _GNU_SOURCE

/*General C Headers*/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>     
#include <assert.h>

#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>


#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <fcntl.h>


/*General C++ Headers*/


#include <iostream>
#include <sstream>
#include <fstream>

#include <vector>
#include <deque>
#include <map>
#include <list>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iomanip>
#include <jsoncpp/json/json.h>
#include <termios.h>

#include <chrono>
#include <thread>
#include <mutex>    


using namespace std;
using namespace std::chrono;


static inline uint64_t currentTimeMillis() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}

struct Point {
    double x;
    double y;
};

class EventObj {

public:

  string event{""};
  bool isTrafficLight{false};
  bool isPedestrian_Cycle{false};
  bool isIntersection{false};
  bool isTurningSignal{false};

  uint64_t event_start{0};
  uint64_t event_end{0};

  int headpose{0}; // 0 - no headpose; 1-4: left, right, top, rear
  int turningSignal{0}; // left = -1, straight = 0, right 1
  double turningRadius{0.0};
  double speed{0.0};
  double accel{false};
  double decel{false};
  double steeringAngle{0.0};

  bool excessiveOfJerk{false};
  bool trafficControlSignal{false};
  bool distraction{false};
  double frontVehicleDistance{0.0};
  bool hasPedestrianCyclist{false};
  
  double rearVehicleDistance{0.0};
  double maneuverDuration{0.0};

  double longtitude{0.0};
  double altitude{0.0};

  int box_num{0};
  int class_name{0};
  string class_id{""};

  uint64_t timestamp{0};
  int confidence{0};
  int upperleft_h{0};
  int upperleft_v{0};
  int bottomright_h{0};
  int bottomright_v{0};

  /*
   * Vehicle Speed
  Accel. / Deceleration
  Steering Angle
  Turning Radius
  Head Pose
  Turning Signal

  Excessive of Jerk
  Front Vehicle Distance
  Traffic Control Signal
  Pedestrian / Cyclist
  Head Pose
  Front Vehicle Distance
  Rear Vehicle Distance
  Turning Signal
  Maneuver Duration
  Head Pose
   * */
};

#endif

