# ======================================================================
#  geant4reweight main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/geant4reweight/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================

cmake_minimum_required (VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(geant4reweight VERSION 01.14.04)

  include(CetCMakeEnv)
  cet_cmake_env()
  
  cet_set_compiler_flags(DIAGS CAUTIOUS
    WERROR
    NO_UNDEFINED
    EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
  )
  
  cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

  option(WANT_G4DDCrossSection "Build the optional G4DDCrossSection application" NO)

  find_package(Geant4 REQUIRED EXPORT)
  find_package(ROOT COMPONENTS Gpad Hist MathCore Matrix RIO Tree REQUIRED EXPORT)
  find_package(cetlib REQUIRED EXPORT)
  find_package(cetlib_except REQUIRED EXPORT)
  find_package(fhiclcpp REQUIRED EXPORT)

  # ADD SOURCE CODE SUBDIRECTORIES HERE
  add_subdirectory(geant4reweight)
  # tests
  add_subdirectory(test)
  
cet_cmake_config()
