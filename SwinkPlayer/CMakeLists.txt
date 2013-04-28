
# HACK!!!
ADD_DEFINITIONS(-fPIC)

FILE(GLOB c_source_files *.c)
FILE(GLOB cxx_source_files *.cpp)

add_executable(SwinkPlayer ${c_source_files} ${cxx_source_files} )

if(RHWIN_GLES2)
  target_link_libraries (SwinkPlayer rh_window_egl img avformat avcodec avutil)
endif()

if(RHWIN_GLX)
  target_link_libraries (SwinkPlayer rh_window_glx img avformat avcodec avutil)
endif()

install(TARGETS SwinkPlayer RUNTIME DESTINATION bin)

