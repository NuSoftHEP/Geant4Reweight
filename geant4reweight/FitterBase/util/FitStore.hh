#ifndef _FitStore_h
#define _FitStore_h

//struct to store information used in Chi2 calculation
struct Chi2Store {
  std::string cut; //exclusive channel name
  int nPoints=0; //number of points for this exclusive channel
  double chi2=0.; //chi2 value generated from this channel

  Chi2Store(std::string this_cut, int this_nPoints, double this_chi2) : cut(this_cut), nPoints(this_nPoints), chi2(this_chi2) {}
};

#endif

