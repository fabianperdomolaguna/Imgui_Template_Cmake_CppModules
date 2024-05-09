find_package(OpenGL REQUIRED)

# ---------- Imgui App --------

add_executable(example
  main.cpp
)

target_sources(example
  PUBLIC
    FILE_SET CXX_MODULES FILES
      ui_layers/main_menu_bar.cpp
      ui_layers/render_scene.cpp
)

target_include_directories(example PRIVATE
  ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(example
  gui_core
  pybind11::embed
)

file(COPY imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY cpp_python_logos.jpg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})