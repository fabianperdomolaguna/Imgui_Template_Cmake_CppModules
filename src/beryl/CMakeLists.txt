set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(beryl SHARED)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/logging/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp
)

target_sources(beryl
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_link_libraries(beryl
  glfw
  glad_gl
  imgui
  stb
  spdlog::spdlog
  nlohmann_json::nlohmann_json
)
