# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(e1039_track_gen CXX)

# Include the Fun4AllUniversalOutputManager source and header files
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB dist_headers ${PROJECT_SOURCE_DIR}/*.h)
file(GLOB non_dist_headers ${PROJECT_SOURCE_DIR}/*LinkDef.h)
list(REMOVE_ITEM dist_headers ${non_dist_headers})

# ROOT dictionary generation
add_custom_command (
  OUTPUT e1039_track_gen_Dict.cc
  COMMAND rootcint
  ARGS -f e1039_track_gen_Dict.cc -noIncludePaths -inlineInputHeader
    -I$ENV{OFFLINE_MAIN}/include/ -I${ROOT_PREFIX}/include/
    ${dist_headers}
    ${PROJECT_SOURCE_DIR}/LinkDef.h
  DEPENDS ${dist_headers} ${PROJECT_SOURCE_DIR}/LinkDef.h
)

# ROOT
find_program(ROOTCONF "root-config")
if(ROOTCONF)
  message("-- Detecting ROOT:    found at ${ROOTCONF}")
else()
  message(FATAL_ERROR "-- Detecting ROOT:    not found")
endif()


# Geant4
  find_program(G4CONF "geant4-config")
  if(G4CONF)
    message("-- Detecting Geant4:  found at ${G4CONF}")
  else()
    message(FATAL_ERROR "-- Detecting Geant4:  not found")
  endif()
  #find_package(Geant4 REQUIRED gdml vis_all)
  execute_process(COMMAND geant4-config --prefix OUTPUT_VARIABLE GEANT4_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND geant4-config --cflags OUTPUT_VARIABLE GEANT4_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND geant4-config --libs   OUTPUT_VARIABLE GEANT4_LINK   OUTPUT_STRIP_TRAILING_WHITESPACE)



execute_process(COMMAND root-config --prefix OUTPUT_VARIABLE ROOT_PREFIX  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS  OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND root-config --libs   OUTPUT_VARIABLE ROOT_LINK    OUTPUT_STRIP_TRAILING_WHITESPACE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -I$ENV{OFFLINE_MAIN}/include/ ${ROOT_CFLAGS}")

# Build the shared library
add_library(e1039_track_gen SHARED ${sources} e1039_track_gen_Dict.cc)
target_link_libraries(e1039_track_gen fun4all interface_main geom_svc ktracker phg4hit PHPythia8 g4detectors UtilAna)
install(TARGETS e1039_track_gen DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(FILES ${dist_headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

# Install the pcm files in case of ROOT 6.
execute_process(COMMAND root-config --version OUTPUT_VARIABLE ROOT_VER)
string(SUBSTRING ${ROOT_VER} 0 1 ROOT_VER)
if (ROOT_VER GREATER 5)
   add_custom_target(install_pcm ALL COMMAND mkdir -p ${CMAKE_INSTALL_PREFIX}/lib COMMAND cp -up *_rdict.pcm ${CMAKE_INSTALL_PREFIX}/lib)
   add_dependencies(install_pcm e1039_track_gen)
endif()

