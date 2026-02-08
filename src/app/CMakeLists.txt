find_package(OpenGL REQUIRED)

add_executable(App
  main.cpp
)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/layers/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/python_interpreter/*.cpp
)

target_sources(App
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_link_libraries(App
  Core
  pybind11::embed
)

add_compile_definitions(PYTHON_VERSION="${DESIRED_PYTHON_VERSION}")

if(WIN32)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/App_Launcher.bat.in"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/App_Launcher.bat"
    @ONLY
  )
endif()

if (WIN32)
  target_link_options(App PRIVATE
    $<$<CONFIG:Release>:-Wl,/SUBSYSTEM:WINDOWS>
    $<$<CONFIG:Debug>:-Wl,/SUBSYSTEM:CONSOLE>
  )

  target_sources(App PRIVATE resources/windows_icon/app_icon.rc)
endif()

add_custom_command(TARGET App POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets/runtime
          $<TARGET_FILE_DIR:App>
)
