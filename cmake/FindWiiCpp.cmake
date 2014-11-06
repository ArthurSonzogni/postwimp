#Search for the include file...
FIND_PATH(WIICPP_INCLUDE_DIRS wiicpp.h
  PATH_SUFFIX include #For finding the include file under the root of the glfw expanded archive, typically on Windows.
  PATHS
  /usr/include/
  /usr/local/include/
  # By default headers are under GLFW subfolder
  /usr/include/wiic
  /usr/local/include/wiic
  ${CMAKE_SOURCE_DIR}/extlibs/headers
)

SET(WIICPP_LIB_NAMES libwiicpp.so wiicpp libwiicpp.a)

#FIND_LIBRARY(WIICPP_LIBRARIES
#    name
#    WIICPP_LIB_NAMES
#    path
#    /usr/local/lib
#    /usr/lib
#)

SET(WIICPP_LIBRARIES /usr/local/lib/libwiicpp.so)

MESSAGE(STATUS WIICPP : incl:${WIICPP_INCLUDE_DIRS})
MESSAGE(STATUS WIICPP : libs:${WIICPP_LIBRARIES})
