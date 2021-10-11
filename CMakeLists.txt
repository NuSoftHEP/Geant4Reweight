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
cmake_minimum_required (VERSION 3.11)

project(geant4reweight VERSION 01.08.05)

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

  if(NOT "${CMAKE_CXX_STANDARD}")
    set(CMAKE_CXX_STANDARD 17)
  endif()
  
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  
  find_package(cetbuildtools REQUIRED)
  include(CetCMakeEnv)
  cet_cmake_env()
  
  cet_set_compiler_flags(DIAGS CAUTIOUS
    WERROR
    NO_UNDEFINED
    EXTRA_FLAGS -pedantic
    EXTRA_CXX_FLAGS -Wno-unused-local-typedefs
  )
  
  cet_report_compiler_flags()
  
  find_ups_product( cetbuildtools ) # LIBRARY_OUTPUT_DIRECTORY, etc.
  
  #new
  find_ups_product(clhep)
  cet_find_library( CLHEP NAMES CLHEP PATHS $ENV{CLHEP_LIB_DIR} NO_DEFAULT_PATH)
  if( NOT CLHEP )
    message( FATAL_ERROR "CLHEP library not found" )
  endif()
  message("CLHEP? " ${CLHEP} )

  cet_find_library( CETBUILD_EXCEPT NAMES cetlib_except PATHS $ENV{CETLIB_EXCEPT_LIB} )
  message("CETBUILD_EXCEPT? " ${CETBUILD_EXCEPT} )
  cet_find_library( CETBUILD NAMES cetlib PATHS $ENV{CETLIB_LIB} )
  message("CETBUILD? " ${CETBUILD} )

  find_ups_geant4( )
  find_ups_root()
  find_ups_product( fhiclcpp)

  #find_library( CETLIB NAMES cetlib PATHS $ENV{CETLIB_LIB} ) 
  #find_library( CETLIB_EXCEPT NAMES cetlib_except PATHS $ENV{CETLIB_EXCEPT_LIB} ) 
  #find_library( CLHEP NAMES CLHEP PATHS $ENV{CLHEP_LIB_DIR} ) 

  #list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
  #find_package(ROOT REQUIRED COMPONENTS RIO Net)

  #set(FHICL_INCLUDE_DIR $ENV{FHICLCPP_INC})
  #set(CETLIB_EXCEPT_INCLUDE_DIR $ENV{CETLIB_EXCEPT_INC})
  #set(CETLIB_INCLUDE_DIR $ENV{CETLIB_INC})
  #file(GLOB FHICLCPP_LIBS $ENV{FHICLCPP_LIB}/*.so)

  # ADD SOURCE CODE SUBDIRECTORIES HERE
  add_subdirectory(geant4reweight)
  # tests
  add_subdirectory(test)
  # ups - table and config files
  # must be AFTER all other subdirectories
  add_subdirectory(ups)
  
  include(UseCPack) 
#endif()
