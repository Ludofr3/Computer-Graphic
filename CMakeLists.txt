# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# Use file(GLOB ...) to collect all .cpp files in the directory
#file(GLOB SOURCES1 "${SOURCE_DIR1}/*.cpp")
#file(GLOB SOURCES2 "${SOURCE_DIR2}/*.cpp")


#execution file name
set(PROJECT_NAME lab)

#Current lab
set(CURRENT lab1)

set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})

find_package(FLTK CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)


file(GLOB_RECURSE SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/*.h"  
)

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/Cyclone/"  "${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT}")

add_executable(${PROJECT_NAME} ${SOURCES}  "lab1/Mover.h" "lab1/MySpring.h" "lab1/MySpring.cpp" "lab1/MoverConnection.h" "lab1/Fire.h" "lab1/FireworksRule.h" "lab1/Fireworks.h" "lab1/Fire.cpp" "lab1/Plane.h" "lab1/Contact.h" "lab1/Contact.cpp"    "lab1/Fluid.h" "lab1/Fluid.cpp"   "lab1/ElipsoidMover.h")

target_link_libraries(${PROJECT_NAME} PRIVATE fltk fltk_gl fltk_forms fltk_images glm::glm)
 
