
file(GLOB Src_List ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(math STATIC ${Src_List})

target_link_libraries(math PUBLIC spdlog::spdlog Qt6::Core)

target_include_directories(math PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
