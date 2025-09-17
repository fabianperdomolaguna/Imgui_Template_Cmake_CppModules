set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_library(Core SHARED)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/image/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.cpp
)

set(MODULE_Path
  path/path_utilities.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/path/include/*.h
)

target_sources(Core
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
    FILE_SET HEADERS     FILES ${HDR_FILES}
  PRIVATE
    ${MODULE_Path}
)

target_include_directories(Core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/path/include>"
)

target_link_libraries(Core
  glfw
  glad_gl
  ImGui
  nlohmann_json::nlohmann_json
  stb
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_MODULES} ${MODULE_Path} ${HEADER_FILES})

file(COPY ConfigFile.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
