# _targetName
# _precompiledHeaderFile
# _precompiledSourceFile

IF(CMAKE_COMPILER_IS_GNUCXX)
    EXEC_PROGRAM(
        ${CMAKE_CXX_COMPILER}
        ARGS                    --version
        OUTPUT_VARIABLE _compiler_output)
    STRING(REGEX REPLACE ".* ([0-9]\\.[0-9]\\.[0-9]) .*" "\\1"
           gcc_compiler_version ${_compiler_output})
    #MESSAGE("GCC Version: ${gcc_compiler_version}")
    IF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
        SET(PCHSupport_FOUND TRUE)
    ELSE(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
        IF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
            SET(PCHSupport_FOUND TRUE)
        ENDIF(gcc_compiler_version MATCHES "3\\.4\\.[0-9]")
    ENDIF(gcc_compiler_version MATCHES "4\\.[0-9]\\.[0-9]")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

MACRO(SET_PRECOMPILED_HEADER _targetName _precompiledHeaderFile _precompiledSourceFile)

  IF(NOT NOPCH)

    IF( "${ARGN}" STREQUAL "0")
      SET(_dowarn 0)
    ELSE( "${ARGN}" STREQUAL "0")
      SET(_dowarn 1)
    ENDIF("${ARGN}" STREQUAL "0")

    IF(CMAKE_GENERATOR MATCHES Visual*)

      GET_FILENAME_COMPONENT(precompiledHeaderFileName "${CMAKE_CURRENT_SOURCE_DIR}/${_precompiledHeaderFile}" NAME)

      GET_PROPERTY(oldProps TARGET ${_targetName} PROPERTY COMPILE_FLAGS)
      IF (${oldProps} MATCHES NOTFOUND)
        SET(oldProps "")
      ENDIF(${oldProps} MATCHES NOTFOUND)
      SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${oldProps} /Yu\"${precompiledHeaderFileName}\"")

      GET_PROPERTY(oldProps SOURCE ${_precompiledSourceFile} PROPERTY COMPILE_FLAGS)
      IF (${oldProps} MATCHES NOTFOUND)
        SET(oldProps "")
      ENDIF(${oldProps} MATCHES NOTFOUND)
      SET_SOURCE_FILES_PROPERTIES("${_precompiledSourceFile}" PROPERTIES COMPILE_FLAGS "${oldProps} /Yc\"${precompiledHeaderFileName}\"")

    ELSEIF(CMAKE_GENERATOR MATCHES Unix* AND CMAKE_C_COMPILER MATCHES gcc)

      GET_FILENAME_COMPONENT(_name ${_precompiledHeaderFile} NAME)
      SET(_source "${CMAKE_CURRENT_SOURCE_DIR}/${_precompiledHeaderFile}")
      SET(_outdir "${CMAKE_CURRENT_BINARY_DIR}/${_name}.gch")
      MAKE_DIRECTORY(${_outdir})
      SET(_output "${_outdir}/${CMAKE_BUILD_TYPE}.c++")
      STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
      SET(_compiler_FLAGS ${${_flags_var_name}})

      GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
      FOREACH(item ${_directory_flags})
    LIST(APPEND _compiler_FLAGS "-I${item}")
      ENDFOREACH(item)

      GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
      LIST(APPEND _compiler_FLAGS ${_directory_flags})

      IF (APPLE)
        SET(_compiler_FLAGS ${_compiler_FLAGS} -mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET_DEFAULT})
      ENDIF (APPLE)

      SEPARATE_ARGUMENTS(_compiler_FLAGS)
  #MESSAGE("_compiler_FLAGS: ${_compiler_FLAGS}")
  #message("${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header -o ${_output} ${_source}")
      ADD_CUSTOM_COMMAND(
          OUTPUT ${_output}
          COMMAND ${CMAKE_CXX_COMPILER}
                                  ${_compiler_FLAGS}
                                  -x c++-header
                                  -o ${_output} ${_source}
          DEPENDS ${_source} )
      ADD_CUSTOM_TARGET(${_targetName}_gch DEPENDS ${_output})
      ADD_DEPENDENCIES(${_targetName} ${_targetName}_gch)
  #SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-include ${_name} -Winvalid-pch-H")
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -include ${_name} -Winvalid-pch")
      SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "-include ${_name} -Winvalid-pch")

    ELSEIF(CMAKE_GENERATOR MATCHES Unix* AND CMAKE_C_COMPILER MATCHES icc)

      GET_TARGET_PROPERTY(oldProps ${_targetName} COMPILE_FLAGS)
      if (${oldProps} MATCHES NOTFOUND)
        SET(oldProps "")
      endif(${oldProps} MATCHES NOTFOUND)

      SET(newProperties "${oldProps} -pch")
      SET_TARGET_PROPERTIES(${_targetName} PROPERTIES COMPILE_FLAGS "${newProperties}")

    ELSE(CMAKE_GENERATOR MATCHES Visual*)
      message("Other than Visual Studio, GCC and ICC precompiled headers support is still not implemented")
    ENDIF(CMAKE_GENERATOR MATCHES Visual*)

  ENDIF(NOT NOPCH)

ENDMACRO(SET_PRECOMPILED_HEADER)
