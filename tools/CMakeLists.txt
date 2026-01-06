if (BUILD_NANOCC_TEST)
    add_executable(nanocc_test test_nanocc.cpp)
    target_link_libraries(nanocc_test PRIVATE nanocc_lib)
else()
    add_executable(nanocc nanocc.cpp)
    target_link_libraries(nanocc PRIVATE nanocc_lib)
endif()

# PUBLIC would work but is wrong
# Public means: Any target that depends on nanocc should also depend on nanocc_lib
# But nanocc is the final executable, nothing depends on it
