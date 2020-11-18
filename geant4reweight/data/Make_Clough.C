

void Make_Clough(){

  const int ntargets = 4;

  std::string targs[ntargets] = {"Li","Be","C","O"};

  //TGraphAsymmErrors *g_err[ntargets];


TFile * fout = new TFile("Clough_piM.root", "RECREATE");



 for(int i_t=0;i_t<ntargets;i_t++){

   //only use the file with error bars for now
   std::string  datafile2 = "Clough_NPhysB_76_15_PiM_"  + targs[i_t] + "_Tot_wErr.txt";

  ifstream input_err(datafile2.c_str());

  if(!input_err) continue;

  int N;

  input_err >> N;

  std::vector<Double_t>X;
  std::vector<Double_t>Y;
  std::vector<Double_t>yErr;

  double x,y,err;

  for(int i=0;i<N;i++){

    input_err >> x >> y >> err;

    X.push_back(x);
  Y.push_back(y);

  yErr.push_back(err);


  }

  // TGraphAsymmErrors *g  = new TGraphAsymmErrors(X.size(),&(X[0]),&(Y[0]),0,0,&(yErrLow[0]),&(yErrHigh[0]));

  TGraphErrors *g = new TGraphErrors(X.size(),&(X[0]),&(Y[0]),0,&(yErr[0]));

  std::string graphname = targs[i_t] + "_xsec_total_piminus"; 

  g->Write(graphname.c_str());


 }

 fout->Close();


}
