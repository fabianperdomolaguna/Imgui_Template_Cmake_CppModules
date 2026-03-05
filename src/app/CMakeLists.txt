find_package(OpenGL REQUIRED)

add_executable(app
  main.cpp
)

file(GLOB_RECURSE CPP_MODULES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/layers/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/python/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/visualization/*.cpp
)

target_sources(app
  PUBLIC
    FILE_SET CXX_MODULES FILES ${CPP_MODULES}
)

target_link_libraries(app
  beryl
  pybind11::embed
)

add_compile_definitions(PYTHON_VERSION="${DESIRED_PYTHON_VERSION}")

if (WIN32)
  target_link_options(app PRIVATE
    $<$<CONFIG:Release>:-Wl,/SUBSYSTEM:WINDOWS>
    $<$<CONFIG:Debug>:-Wl,/SUBSYSTEM:CONSOLE>
  )

  target_sources(app PRIVATE resources/windows_icon/app_icon.rc)
endif()

add_custom_command(TARGET app POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets/runtime
          $<TARGET_FILE_DIR:app>
)
