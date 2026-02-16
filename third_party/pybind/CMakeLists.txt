set(PYBIND11_FINDPYTHON ON)

include(FetchContent)

FetchContent_Declare(pybind
  URL https://github.com/pybind/pybind11/archive/refs/tags/v2.13.6.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
  
FetchContent_MakeAvailable(pybind)