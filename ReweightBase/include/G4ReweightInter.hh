#ifndef G4ReweightInter_h
#define G4ReweightInter_h

#include <vector>
#include <string>
class G4ReweightInter
{

  public:
           G4ReweightInter(std::vector< std::pair<double, double> > pts);
	  ~G4ReweightInter();

           size_t GetNPoints(){ return content.size(); };
           double GetContent( double );
           double GetPoint( size_t );
           double GetValue( size_t ); 

  private:

           std::vector< std::pair< double, double > > content;
        
};
#endif
