 cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("mini")

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	add_compile_options(/Zc:__cplusplus)
endif()


find_package(Boost REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL Widgets OpenGLWidgets)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)

message(STATUS "Qt6 Core include dirs: ${Qt6Core_INCLUDE_DIRS}")
message(STATUS "Qt6 Widgets include dirs: ${Qt6Widgets_INCLUDE_DIRS}")
message(STATUS "Qt6 Gui include dirs: ${Qt6Gui_INCLUDE_DIRS}")
message(STATUS "Qt6 OpenGL include dirs: ${Qt6OpenGL_INCLUDE_DIRS}")
message(STATUS "Qt6 OpenGLWidgets include dirs: ${Qt6OpenGLWidgets_INCLUDE_DIRS}")


message(${CMAKE_TOOLCHAIN_FILE})

add_subdirectory(thirdparty)
add_subdirectory(base)
add_subdirectory(core)
add_subdirectory(math)
add_subdirectory(gal)
add_subdirectory(view)
add_subdirectory(data)
add_subdirectory(resources)

add_subdirectory(TimeTest)

add_executable (mini "mini.cpp" "mini.h" )

target_link_libraries(mini PUBLIC 
                        Qt6::Core
                        Qt6::Gui
                        Qt6::Widgets
                        Qt6::OpenGL
                        Qt6::OpenGLWidgets
						spdlog::spdlog 
						math
                        gal
                        GLEW::GLEW
)

project("TestGAL")

add_executable(TestGAL "test/testGal.cpp")
target_link_libraries(TestGAL PUBLIC 
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
                        GLEW::GLEW
)

project("TestView")

add_executable(TestView "test/testView.cpp")
target_link_libraries(TestView PUBLIC 
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
