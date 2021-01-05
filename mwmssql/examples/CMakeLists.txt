set(tests_SRCS
	test.cpp
  )

#include_directories(${PROJECT_SOURCE_DIR}/mwmssql/examples/include)
#link_directories(${PROJECT_SOURCE_DIR}/mwmssql/examples/lib)

add_executable(mwmssql_test test.cpp)
target_link_libraries(mwmssql_test
    mwmssql
    pthread
    z
    rt
    dl
)
