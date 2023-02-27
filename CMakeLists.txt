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


# use cmake 3.11 or later
cmake_minimum_required (VERSION 3.20 FATAL_ERROR)

find_package(cetmodules REQUIRED)
project(geant4reweight VERSION 01.14.04)

# cetbuildtools contains our cmake modules
#find_package(cetbuildtools REQUIRED)


#### try a cmake flag 
#if(NOT DEFINED USEART)
#  #set(USEART TRUE)
#  set(USEART FALSE)
#endif()

#if(USEART)
#  message("USING ART")
#  
#  find_package(cetbuildtools REQUIRED)
#  
#  list(APPEND CMAKE_MODULE_PATH
#       $ENV{CANVAS_ROOT_IO_DIR}/Modules ## Get rid of these?
#       $ENV{ART_DIR}/Modules
#  #     ${CMAKE_CURRENT_SOURCE_DIR}/Modules
#  )
#  
#  include(CetCMakeEnv)
#  cet_cmake_env()
#  
#  cet_set_compiler_flags(DIAGS CAUTIOUS
#    WERROR
#    NO_UNDEFINED
#    EXTRA_FLAGS -pedantic
#    EXTRA_CXX_FLAGS -Wno-unused-local-typedefs
#  )
#  
#  cet_report_compiler_flags()
#  
#  # these are minimum required versions, not the actual product versions
#  #find_ups_product( larsoft )
#  find_ups_product( nutools ) #should get rid of this I think
#  find_ups_product( art ) #should also get rid of this
#  #find_ups_product( root ) #eventually replace with this
#  find_ups_product( cetbuildtools ) # LIBRARY_OUTPUT_DIRECTORY, etc.
#  
#  #new
#  find_ups_geant4( )
#  
#  # macros for dictionary and simple_plugin
#  include(ArtDictionary) #Get rid of these once the rest of the build is independent of art
#  include(ArtMake)
#  include(BuildPlugins) #This one too?
#  
#  # add cet_find_library commands here when needed
#  
#  # ADD SOURCE CODE SUBDIRECTORIES HERE
#  add_subdirectory(geant4reweight)
#  # tests
#  add_subdirectory(test)
#  # ups - table and config files
#  # must be AFTER all other subdirectories
#  add_subdirectory(ups)
#  
#  # packaging utility - must be last
#  include(UseCPack) 
#  
#  
#
#else()
  #message( FATAL_ERROR "NOT USING ART" )


  #LG if(NOT "${CMAKE_CXX_STANDARD}")
  #LG   set(CMAKE_CXX_STANDARD 17)
  #LG endif()
  
  #LG set(CMAKE_CXX_STANDARD_REQUIRED ON)
  #LG set(CMAKE_CXX_EXTENSIONS OFF)
  
  include(CetCMakeEnv)
  cet_cmake_env()
  
  cet_set_compiler_flags(DIAGS CAUTIOUS
    WERROR
    NO_UNDEFINED
    EXTRA_FLAGS -pedantic
    EXTRA_CXX_FLAGS -Wno-unused-local-typedefs
  )
  
  cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)
  
  #new
  find_package(cetlib REQUIRED)
  find_package(cetlib_except REQUIRED)
  find_package(CLHEP REQUIRED)
  find_package(fhiclcpp REQUIRED EXPORT)

  find_package( Geant4 REQUIRED )
  find_package(ROOT REQUIRED COMPONENTS Hist Tree Core Gpad)
  #find_package(ROOT REQUIRED COMPONENTS RIO Net)
  #message(STATUS "ROOT_BASIC_LIB_LIST: ${ROOT_BASIC_LIB_LIST}")

  # ADD SOURCE CODE SUBDIRECTORIES HERE
  add_subdirectory(geant4reweight)
  # tests
  add_subdirectory(test)
  # ups - table and config files
  # must be AFTER all other subdirectories
  
#endif()
cet_cmake_config()
