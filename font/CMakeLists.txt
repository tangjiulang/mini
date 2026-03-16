
file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(font STATIC ${Src_List})

target_link_libraries(font PUBLIC
						spdlog::spdlog 
                        Freetype::Freetype
						taocpp::pegtl
						Fontconfig::Fontconfig
						harfbuzz::harfbuzz
                        base
						gal
						

)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(font PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
