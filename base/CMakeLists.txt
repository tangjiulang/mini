
file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(base STATIC ${Src_List})

target_link_libraries(base PUBLIC 
                        spdlog::spdlog
                        magic_enum
                        math
)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)