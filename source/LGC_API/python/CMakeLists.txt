# Python ctypes wrapper for the monitoring API.
#
# The native side (the flat C ABI) is compiled into LGC_API itself, so the
# single LGC_API.dll / libLGC_API.so is the only binary the Python wrapper
# needs. This directory therefore builds nothing — it only stages the wrapper
# + tests next to the freshly built LGC_API library and runs the pytest gate.

# Turn OFF if you don't want pytest to gate every build.
option(PYMONITORING_RUN_TESTS_AFTER_BUILD "Run pyMonitoring pytest after the LGC_API build" ON)

set(_pyMonitoring_testsDir "${CMAKE_CURRENT_BINARY_DIR}/tests")

# Copy Python wrapper and test files to the build test directory (configure time)
file(GLOB PYMONITORING_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.py" "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.lgc" "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.lgc2")
foreach(test_file ${PYMONITORING_TEST_FILES})
    file(COPY "${test_file}" DESTINATION "${_pyMonitoring_testsDir}")
endforeach()
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/pyMonitoring.py" DESTINATION "${_pyMonitoring_testsDir}")

# Find Python for test execution only (not required for building)
find_package(Python3 COMPONENTS Interpreter QUIET)

# Create a virtual environment with all the test dependencies
set(_pyMonitoring_testsReady FALSE)
if(Python3_FOUND)
    set(_pyMonitoring_venv          "${CMAKE_CURRENT_BINARY_DIR}/venv")
    set(_pyMonitoring_requirements  "${CMAKE_CURRENT_SOURCE_DIR}/tests/requirements.txt")
    set(_pyMonitoring_reqStamp      "${_pyMonitoring_venv}/.requirements.stamp")
    if(WIN32)
        set(_pyMonitoring_venvPython "${_pyMonitoring_venv}/Scripts/python.exe")
    else()
        set(_pyMonitoring_venvPython "${_pyMonitoring_venv}/bin/python")
    endif()

    # Create the venv once. Subsequent configures find it already there
    # and skip this step.
    if(NOT EXISTS "${_pyMonitoring_venvPython}")
        message(STATUS "pyMonitoring: creating test venv at ${_pyMonitoring_venv} ...")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m venv "${_pyMonitoring_venv}"
            RESULT_VARIABLE _pyMonitoring_venvResult
        )
        if(NOT _pyMonitoring_venvResult EQUAL 0)
            message(WARNING "pyMonitoring: failed to create venv (exit ${_pyMonitoring_venvResult}); pyMonitoring_Tests will be skipped.")
        endif()
    endif()

    # (Re)install the curated dependencies whenever requirements.txt was
    # edited since the last successful install.
    if(EXISTS "${_pyMonitoring_venvPython}" AND "${_pyMonitoring_requirements}" IS_NEWER_THAN "${_pyMonitoring_reqStamp}")
        message(STATUS "pyMonitoring: installing test dependencies into venv from ${_pyMonitoring_requirements} ...")
        execute_process(
            COMMAND ${_pyMonitoring_venvPython} -m pip install --quiet --disable-pip-version-check -r "${_pyMonitoring_requirements}"
            RESULT_VARIABLE _pyMonitoring_pipResult
        )
        if(_pyMonitoring_pipResult EQUAL 0)
            file(TOUCH "${_pyMonitoring_reqStamp}")
        else()
            message(WARNING "pyMonitoring: failed to install requirements (pip exit ${_pyMonitoring_pipResult}); pyMonitoring_Tests will be skipped.")
        endif()
    endif()

    if(EXISTS "${_pyMonitoring_venvPython}" AND EXISTS "${_pyMonitoring_reqStamp}")
        set(_pyMonitoring_testsReady TRUE)
    endif()
endif()

# A custom target (part of ALL) that runs after LGC_API is built: it stages the
# freshly built LGC_API library next to pyMonitoring.py and, when enabled, runs
# the pytest gate. add_custom_command(TARGET ...) cannot be attached to LGC_API
# from here (it is defined in the parent directory), so a dependent target is
# used instead.
set(_pyMonitoring_stageCmd
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LGC_API> "${_pyMonitoring_testsDir}/")

if(_pyMonitoring_testsReady AND PYMONITORING_RUN_TESTS_AFTER_BUILD)
    add_custom_target(pyMonitoring_stage ALL
        ${_pyMonitoring_stageCmd}
        COMMAND ${_pyMonitoring_venvPython} -m pytest tests.py -v
        WORKING_DIRECTORY "${_pyMonitoring_testsDir}"
        COMMENT "Staging LGC_API + running pyMonitoring Python tests (using build venv)"
    )
else()
    add_custom_target(pyMonitoring_stage ALL
        ${_pyMonitoring_stageCmd}
        COMMENT "Staging LGC_API next to pyMonitoring.py"
    )
endif()
add_dependencies(pyMonitoring_stage LGC_API)

# Register the suite with CTest if the venv was provisioned successfully.
if(_pyMonitoring_testsReady)
    add_test(
        NAME pyMonitoring_Tests
        COMMAND ${_pyMonitoring_venvPython} -m pytest tests.py -v
        WORKING_DIRECTORY "${_pyMonitoring_testsDir}"
    )
endif()
