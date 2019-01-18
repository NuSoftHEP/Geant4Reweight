#ifndef G4ReweightInter_h
#define G4ReweightInter_h

#include <vector>
#include <string>
class G4ReweightInter
{

  public:
           G4ReweightInter(std::vector< std::pair<double, double> > pts);

           G4ReweightInter( const G4ReweightInter &inter ){ 
             for( size_t i = 0; i < inter.GetNPoints(); ++i ) content.push_back( std::make_pair( inter.GetPoint(i), inter.GetValue(i) ) ); 
           };

	  ~G4ReweightInter(){content.clear();};

           const size_t GetNPoints() const { return NPoints; };
           double GetContent( double );
           const double GetPoint( size_t ) const ;
           const double GetValue( size_t ) const ; 

  private:

           std::vector< std::pair< double, double > > content;
           size_t NPoints;
        
};
#endif
