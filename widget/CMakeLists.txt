project(widget)

file(GLOB Src_List ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hxx)

add_library(widget STATIC ${Src_List})

target_link_libraries(widget PUBLIC math gal view data)

target_include_directories(widget PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})