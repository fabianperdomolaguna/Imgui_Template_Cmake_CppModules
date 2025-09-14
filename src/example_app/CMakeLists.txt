find_package(OpenGL REQUIRED)

add_executable(example
  ui_layers/main_menu_bar.cpp
  ui_layers/render_scene.cpp
  ui_layers/title_bar.cpp
  ui_layers/custom_main_menu_bar.cpp
  main.cpp
)

target_include_directories(example PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ui_layers>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/icon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/embed>"
  ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(example
  gui_core
  pybind11::embed
)

if (WIN32)
  target_link_options(example PRIVATE
    "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    "$<$<NOT:$<CONFIG:Release>>:/SUBSYSTEM:CONSOLE>"
  )

  target_sources(example PRIVATE icon/app_icon.rc)
endif()

file(COPY app_logo.svg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY cpp_python_logos.jpg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})