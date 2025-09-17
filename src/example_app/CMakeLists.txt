find_package(OpenGL REQUIRED)

# ---------- Imgui App --------

set(MODULE_UI_Layers
  ui_layers/main_menu_bar.cpp
  ui_layers/render_scene.cpp
  ui_layers/title_bar.cpp
  ui_layers/custom_main_menu_bar.cpp
)

set(ALL_MODULE_FILES
  ${MODULE_UI_Layers}
)

source_group("" FILES main.cpp icon/app_icon.rc)

foreach(FILE IN LISTS ALL_MODULE_FILES MODULE_Path)
  get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
  string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
  source_group("${GROUP}" FILES "${FILE}")
endforeach()

add_executable(example
  main.cpp
)

file(GLOB_RECURSE HEADERS_Embed "${CMAKE_CURRENT_SOURCE_DIR}/embed/*.h")

source_group("embed" FILES ${HEADERS_Embed})

target_sources(example
  PUBLIC
    FILE_SET CXX_MODULES FILES ${ALL_MODULE_FILES}
    FILE_SET HEADERS FILES ${HEADERS_Embed}
)

target_include_directories(example PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/icon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/embed>"
  ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(example
  gui_core
  pybind11::embed
)

if(COMMAND set_python_rpath)
    set_python_rpath(example)
endif()

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