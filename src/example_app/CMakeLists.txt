find_package(OpenGL REQUIRED)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/layers/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/python_interpreter/*.cpp
)

file(GLOB_RECURSE HDR_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/include/widgets/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/components/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/layers/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/python_interpreter/*.h
)

add_executable(App
  ${SRC_FILES}
  ${HDR_FILES}
  main.cpp
)

target_include_directories(App PRIVATE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/widgets>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/components>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/layers>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/python_interpreter>"
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

if(PYTHON_PATH AND WIN32)
  set_target_properties(App PROPERTIES
  VS_DEBUGGER_ENVIRONMENT
    "PATH=${PYTHON_PATH};${PYTHON_PATH}/Library/bin;%PATH%"
  )

  file(TO_CMAKE_PATH "${PYTHON_PATH}" PYTHON_PATH_NORM)
  get_filename_component(CONDA_ENV "${PYTHON_PATH_NORM}" NAME)
  get_filename_component(CONDA_ROOT "${PYTHON_PATH_NORM}/../.." REALPATH)

  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/App_Launcher.bat.in"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/App_Launcher.bat"
    @ONLY
  )
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