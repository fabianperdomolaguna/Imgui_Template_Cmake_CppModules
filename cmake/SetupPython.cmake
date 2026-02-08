function(setup_python_env)
  find_program(UV_EXECUTABLE uv REQUIRED)

  set(VENV_DIR "${CMAKE_BINARY_DIR}/bin/.venv")
  if(WIN32)
    set(VENV_PYTHON "${VENV_DIR}/Scripts/python.exe")
  else()
    set(VENV_PYTHON "${VENV_DIR}/bin/python")
  endif()

  if(NOT EXISTS "${VENV_PYTHON}")
    message(STATUS "UV: Creating python virtual environment at ${VENV_DIR}...")
    execute_process(
      COMMAND ${UV_EXECUTABLE} venv "${VENV_DIR}" --python ${DESIRED_PYTHON_VERSION}
    )
  endif()
  
  execute_process(
    COMMAND ${UV_EXECUTABLE} pip install
            -r "${CMAKE_SOURCE_DIR}/requirements/python/requirements.txt"
            --python "${VENV_PYTHON}"
  )

  set(GLAD_PYTHON "${VENV_PYTHON}" CACHE FILEPATH "" FORCE)
  set(PYTHON_EXECUTABLE "${VENV_PYTHON}" CACHE FILEPATH "" FORCE)

  if(WIN32)
    set(ENV{PATH} "${VENV_DIR}/Scripts;$ENV{PATH}")
  else()
    set(ENV{PATH} "${VENV_DIR}/bin:$ENV{PATH}")
    unset(ENV{CONDA_PREFIX})
    unset(ENV{CONDA_DEFAULT_ENV})
  endif()

  if(WIN32)
    execute_process(
      COMMAND "${VENV_PYTHON}" -c "import sys, os; print(';'.join([sys.base_prefix, os.path.join(sys.base_prefix, 'Library', 'bin'), os.path.join(sys.base_prefix, 'Scripts')]))"
      OUTPUT_VARIABLE PYTHON_PATHS
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
      COMMAND "${VENV_PYTHON}" -c "import sys; print(sys.base_prefix)"
      OUTPUT_VARIABLE PYTHON_BASE
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  
    set(Python3_FIND_STRATEGY "LOCATION" CACHE STRING "" FORCE)
    set(Python3_FIND_REGISTRY "NEVER" CACHE STRING "" FORCE)

    set(PYTHON_BIN_DIR "${PYTHON_BASE}" PARENT_SCOPE)
    set(PYTHON_RUNTIME_PATHS "${PYTHON_PATHS}" PARENT_SCOPE)
    set(VENV_DIR "${VENV_DIR}" PARENT_SCOPE)
  
    message(STATUS "Rutas de runtime detectadas: ${REAL_PYTHON_PATHS}")
  endif()

  if(UNIX)
    execute_process(
      COMMAND "${VENV_PYTHON}" -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))"
      OUTPUT_VARIABLE PYTHON_LIB_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(CMAKE_BUILD_RPATH "${CMAKE_BUILD_RPATH};${PYTHON_LIB_DIR}" PARENT_SCOPE)
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${PYTHON_LIB_DIR}" PARENT_SCOPE)
  endif()
endfunction()