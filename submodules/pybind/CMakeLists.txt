set(PYTHON_PATH "" CACHE PATH "Python environment directory path")
if(NOT PYTHON_PATH)
  message(WARNING "PYTHON_PATH is empty. You must specify the path using -DPYTHON_PATH=/path/python")
endif()

if(WIN32)
  set(Python_ROOT_DIR "${PYTHON_PATH}")
  set(PYTHON_EXECUTABLE "${PYTHON_PATH}/python.exe")
else()
  set(Python_ROOT_DIR "${PYTHON_PATH}")
  set(Python_EXECUTABLE "${PYTHON_PATH}/bin/python3")
endif()

set(PYBIND11_FINDPYTHON ON CACHE BOOL "" FORCE)

include(FetchContent)

FetchContent_Declare(pybind
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.13.6.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
  
FetchContent_MakeAvailable(pybind)

if(PYTHON_PATH AND NOT WIN32)
  find_library(PYTHON_LIBRARY
    python3
    PATHS "${PYTHON_PATH}/lib"
    NO_DEFAULT_PATH
  )

  if(NOT PYTHON_LIBRARY)
    message(WARNING "Could not find libpython inside ${PYTHON_PATH}/lib. RPATH may not work.")
  else()
    message(STATUS "Found Python library: ${PYTHON_LIBRARY}")
  endif()

  function(set_python_rpath target)
    set_target_properties(${target} PROPERTIES
      BUILD_RPATH "${PYTHON_PATH}/lib;$ORIGIN/lib"
      INSTALL_RPATH "${PYTHON_PATH}/lib;$ORIGIN/lib"
    )
  endfunction()
endif()