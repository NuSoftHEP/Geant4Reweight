// root-ify the dcex data from

//4He, piplus, I V Falomkin et al, Lett Nuovo Cimento 16 525-530 (1976)
//https://lib-extopc.kek.jp/preprints/PDF/1976/7611/7611082.pdf

#include "TGraphErrors.h"
#include "TFile.h"

//everything in MeV and millibarn


double mass = 139.57;

//convert KE into momentum
double P(double KE){

return sqrt(KE*KE + 2*KE*mass);


}

void Make_Falomkin2(){

std::vector<double>X = {98.0,135.0,145.0,155.0,485.0};
std::vector<double>Y = {300.0,290.0,340.0,130.0,1200.0};
std::vector<double>Err = {100.0,110.0,170.0,70.0,210.0}; //y error


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

TFile *f = new TFile("Falomkin2_piP.root", "RECREATE");

g->Write("He4_xsec_dcex_piplus");

f->Close();
}
