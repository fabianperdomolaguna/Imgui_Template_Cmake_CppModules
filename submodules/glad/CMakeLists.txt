include(FetchContent)

FetchContent_Declare(glad
  URL https://github.com/Dav1dde/glad/archive/refs/tags/v2.0.8.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(glad)

add_subdirectory("${glad_SOURCE_DIR}/cmake" glad_cmake)

glad_add_library(glad_gl STATIC API gl:core=4.0)
