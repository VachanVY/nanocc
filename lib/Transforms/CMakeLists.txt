add_library(nanoccTransforms
    ConstantFolding.cpp
    CopyPropagation.cpp
    DeadStoreElimination.cpp
    SimplifyCFG.cpp
    PassManager.cpp
)

target_include_directories(nanoccTransforms PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccTransforms PUBLIC
    nanoccIR
)
