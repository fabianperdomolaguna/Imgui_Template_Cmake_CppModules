set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/logging/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/image/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/path/*.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.h
)

add_library(Core SHARED 
  ${SRC_FILES}
  ${HDR_FILES}
)

target_include_directories(Core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(Core
  glfw
  glad_gl
  ImGui
  spdlog::spdlog
  nlohmann_json::nlohmann_json
  stb
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES})

file(COPY ConfigFile.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
