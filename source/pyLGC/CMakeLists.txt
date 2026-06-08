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
if(Python3_FOUND)
    # Make sure pytest is available for the test runner. Install it into the
    # discovered interpreter on the fly if missing, so CI runners and fresh
    # dev machines don't need to provision it manually.
    execute_process(
        COMMAND ${Python3_EXECUTABLE} -c "import pytest"
        RESULT_VARIABLE _pyLGC_pytestCheck
        OUTPUT_QUIET ERROR_QUIET
    )
    if(NOT _pyLGC_pytestCheck EQUAL 0)
        message(STATUS "pyLGC: pytest not found for ${Python3_EXECUTABLE}, attempting to install it...")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} -m pip install --quiet --disable-pip-version-check pytest
            RESULT_VARIABLE _pyLGC_pipResult
        )
        if(_pyLGC_pipResult EQUAL 0)
            execute_process(
                COMMAND ${Python3_EXECUTABLE} -c "import pytest"
                RESULT_VARIABLE _pyLGC_pytestCheck
                OUTPUT_QUIET ERROR_QUIET
            )
        else()
            message(WARNING "pyLGC: failed to install pytest (pip exit ${_pyLGC_pipResult}); pyLGC_Tests will be skipped.")
        endif()
    endif()

    if(_pyLGC_pytestCheck EQUAL 0)
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/tests/run_tests.py"
            "import subprocess, sys\n"
            "result = subprocess.run([sys.executable, '-m', 'pytest', 'tests.py', '-v'])\n"
            "if result.returncode:\n"
            "    print('\\nWARNING: ========== pyLGC Python tests FAILED ==========')\n"
        )
        add_custom_command(TARGET pyLGC_C POST_BUILD
            COMMAND ${Python3_EXECUTABLE} run_tests.py
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests"
            COMMENT "Running pyLGC Python tests"
        )

        add_test(
            NAME pyLGC_Tests
            COMMAND ${Python3_EXECUTABLE} -m pytest tests.py -v
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests"
        )
    endif()
endif()
