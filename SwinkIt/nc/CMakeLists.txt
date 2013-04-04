
# HACK!!!
ADD_DEFINITIONS(-fPIC)

FILE(GLOB c_source_files *.c)
FILE(GLOB cxx_source_files *.cpp)

add_library(nc STATIC ${c_source_files} ${cxx_source_files} )


