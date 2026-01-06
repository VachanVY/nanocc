add_executable(nanocc
    nanocc.cpp
)

# PUBLIC would work but is wrong
# Public means: Any target that depends on nanocc should also depend on nanocc_lib
# But nanocc is the final executable, nothing depends on it
target_link_libraries(nanocc PRIVATE nanocc_lib)
