
file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(font STATIC ${Src_List})

target_link_libraries(font PUBLIC 
                        Qt6::Core
                        Qt6::Gui
                        Qt6::Widgets
                        Qt6::OpenGL
                        Qt6::OpenGLWidgets
						spdlog::spdlog 
                        core
						math
                        base

)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(font PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
