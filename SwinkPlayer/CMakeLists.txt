
# HACK!!!
ADD_DEFINITIONS(-fPIC)

FILE(GLOB c_source_files *.c)
FILE(GLOB cxx_source_files *.cpp)

add_executable(SwinkPlayer ${c_source_files} ${cxx_source_files} )

target_link_libraries (SwinkPlayer rh_window_glx img avutil avcodec avformat)

install(TARGETS SwinkPlayer RUNTIME DESTINATION bin)
