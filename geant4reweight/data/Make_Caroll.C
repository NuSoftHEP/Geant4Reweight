

void Make_Caroll(){

  const int ntargets = 10;

  std::string targs[ntargets] = {"4He","9Be","Li","C","O","Al","32S","Fe","Sn","Pb"};

  //TGraphAsymmErrors *g_err[ntargets];


TFile * fout = new TFile("Caroll_piP.root", "RECREATE");



 for(int i_t=0;i_t<ntargets;i_t++){

   //only use the file with error bars for now
   std::string  datafile2 = "Caroll_PRC_V14N2_PiP_"  + targs[i_t] + "_Tot_wErr.txt";

  ifstream input_err(datafile2.c_str());

  if(!input_err) continue;

  int N;

  input_err >> N;

  std::vector<Double_t>X;
  std::vector<Double_t>Y;
  std::vector<Double_t>yErrLow;
  std::vector<Double_t>yErrHigh;
  std::vector<Double_t>yErr;

  double x,y,errlow,errhigh;

  double err;

  //  std::cout << targs[i_t] << "  "  << N << std::endl;

  for(int i=0;i<N;i++){

    input_err >> x >> y >> errlow >> errhigh;


//convert x from KE into momentum

x = sqrt(x*x + 2*x*139.57);


//convert to mb
y *= 1e3;
errlow *= 1e3;
errhigh *= 1e3;

    errlow = y - errlow;
    errhigh = errhigh - y;

    X.push_back(x);
  Y.push_back(y);
  yErrLow.push_back(errlow);
  yErrHigh.push_back(errhigh);

  //symmetrize errors

  err = (errhigh + errlow)/2;
  yErr.push_back(err);


  }

  // TGraphAsymmErrors *g  = new TGraphAsymmErrors(X.size(),&(X[0]),&(Y[0]),0,0,&(yErrLow[0]),&(yErrHigh[0]));

  TGraphErrors *g = new TGraphErrors(X.size(),&(X[0]),&(Y[0]),0,&(yErr[0]));

  std::string graphname = targs[i_t] + "_xsec_total_piplus"; 

  g->Write(graphname.c_str());


 }

 fout->Close();


}
