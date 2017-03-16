# Search module for URay CONTRIB directory
# Author: Lukas Marsalek, marsalek@cs.uni-saarland.de

# See accompanying LICENSE.txt file for terms of use

#  As an output, defines following variables:
#  CONTRIB_ROOT_DIR   - root directory of the contrib package
#  CONTRIB_FOUND      - TRUE if contrib package was found somewhere


FIND_PATH( CONTRIB_ROOT_DIR contrib_version.txt
        PATHS ${CONTRIB_ROOT} $ENV{CONTRIB_ROOT} ./contrib ../contrib ../../contrib)

IF(CONTRIB_ROOT_DIR)
  SET(CONTRIB_FOUND TRUE)
  MESSAGE(STATUS "CONTRIB package found in ${CONTRIB_ROOT_DIR}")
ELSE(CONTRIB_ROOT_DIR)
  SET(CONTRIB_FOUND FALSE)
  MESSAGE("CONTRIB package not found.")
ENDIF(CONTRIB_ROOT_DIR)
