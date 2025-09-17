find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/ui_layers/*.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ui_layers/*.h
)

add_executable(App
  ${SRC_FILES}
  ${HDR_FILES}
  main.cpp
)

target_include_directories(App PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/ui_layers>"
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

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES} ${HDR_FILES})

file(COPY app_logo.svg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY imgui.ini DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY cpp_python_logos.jpg DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY scripts DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})