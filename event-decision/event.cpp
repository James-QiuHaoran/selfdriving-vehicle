//============================================================================
// Name        : event.cpp
// Author      : wei
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "headers.h"


using namespace std;
EventObj fromJson(EventObj eventObj, string json);
void start(string json);
string CreateJson();
void CheckTurn(EventObj eventObj);
void CheckBrake(EventObj eventObj);
void CheckLaneChange(EventObj eventObj);

// detect whether there's overlap
bool hasOverlap(vector<Point> object1, vector<Point> object2);

int main() {
	string eventjson = CreateJson();
	cout << "create: " << eventjson << endl;
	start(eventjson);
	return 0;
}


void start(string json){
    EventObj eventObj;
    eventObj = fromJson(eventObj,json);
    cout << "Event: " << eventObj.event << endl;

    int limitedSpeed = 35;
    int limitAngle = 15;
    int limitFrontVehicleDistance = 5;
    int limitRearVehicleDistance = 5;

    if (eventObj.event == "turn") {
        double score = 0;

        double w[] = {0.32, 0.17, 0.08, 0.05, 0.21, 0.17};
        vector<double> coefficients(w, w + sizeof(w)/sizeof(*w));

        int alpha = 0;
        double speedDiff = (eventObj.speed - limitedSpeed)/20;
        if (speedDiff > 1)
            speedDiff = 1;
        else if (speedDiff < -1)
            speedDiff = -1; 
           
        alpha = speedDiff > 0 ? 1 : -1;

        // speed
        score += speedDiff * coefficients[0];
        // accelerate / de-accelerate
        score += eventObj.accel ? coefficients[1] : -coefficients[1];
        // steering
        if (abs(eventObj.steeringAngle) > limitAngle)
            score += alpha*coefficients[2];
        // turning radius
        // score += alpha*coefficients[3];
        // head pose
        if (eventObj.headpose == 0)
            score += alpha*coefficients[4];
        // turning signal
        if (eventObj.turningSignal == 0)
            score += alpha*coefficients[5];
        cout << "Result: ";
        if (score <= -0.17)
            cout << "Conservative" << endl;
        else if (score >= 0.17)
            cout << "Aggressive" << endl;
        else
            cout << "Moderate" << endl;
    } else if (eventObj.event == "hardbrake") {
        double score = 0;

        double w[] = {0.2, 0.3, 0.5};
        vector<double> coefficients(w, w + sizeof(w)/sizeof(*w));

        int alpha = eventObj.trafficControlSignal ? 3.3 : -3.3;
        int f1 = eventObj.trafficControlSignal ? 1 : 0; 
        
        // excessive of jerk
        if (eventObj.excessiveOfJerk)
            score += coefficients[0];
        // traffic control signal
        if (eventObj.trafficControlSignal)
            score += coefficients[1];
        // distraction
        if (eventObj.distraction)
            score += coefficients[2];
        // front vehicle distance
        if (eventObj.frontVehicleDistance < limitFrontVehicleDistance)
            score += alpha * f1 * 1;
        // pedestrian or cyclist
        if (eventObj.hasPedestrianCyclist)
            score += alpha * f1 * 1;

        cout << "Result: ";
        if (score >= 0.2)
            cout << "Harmful" << endl;
        else
            cout << "Reasonable" << endl;
    } else if (eventObj.event == "lanechange"){
        double score = 0;

        double w[] = {0.06, 0.35, 0.25, 0.14, 0.25};
        vector<double> coefficients(w, w + sizeof(w)/sizeof(*w));

        int alpha = 0;
        double speedDiff = (eventObj.speed - limitedSpeed)/20;
        if (speedDiff > 1)
            speedDiff = 1;
        else if (speedDiff < -1)
            speedDiff = -1; 
           
        alpha = speedDiff > 0 ? 1 : -1;

        // front vehicle distance
        if (eventObj.frontVehicleDistance < limitFrontVehicleDistance)
            score += alpha * coefficients[0] * 1;
        // rear vehicle distance
        if (eventObj.rearVehicleDistance < limitRearVehicleDistance)
            score += alpha * coefficients[1] * 1;
        // turning signal
        if (eventObj.turningSignal == 0)
            score += alpha * coefficients[2];
        // maneuver duration
        if (eventObj.maneuverDuration < 2)
            score += alpha * coefficients[3];
        else if (eventObj.maneuverDuration > 5)
            score -= alpha * coefficients[3];
        // head pose
        if (eventObj.headpose == 0)
            score += alpha * coefficients[4];
  
        cout << "Result: ";
        if (score <= -0.07)
            cout << "Conservative" << endl;
        else if (score >= 0.25)
            cout << "Aggressive" << endl;
        else
            cout << "Moderate" << endl;

    } else {
        cout<<"Unknown event:"<<endl;
    }
}

string CreateJson() {
  Json::Value jsonData;
  jsonData["leftUp"] = 18;
  jsonData["event"] = "turn";
  jsonData["speed"] = 10.0;
  jsonData["headpose"] = 1;
  jsonData["turningSignal"] = 1;
  jsonData["steeringAngle"] = 0.0;
  jsonData["turningRadius"] = 0.0;
  jsonData["decel"] = true;
  jsonData["accel"] = false;

  Json::FastWriter fastWriter;
  std::string output = fastWriter.write(jsonData);
  return output;
}


EventObj fromJson(EventObj eventObj, string json) {
  Json::Value parsedFromString;
  Json::Reader reader;
  assert(reader.parse(json, parsedFromString));

  cout<<"inside"<<json<<endl;

  //parse json data and read more data
  eventObj.upperleft_h = parsedFromString["leftUp"].asDouble();
  eventObj.event = parsedFromString["event"].asString();
  eventObj.speed = parsedFromString["speed"].asDouble();
  eventObj.headpose = parsedFromString["headpose"].asInt();
  eventObj.turningSignal = parsedFromString["turningSignal"].asInt();
  eventObj.turningRadius = parsedFromString["turningRadius"].asDouble();
  eventObj.steeringAngle = parsedFromString["steeringAngle"].asDouble();
  eventObj.accel = parsedFromString["accel"].asBool();
  eventObj.decel = parsedFromString["decel"].asBool();
  //eventObj.leftUp = parsedFromString["leftUp"].asDouble();
  //eventObj.leftUp = parsedFromString["leftUp"].asDouble();
  //cout<<"left"<<parsedFromString["leftUp"]<<eventObj.upperleft_h<<endl;

  return eventObj;
}

void CheckTurn(EventObj eventObj){

}

void savePreviousObj(EventObj eventObj){
     EventObj preEventObj = eventObj;
}

void CheckBrake(EventObj eventObj){
//        int objSize = 0;
//        int carize = 111;
//	if (eventObj.box_num = 0) {//object in front 
             //check what obj, check distance, check if it is moveing
//            objSize = abs((eventObj.bottomright_h - eventObj.upperleft_h)*(eventObj.bottomright_v - eventObj.upperleft_v));
        //}else if (eventObj) {

//	} else{
        //     return false;
//        }
}

void CheckLaneChange(EventObj eventObj){

}


/*
 * Vehicle Speed
Accel. / Deceleration
Steering Angle
Turning Radius //由转弯半径R与外轮转角之间的关系是 R=L/Sinθ。+a 可知，转弯半径仅与外轮转角θ。、轴距L、主销到外轮轮辙中心线距离a有关，所以根据车辆的参数应该能直接算出
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

// given three colinear points p, q, r, the function checks if q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r) {
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
            q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
        return true;
    return false;
}

/*
 * find orientation of ordered triplet (p, q, r)
 *   0 --> p q r colinear
 *   1 --> clockwise
 *   2 --> counterclockwise
 */
int orientation(Point p, Point q, Point r) {
    double val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear
    return (val > 0)? 1: 2;  // clock or counterclock wise
}

// returns true if line segment 'p1q1' and 'p2q2' intersect.
bool doIntersect(Point p1, Point q1, Point p2, Point q2) {
    // find the four orientations needed for general and special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
    // general case
    if (o1 != o2 && o3 != o4)
        return true;

    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;

    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;

    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;

     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;

    return false;
}

// returns true if the point p lies inside the polygon[] with n vertices
bool isInside(vector<Point> polygon, Point p) {
    // there must be at least 3 vertices in polygon[]
    int n = polygon.size();
    if (n < 3)  return false;

    // get the MAX
    double maxXCoordinate = -1;
    for (int i = 0; i < n; i++) {
        if (polygon[i].x > maxXCoordinate)
            maxXCoordinate = polygon[i].x;
    }
    // create a point for line segment from p to infinite
    Point extreme = {maxXCoordinate + 1, p.y};

    // count intersections of the above line with sides of polygon
    int count = 0, i = 0;
    do {
        int next = (i + 1) % n;

        // check if the line segment from 'p' to 'extreme' intersects
        if (doIntersect(polygon[i], polygon[next], p, extreme))
        {
            // if the point 'p' is colinear with line segment 'i-next', then check if it lies on segment
            if (orientation(polygon[i], p, polygon[next]) == 0)
               return onSegment(polygon[i], p, polygon[next]);
            count++;
        }
        i = next;
    } while (i != 0);

    // return true if count is odd, false otherwise
    return count % 2 == 1;
}

// returns true if x is between p and q
bool between(double x, double p, double q) {
    if (p > q)
        return x >= q && x <= p;
    else
        return x <= q && x >= p;
}

// returns true if the two polygon has intersection, one with n1 vertices the other with n2
// Note: this requires that the points in each polygon is in order (clockwise or anticlockwise)
bool hasOverlap(vector<Point> polygon1, vector<Point> polygon2) {
    // check whether there's vertices of one inside another
    int n1 = polygon1.size();
    int n2 = polygon2.size();
    for (int i = 0; i < n2; i++) {
        if (isInside(polygon1, polygon2[i])) {
            return true;
        }
    }

    // check for edge intersection
    for (int i = 0; i < n1; i++) {
        int j = i + 1;
        if (i == n1 - 1) j = 0;

        // general form of the equation: Mx + Ny + Q = 0
        float M = polygon1[i].y - polygon1[j].y;
        float N = polygon1[j].x - polygon1[i].x;
        float Q = 0 - M*polygon1[j].x - N*polygon1[j].y;

        for (int k = 0; k < n2; k++) {
            int l = k + 1;
            if (l == n2) l = 0;
            // general form of the equation: Ax + By + C = 0
            float A = polygon2[k].y - polygon2[l].y;
            float B = polygon2[l].x - polygon2[k].x;
            float C = 0 - A*polygon2[l].x - B*polygon2[k].y;

            // if both edge on the same line
            if (A*N-B*M == 0 && B*Q-C*N == 0) {
                if (onSegment(polygon2[k], polygon1[i], polygon2[l]) || onSegment(polygon2[k], polygon1[j], polygon2[l])) {
                    return true;
                }
            } else if (A*N - B*M != 0) {
                float X = (B*Q-C*N) / (A*N-B*M); // x-Coordinate of the intersection
                float Y = (A*Q-M*C) / (M*B-A*N); // y-Coordinate of the intersection
                if (between(X, polygon2[k].x, polygon2[l].x) && between(X, polygon1[i].x, polygon1[j].x)
                    && between(Y, polygon2[k].y, polygon2[l].y) && between(Y, polygon1[i].y, polygon1[j].y)) {
                    return true;
                }
            }
        }
    }

    // there's no intersection & there's no completely cover
    return false;
}
