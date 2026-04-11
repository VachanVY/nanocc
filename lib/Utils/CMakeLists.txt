add_library(nanoccUtils
	Utils.cpp
)

target_include_directories(nanoccUtils PUBLIC
	${PROJECT_SOURCE_DIR}/include
)
