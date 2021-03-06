include_directories(include)
add_subdirectory(db_fixture)

find_package( Gperftools QUIET )
if( GPERFTOOLS_FOUND )
    message( STATUS "Found gperftools; compiling tests with TCMalloc")
    list( APPEND PLATFORM_SPECIFIC_LIBS tcmalloc )
endif()

file(GLOB UNIT_TESTS "tests/*.cpp")
add_executable( chain_test ${UNIT_TESTS} )
target_link_libraries( chain_test db_fixture chainbase sophiatx_chain sophiatx_protocol account_history_plugin witness_plugin debug_node_plugin fc ${PLATFORM_SPECIFIC_LIBS} )

file(GLOB PLUGIN_TESTS "plugin_tests/*.cpp")
add_executable( plugin_test ${PLUGIN_TESTS} )
target_link_libraries( plugin_test db_fixture sophiatx_chain sophiatx_protocol account_history_plugin witness_plugin debug_node_plugin fc ${PLATFORM_SPECIFIC_LIBS} )


#add_subdirectory(smart_contracts)
add_subdirectory(utilities)

if(MSVC)
  set_source_files_properties( tests/serialization_tests.cpp PROPERTIES COMPILE_FLAGS "/bigobj" )
endif(MSVC)

#add_subdirectory( generate_empty_blocks )
