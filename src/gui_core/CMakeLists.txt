set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(Core SHARED)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/logging/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/path/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/image/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.cpp
)

target_sources(Core
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_link_libraries(Core
  glfw
  glad_gl
  ImGui
  spdlog::spdlog
  stb
  nlohmann_json_modules
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_MODULES})

file(COPY ConfigFile.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
