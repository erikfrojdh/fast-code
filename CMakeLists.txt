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
    # 00-RowVsCol.cpp
    01-AddingWork.cpp
    02-BranchPredictor.cpp
    03-Pedestal.cpp
    04-CacheLine.cpp
)
# List of python files to be copied to the build directory
set( PYTHON_FILES
    pedestal.py
)

# Copy the python files to the build directory
foreach(FILE ${PYTHON_FILES})
    configure_file(${FILE} ${CMAKE_BINARY_DIR}/${FILE}  )
endforeach(FILE ${PYTHON_FILES})



find_package(hwy REQUIRED)

#Iterate over the list of examples and compile them
foreach(FILE ${ExampleSrc})
    get_filename_component(EXENAME ${FILE} NAME_WE)
    add_executable(${EXENAME} ${FILE})
    # target_compile_options(${EXENAME} PRIVATE -mavx -mavx2 -maes)
    target_link_libraries(${EXENAME}  aare_core benchmark::benchmark hwy::hwy fmt::fmt)
    set_target_properties(${EXENAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()


add_executable(00-RowVsCol 00-RowVsCol.cpp hwy-add.cpp)
target_link_libraries(00-RowVsCol  aare_core benchmark::benchmark hwy::hwy)
target_include_directories(00-RowVsCol PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(00-RowVsCol PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


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
