// root-ify the dcex data from
//14C -> 14Oxygen , piplus, M J Leitch et al , PRL 54, 1482 (1985) 
//https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.54.1482

#include "TGraphErrors.h"
#include "TFile.h"

//everything in MeV and millibarn


double mass = 139.57;

//convert KE into momentum
double P(double KE){

return sqrt(KE*KE + 2*KE*mass);


}

void Make_Falomkin(){

std::vector<double>X = {100.0};
std::vector<double>Y = {300.0};
std::vector<double>Err = {150.0}; //y error


//convert KE's to momenta, convert microbarn to millibarn
for(size_t i=0;i<X.size();i++){

X.at(i) = P(X.at(i));
Y.at(i) = Y.at(i)*1e-3;
Err.at(i) = Err.at(i)*1e-3;

}


TGraphErrors *g = new TGraphErrors(X.size() , &(X[0]) , &(Y[0]) , 0 , &(Err[0]));

//g->GetXaxis()->SetTitle("P (MeV)");
//g->GetYaxis()->SetTitle("#sigma (mb)");
//g->Draw("APC");

TFile *f = new TFile("Falomkin_piP.root", "RECREATE");

g->Write("He4_xsec_dcex_piplus");

f->Close();
}
