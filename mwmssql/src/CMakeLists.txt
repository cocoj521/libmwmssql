set(mwmssql_SRCS
	LockBase.cpp 
	DerivedLock.cpp 
	SafeLock.cpp 
	SQLBase.cpp 
	SQLConnection.cpp 
	SQLException.cpp 
	SQLField.cpp 
	SQLResult.cpp 
	SQLValue.cpp 
	MsSqlHelper.cpp 
	MsSqlConnPool.cpp 
	MsSqlOperator.cpp
)

if(NOT CMAKE_BUILD_DYNAMIC_LIB)
	add_library(mwmssql ${mwmssql_SRCS})
else()
	add_library(mwmssql SHARED ${mwmssql_SRCS})
endif()

target_link_libraries(mwmssql tdsodbc pthread rt)

install(TARGETS mwmssql DESTINATION lib)

set(HEADERS
	MsSqlHelper.h
	SQLBase.h
	SQLConnection.h
	SQLException.h
	SQLField.h
	SQLResult.h
	SQLTypes.h
	SQLValue.h
)

install(FILES ${HEADERS} DESTINATION include)
