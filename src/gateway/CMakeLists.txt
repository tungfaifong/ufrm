file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp)

include_directories(${PATH_INC_3RD} ${PATH_SRC}/common ./)
link_directories(${PATH_LIB_3RD})

add_executable(gateway ${srcs})

target_link_libraries(gateway ${LIB_3RD} common)

add_dependencies(gateway common)
