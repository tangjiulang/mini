file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(ipc-2581 STATIC ${Src_List})

target_link_libraries(ipc-2581 PUBLIC tinyxml2::tinyxml2
)

target_include_directories(ipc-2581  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
