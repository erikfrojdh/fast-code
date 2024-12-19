cmake_minimum_required(VERSION 3.14)
project(example)
set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
#Use cmake to find aare

include(cmake/helpers.cmake)
default_build_type("Release")

find_package(aare REQUIRED)
find_package(benchmark REQUIRED)

#List of examples to be compiled
set(ExampleSrc
    00-RowVsCol.cpp
    01-AddingWork.cpp
    02-BranchPredictor.cpp
)

find_package(hwy REQUIRED)

#Iterate over the list of examples and compile them
foreach(FILE ${ExampleSrc})
    get_filename_component(EXENAME ${FILE} NAME_WE)
    add_executable(${EXENAME} ${FILE})
    target_link_libraries(${EXENAME}  aare_core benchmark::benchmark hwy::hwy)
    set_target_properties(${EXENAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()


#Some examples require ROOT. Only enable them if the user requests it
option(ENABLE_ROOT_EXAMPLES "Enable ROOT examples" OFF)


if(ENABLE_ROOT_EXAMPLES)
    find_package(ROOT CONFIG REQUIRED)

    set(RootExampleSrc
        04-TH1.cpp
    )

    foreach(FILE ${RootExampleSrc})
        get_filename_component(EXENAME ${FILE} NAME_WE)
        add_executable(${EXENAME} ${FILE})
        target_link_libraries(${EXENAME}  aare_core ROOT::Hist ROOT::Graf ROOT::Gpad ROOT::Core)
        set_target_properties(${EXENAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endforeach()
endif()