#include "G4ReweightInter.hh"

 
G4ReweightInter::G4ReweightInter(std::vector< std::pair< double, double > > input) : content(input) { }

double G4ReweightInter::GetContent( double inputPoint ){

  if( GetNPoints() <= 1 ) return 1.;

  if( inputPoint < GetPoint(0) || inputPoint > GetPoint( GetNPoints() - 1 ) ) return 1.;


  for(size_t i = 0; i < (GetNPoints() - 1); ++i){
    if ( inputPoint > GetPoint(i) && inputPoint < GetPoint(i+1) ){
      double lowerPoint = GetPoint(i);
      double upperPoint = GetPoint(i+1);

      double lowerValue = GetValue(i);
      double upperValue = GetValue(i+1);

      double slope = upperValue - lowerValue;
      slope = slope / ( upperPoint - lowerPoint );

      double deltaX = inputPoint - lowerPoint;

      return lowerValue + slope * deltaX;
    }
  }
  

  return 1.;
}

double G4ReweightInter::GetPoint( size_t i ){
  if( i >= GetNPoints() ) return -1.;

  return content[i].first;
}

double G4ReweightInter::GetValue( size_t i ){
  if( i >= GetNPoints() ) return -1.;

  return content[i].second;
}
