#ifndef G4ReweightFitter_h
#define G4ReweightFitter_h

class G4ReweightFitter{
  public:
    G4ReweightFitter(){};
    ~G4ReweightFitter(){};

    virtual void   LoadData(){};
    virtual double DoFit(){};
    virtual void   LoadMC(){};

  private:
};

#endif
