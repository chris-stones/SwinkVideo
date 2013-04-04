
add_subdirectory( nc )

# HACK!!!
ADD_DEFINITIONS(-fPIC)



FILE(GLOB c_source_files *.c)
FILE(GLOB cxx_source_files *.cpp)

add_executable(SwinkIt ${c_source_files} ${cxx_source_files} )

target_link_libraries (SwinkIt imgutil nc)

install(TARGETS SwinkIt RUNTIME DESTINATION bin)
