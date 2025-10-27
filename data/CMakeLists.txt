file(GLOB Src_List ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(data STATIC ${Src_List})

target_link_libraries(data PUBLIC gal)

target_include_directories(data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})