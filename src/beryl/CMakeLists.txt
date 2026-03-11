set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/logging/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/renderer/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/**/**/*.h
)

add_library(beryl SHARED 
  ${SRC_FILES}
  ${HDR_FILES}
)

target_include_directories(beryl PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(beryl
  glfw
  glad_gl
  imgui
  spdlog::spdlog
  nlohmann_json::nlohmann_json
  stb
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES})