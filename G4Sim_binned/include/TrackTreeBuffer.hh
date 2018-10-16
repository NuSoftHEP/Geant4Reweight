#ifndef TrackTreeBuffer_h
#define TrackTreeBuffer_h 1

#include <vector>
#include <map>
#include <string>
#include <utility>

struct TrackTreeBuffer{
  int trackID;
  int PID;
  int parID;
  int eventNum;  
  double xi, yi, zi;

  double Energy;

  //Gives bounds for entries within step tree
  std::pair<int,int> * steps;
};

#endif
