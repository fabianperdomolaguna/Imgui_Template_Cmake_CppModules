add_library(gui_core STATIC 
  core/window.cpp
  core/application.cpp
  core/imgui_context.cpp
  core/color_styles.cpp
  core/custom_widgets.cpp
  config/settings_config.cpp
  renderer/texture.cpp
  renderer/framebuffer.cpp
  renderer/vertex.cpp
  image/image_reader.cpp
  shaders/shaders.cpp
  path/path_utilities.cpp
)

target_include_directories(gui_core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
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
