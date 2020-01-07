# GeantReweight
Repository for implementing reweighting of Pion Scattering as simulated by Geant4

# Building on Fermilab Machines
source /path/to/experimental/software
setup -B nutools -v <version> -q <qualifiers (i.e. +e17:+prof)> 
cd <build_dir>
source /path/to/geant4reweight/ups/setup_for_development <-d|-p|-o> <qualifier (i.e. e17:prof)>
buildtool -I /path/to/install/dir/ -bti -jN

Note: when sourcing the geant4reweight setup file, the first flag should 
      match the qualifiers: -d for debug, -p for prof, etc.
