file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(data STATIC ${Src_List})

target_link_libraries(data PUBLIC gal view ipc-2581 magic_enum font)

target_include_directories(data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
