
file(GLOB Src_List CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(gal STATIC ${Src_List})

target_link_libraries(gal PUBLIC 
                        Qt6::Core
                        Qt6::Gui
                        Qt6::Widgets
                        Qt6::OpenGL
                        Qt6::OpenGLWidgets
                        glm::glm
						spdlog::spdlog 
                        GLEW::GLEW
                        core
						math
                        base
)

include_directories(${Boost_INCLUDE_DIRS})
target_include_directories(gal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)