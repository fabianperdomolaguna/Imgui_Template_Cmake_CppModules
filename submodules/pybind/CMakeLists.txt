set(PYTHON_PATH "" CACHE PATH "Python environment directory path")
if(NOT PYTHON_PATH)
   message(WARNING "PYTHON_PATH is empty. You must specify the path using -DPYTHON_PATH=/path/python")
endif()

if(WIN32)
   set(PYTHON_EXECUTABLE "${PYTHON_PATH}/python.exe")
else()
   set(PYTHON_EXECUTABLE "${PYTHON_PATH}/bin/python3")
endif()

include(FetchContent)

FetchContent_Declare(pybind
   URL https://github.com/pybind/pybind11/archive/refs/tags/v2.13.6.tar.gz
   DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
  
FetchContent_MakeAvailable(pybind)