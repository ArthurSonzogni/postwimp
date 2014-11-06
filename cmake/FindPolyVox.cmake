FIND_PATH(PolyVox_INCLUDE_DIRS PolyVoxCore/Array.h DOC "Path to POLYVOX include directory."
  ${CMAKE_SOURCE_DIR}/extlibs/headers/PolyVoxCore
)

SET(PolyVox_LIB_NAMES libPolyVoxCore.so)

FIND_LIBRARY(PolyVox_LIBRARIES ${PolyVox_LIB_NAMES}
  ${CMAKE_SOURCE_DIR}/extlibs/libs-linux/
  /usr/local/lib
  /usr/lib
)

MESSAGE(STATUS Polyvox finder : )
MESSAGE(STATUS |__ ${PolyVox_INCLUDE_DIRS})
MESSAGE(STATUS |__ ${PolyVox_LIBRARIES})
