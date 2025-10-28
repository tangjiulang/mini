file(GLOB Src_List ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(view STATIC ${Src_List})

target_link_libraries(view PUBLIC math gal)

target_include_directories(view PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})