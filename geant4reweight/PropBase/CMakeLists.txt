cet_make_library(LIBRARY_NAME PropBaseLib
  SOURCE 
  G4ReweightParameterMaker.cc
  G4ReweightThrowManager.cc
  LIBRARIES
  PUBLIC
  fhiclcpp::fhiclcpp
  ROOT::Matrix
  ROOT::MathCore
  PRIVATE
  ROOT::Hist
)

install_headers()
install_source()
