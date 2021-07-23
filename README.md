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

# Important information about branches.

This repository serves both LArSoft and NOvA.
We use branches to manage different required versions of art and geant4.

Branches for NOvA will be nova_vXX_YY_br.
Since the head of the development is primarily used for LArSoft work, branches for LArSoft releases will only be made when necessary and will be of the form vXX_YY_br.

## Latest LArSoft releases

- v01_08_03 (for art 3.09)
- v01_06_08 on v01_06_br (for art 3.06)

