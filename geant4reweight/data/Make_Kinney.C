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

void Make_Kinney(){

std::vector<double>X = {240.0,270.0};
std::vector<double>Y = {886.0,951.0};
std::vector<double>Err = {138.0,116.0}; //y error


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

TFile *f = new TFile("Kinney.root", "RECREATE");

g->Write("He4_xsec_dcex_piplus");

std::vector<double> X2 = {240.0};
std::vector<double> Y2 = {926.0};
std::vector<double> Err2 = {77.0};


TGraphErrors *g2 = new TGraphErrors(X2.size() , &(X2[0]) , &(Y2[0]) , 0 , &(Err2[0]));

g2->Write("He4_xsec_dcex_piminus");



f->Close();
}
