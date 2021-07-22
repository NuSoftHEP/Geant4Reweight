// root-ify the dcex data from

//https://journals.aps.org/prc/abstract/10.1103/PhysRevC.58.1576
//4He, piplus, J Grater et al, PRC 58 Vol 3, 1576 (1998) 


#include "TGraphErrors.h"
#include "TFile.h"

//everything in MeV and millibarn


double mass = 139.57;

//convert KE into momentum
double P(double KE){

return sqrt(KE*KE + 2*KE*mass);


}

void Make_Grater(){

std::vector<double>X = {70.0,80.0,90.0,100.0,115.0,130.0};
std::vector<double>Y = {5.0,27.0,60.0,75.0,131.0,164.0};
std::vector<double>Err = {7.0,18.0,21.0,21.0,30.0,24.0}; //y error


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

TFile *f = new TFile("Grater_piP.root", "RECREATE");

g->Write("He4_xsec_dcex_piplus");

f->Close();
}
