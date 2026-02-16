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
      COMMAND "${VENV_PYTHON}" -c "import sys; print(sys.base_prefix)"
      OUTPUT_VARIABLE PYTHON_DLL_DIR
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    file(TO_NATIVE_PATH "${PYTHON_DLL_DIR}" PYTHON_DLL_DIR_NATIVE)

    set(PS_COMMAND "[Environment]::GetEnvironmentVariable('Path', 'User')")
    set(PS_ADD_PATH "$currentPath = ${PS_COMMAND}; 
      if ($currentPath -notlike '*${PYTHON_DLL_DIR_NATIVE}*') 
        { [Environment]::SetEnvironmentVariable('Path', $currentPath + ';${PYTHON_DLL_DIR_NATIVE}', 'User') }")

    execute_process(
      COMMAND powershell -Command "${PS_ADD_PATH}"
      RESULT_VARIABLE PS_RESULT
    )

    if(PS_RESULT EQUAL 0)
      message(STATUS "Route added to PATH (User) Environment variable")
    else()
      message(WARNING "Failed to modify PATH (User) Environment variable")
    endif()
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