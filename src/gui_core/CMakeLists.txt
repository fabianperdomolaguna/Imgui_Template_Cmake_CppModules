add_library(gui_core STATIC)

set(MODULE_Core
  core/window.cpp
  core/application.cpp
  core/imgui_context.cpp
  core/layer.cpp
  core/color_styles.cpp
  core/custom_widgets.cpp
)

set(MODULE_Config
  config/settings_config.cpp
)

set(MODULE_Renderer
  renderer/texture.cpp
  renderer/framebuffer.cpp
  renderer/vertex.cpp
)

set(MODULE_Image
  image/image_reader.cpp
)

set(MODULE_Shaders
  shaders/shaders.cpp
)

set(MODULE_Path
  path/path_utilities.cpp
)
file(GLOB_RECURSE HEADERS_Path "${CMAKE_CURRENT_SOURCE_DIR}/path/include/*.h")

set(ALL_MODULE_FILES
  ${MODULE_Core}
  ${MODULE_Renderer}
  ${MODULE_Image}
  ${MODULE_Shaders}
  ${MODULE_Config}
)

foreach(FILE IN LISTS ALL_MODULE_FILES MODULE_Path)
  get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
  string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
  source_group("${GROUP}" FILES "${FILE}")
endforeach()

source_group("path\\include" FILES ${HEADERS_Path})

target_sources(gui_core
  PUBLIC
    FILE_SET CXX_MODULES FILES ${ALL_MODULE_FILES}
    FILE_SET HEADERS FILES ${HEADERS_Path}
  PRIVATE
    ${MODULE_Path}
)

target_include_directories(gui_core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/path/include>"
)

target_link_libraries(gui_core
  glfw
  glad_gl
  imgui
  nlohmann_json::nlohmann_json
  stb
)

file(COPY ConfigFile.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})