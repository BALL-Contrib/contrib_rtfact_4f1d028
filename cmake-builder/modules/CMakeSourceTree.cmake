# relative_from - folder from which filters should be relative to
# root - from which all files will be listed
# filter_prefix - part of filter path which will be prefixed to each filter
# files_variable - variable to store filenames to
# filefilter - file filter in form of globbing expresion
function(build_source_tree root relative_from filter_prefix files_variable filefilter exclude_patterns)

  file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${root} absroot)
  if(NOT EXISTS ${absroot})
    message("build_source_tree: root=${root} does not exist!")
    return()
  endif()

  file(TO_CMAKE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/${relative_from} absrelative_from)
  if(NOT EXISTS ${absrelative_from})
    message("build_source_tree: relative_from=${relative_from} does not exist!")
    return()
  endif()

  file(GLOB_RECURSE files RELATIVE ${absrelative_from} ${absroot}/${filefilter})

  FOREACH(infileName ${files})

    #exclude
    SET(exclude 0)
    FOREACH(excludePattern ${exclude_patterns})
      string(REGEX MATCH "${excludePattern}" match "${infileName}")
      string(COMPARE NOTEQUAL "${match}" "" isExcluded)
      if(${isExcluded})
        set(exclude 1)
      endif()
    ENDFOREACH(excludePattern)

    IF(NOT ${exclude})
      #remove filename
      set(infileName "/${infileName}")
      string(REGEX REPLACE "/[^/]*$" "/" folder ${infileName})

      #remove ending/starting slash if it is not the only character left
      string(COMPARE NOTEQUAL ${folder} "/" isEmpty)
      if(${isEmpty})
        string(REGEX REPLACE "/$" "" folder ${folder})
        string(REGEX REPLACE "^/" "" folder ${folder})
      endif()

      #create filter path
      string(REGEX REPLACE "/" "\\\\\\\\" filter ${folder})

      #if folder does not exit add it to the list of folders
      list(FIND allFilters ${filter} exist)
      if(${exist} EQUAL -1)
        list(APPEND allFilters "${filter}")
        file(GLOB filesInFolder ${absrelative_from}/${folder}/${filefilter})

        source_group(${filter_prefix}\\${filter} FILES ${filesInFolder})
      endif()

      set(absoluteFile ${absrelative_from}${infileName})
      canonizePath(${absoluteFile} canonizedFileName)
      set(${files_variable} ${${files_variable}} ${canonizedFileName})
    ENDIF()

  ENDFOREACH(infileName)

  set(${files_variable} ${${files_variable}} PARENT_SCOPE)

endfunction(build_source_tree)
