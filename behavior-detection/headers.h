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


/////wei
#include <curl/curl.h>
#include <curl/easy.h>

#include <cstdlib>
// #define WINDOWS  /* uncomment this line to use it for windows.*/
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#include <ctime>//stopsign

/////////////////////////////
#include <gst/gst.h>

using namespace std;
using namespace std::chrono;


static inline uint64_t currentTimeMillis() {
	return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
}


#endif

