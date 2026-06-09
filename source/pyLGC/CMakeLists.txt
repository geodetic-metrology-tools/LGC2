add_library(pyLGC_C SHARED pyLGC_C.cpp pyLGC_C.h)

target_compile_definitions(pyLGC_C PRIVATE PYLGC_C_EXPORTS)

target_link_libraries(pyLGC_C PRIVATE
    LGC_Core
    LSAlgorithms
    ProjectFramework
    SpatialObjects
    Logs
    Plugins
)

# Copy Python wrapper and test files to build directory
file(GLOB PYLGC_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py" "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.lgc2")
foreach(test_file ${PYLGC_TEST_FILES})
    file(COPY "${test_file}" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/tests")
endforeach()
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pyLGC.py" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/tests")

# Copy the built DLL/so into the test directory after build
add_custom_command(TARGET pyLGC_C POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:pyLGC_C> "${CMAKE_CURRENT_BINARY_DIR}/tests/"
)

# Find Python for test execution only (not required for building)
find_package(Python3 COMPONENTS Interpreter QUIET)

# Create a virtual environment with all the test dependencies
set(_pyLGC_testsReady FALSE)
if(Python3_FOUND)
    set(_pyLGC_venv          "${CMAKE_CURRENT_BINARY_DIR}/venv")
    set(_pyLGC_requirements  "${CMAKE_CURRENT_SOURCE_DIR}/tests/requirements.txt")
    set(_pyLGC_reqStamp      "${_pyLGC_venv}/.requirements.stamp")
    if(WIN32)
        set(_pyLGC_venvPython "${_pyLGC_venv}/Scripts/python.exe")
    else()
        set(_pyLGC_venvPython "${_pyLGC_venv}/bin/python")
    endif()

    # Create the venv once. Subsequent configures find it already there
    # and skip this step.
    if(NOT EXISTS "${_pyLGC_venvPython}")
        message(STATUS "pyLGC: creating test venv at ${_pyLGC_venv} ...")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m venv "${_pyLGC_venv}"
            RESULT_VARIABLE _pyLGC_venvResult
        )
        if(NOT _pyLGC_venvResult EQUAL 0)
            message(WARNING "pyLGC: failed to create venv (exit ${_pyLGC_venvResult}); pyLGC_Tests will be skipped.")
        endif()
    endif()

    # (Re)install the curated dependencies whenever requirements.txt was
    # edited since the last successful install.
    if(EXISTS "${_pyLGC_venvPython}" AND "${_pyLGC_requirements}" IS_NEWER_THAN "${_pyLGC_reqStamp}")
        message(STATUS "pyLGC: installing test dependencies into venv from ${_pyLGC_requirements} ...")
        execute_process(
            COMMAND ${_pyLGC_venvPython} -m pip install --quiet --disable-pip-version-check -r "${_pyLGC_requirements}"
            RESULT_VARIABLE _pyLGC_pipResult
        )
        if(_pyLGC_pipResult EQUAL 0)
            file(TOUCH "${_pyLGC_reqStamp}")
        else()
            message(WARNING "pyLGC: failed to install requirements (pip exit ${_pyLGC_pipResult}); pyLGC_Tests will be skipped.")
        endif()
    endif()

    if(EXISTS "${_pyLGC_venvPython}" AND EXISTS "${_pyLGC_reqStamp}")
        set(_pyLGC_testsReady TRUE)
    endif()
endif()

# Register the test runner only if the venv was provisioned successfully.
# Otherwise, skip silently.
if(_pyLGC_testsReady)
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tests/run_tests.py"
        "import subprocess, sys\n"
        "result = subprocess.run([sys.executable, '-m', 'pytest', 'tests.py', '-v'])\n"
        "if result.returncode:\n"
        "    print('\\nWARNING: ========== pyLGC Python tests FAILED ==========')\n"
    )
    add_custom_command(TARGET pyLGC_C POST_BUILD
        COMMAND ${_pyLGC_venvPython} run_tests.py
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests"
        COMMENT "Running pyLGC Python tests (using build venv)"
    )

    add_test(
        NAME pyLGC_Tests
        COMMAND ${_pyLGC_venvPython} -m pytest tests.py -v
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests"
    )
endif()
