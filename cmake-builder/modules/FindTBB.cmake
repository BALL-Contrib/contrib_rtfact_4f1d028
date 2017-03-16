#
# Find the TBB includes and library
##
# sets TBB_INCLUDE_DIR
#      TBB_LIBRARY
#      TBB_DEBUG_LIBRARY
#      TBB_FOUND
#      TBB_LIBRARIES
#      TBB_LIBRARY_DIRS

FIND_PATH(TBB_INCLUDE_DIR tbb/parallel_for.h
  ${TBB_ROOT}/include
  /usr/local/include
  /usr/include
  ../contrib/include
)


IF(WIN32)
  FIND_LIBRARY(TBB_LIBRARY
               NAMES tbb
               PATHS ${TBB_INCLUDE_DIR}/../lib
               ${TBB_INCLUDE_DIR}/../lib/${ARCH}
			   ../contrib/${ARCH})
  FIND_LIBRARY(TBB_LIBRARY_DEBUG
               NAMES tbbd tbb_debug
               PATHS ${TBB_INCLUDE_DIR}/../lib
               ${TBB_INCLUDE_DIR}/../lib/${ARCH}
			   ../contrib/${ARCH})
ELSE(WIN32)
  FIND_LIBRARY(TBB_LIBRARY tbb
               ${TBB_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
  FIND_LIBRARY(TBB_LIBRARY_DEBUG tbb_debug
               ${TBB_INCLUDE_DIR}/../lib
               /usr/local/lib
               /usr/lib)
ENDIF(WIN32)


SET(TBB_FOUND "NO")
IF(TBB_INCLUDE_DIR AND TBB_LIBRARY)
  SET(TBB_FOUND "YES")
  GET_FILENAME_COMPONENT( TBB_LIBRARY_DIRS ${TBB_LIBRARY} PATH )
  GET_FILENAME_COMPONENT( TBB_LIBRARY ${TBB_LIBRARY} NAME )

  IF(WIN32)
    SET(TBB_LIBRARIES optimized ${TBB_LIBRARY} ${CMAKE_DL_LIBS})
    IF(TBB_LIBRARY_DEBUG)
      GET_FILENAME_COMPONENT( TBB_LIBRARY_DEBUG ${TBB_LIBRARY_DEBUG} NAME )
      SET(TBB_LIBRARIES ${TBB_LIBRARIES} debug ${TBB_LIBRARY_DEBUG})
    ENDIF(TBB_LIBRARY_DEBUG)
  ELSE(WIN32)
    SET(TBB_LIBRARIES general ${TBB_LIBRARY} ${CMAKE_DL_LIBS})
  ENDIF(WIN32)
ENDIF(TBB_INCLUDE_DIR AND TBB_LIBRARY)

SET(TBB_LIBRARIES "${TBB_LIBRARIES}" CACHE STRING "List of TBB libraries")

IF(${TBB_FOUND} STREQUAL "NO")
  MESSAGE("TBB library not found.")
ENDIF(${TBB_FOUND} STREQUAL "NO")

