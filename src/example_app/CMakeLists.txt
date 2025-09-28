find_package(OpenGL REQUIRED)

# ---------- Imgui App --------

add_executable(App
  main.cpp
)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/layers/*.cpp
)

target_sources(App
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_include_directories(App PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/icon>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/embed>"
  ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(App
  Core
  pybind11::embed
)

if(COMMAND set_python_rpath)
    set_python_rpath(App)
endif()

if (WIN32)
  target_link_options(App PRIVATE
    "$<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>"
    "$<$<NOT:$<CONFIG:Release>>:/SUBSYSTEM:CONSOLE>"
  )

  target_sources(App PRIVATE icon/app_icon.rc)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_MODULES})

file(COPY app_logo.svg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY cpp_python_logos.jpg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
