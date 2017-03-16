#!/bin/bash
# Script which prepare building environment for CMake

_baseURL () {
  echo $1|sed s/\\/[^\\/]*$//
}

if test -d cmake-builder 
then
  echo "CMake-builder system is already present, you can pull it manualy."
else
  GIT_REMOTE_URL=`git config --get remote.origin.url`
  GIT_BASE_URL=`_baseURL ${GIT_REMOTE_URL}`
  GIT_CMAKE_BUILDER="${GIT_BASE_URL}/cmake-builder.git"
  echo "-----------------------------------------------------------------------"
  echo "Cloning cmake-builder system from ${GIT_CMAKE_BUILDER}."

  git clone "${GIT_CMAKE_BUILDER}"  
  GIT_RETVAL=$?
  
  if [ ${GIT_RETVAL} -eq 0 ]; then
    echo "Successfully cloned builder engine from ${GIT_CMAKE_BUILDER}"
  else
    echo "Could not clone builder from ${GIT_CMAKE_BUILDER}"
  fi
fi
