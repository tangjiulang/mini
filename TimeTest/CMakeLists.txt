project("TimeTest")

set(CMAKE_AUTOMOC ON)

file(GLOB Src_List ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/*.hxx)

add_executable(TimeTest ${Src_List})

target_link_libraries(TimeTest PUBLIC 
                        Qt6::Core
                        Qt6::Gui
                        Qt6::Widgets
                        Qt6::OpenGL
                        Qt6::OpenGLWidgets
						spdlog::spdlog 
                        base
                        core
						math
                        gal
                        view
                        data
                        GLEW::GLEW
)


target_include_directories(TimeTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
