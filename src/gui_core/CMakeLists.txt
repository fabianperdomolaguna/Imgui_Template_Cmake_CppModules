add_library(gui_core STATIC)

target_sources(gui_core
  PUBLIC
    FILE_SET CXX_MODULES FILES
      core/window.cpp
      core/application.cpp
      core/imgui_context.cpp
      core/layer.cpp
      core/color_styles.cpp
      renderer/texture.cpp
      utilities/settings_config.cpp
      utilities/image.cpp
)

target_include_directories(gui_core PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/submodules/stb_image>"
)

target_link_libraries(gui_core
  glfw
  glad
  imgui
  nlohmann_json::nlohmann_json
)

file(COPY ConfigFile.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY fonts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})