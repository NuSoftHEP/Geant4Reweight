// root-ify the dcex data from
// A Altman et al, PRL 55, 1273 (1985) https://journals.aps.org/prl/abstract/10.1103/PhysRevLett.55.1273

#include "TGraphErrors.h"
#include "TFile.h"

//everything in MeV and millibarn


double mass = 139.57;

//convert KE into momentum
double P(double KE){

return sqrt(KE*KE + 2*KE*mass);


}

void Make_Altman(){

std::vector<double>X = {50.0};
std::vector<double>Y = {16.7*1e-3};
std::vector<double>Err = {1.2*1e-3}; //y error


//convert KE's to momenta
for(size_t i=0;i<X.size();i++){

X.at(i) = P(X.at(i));

}


TGraphErrors *g = new TGraphErrors(X.size() , &(X[0]) , &(Y[0]) , 0 , &(Err[0]));

//g->GetXaxis()->SetTitle("P (MeV)");
//g->GetYaxis()->SetTitle("#sigma (mb)");
//g->Draw("APC");

TFile *f = new TFile("Altman_piP.root", "RECREATE");

g->Write("O18_xsec_dcex_piplus");

f->Close();
}
