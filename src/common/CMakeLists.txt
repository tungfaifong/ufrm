file(GLOB_RECURSE srcs CONFIGURE_DEPENDS *.cpp *.cc)

include_directories(${PATH_INC_3RD} ./)
link_directories(${PATH_LIB_3RD})

add_library(common STATIC ${srcs})

target_link_libraries(common ${LIB_3RD})

file(GLOB_RECURSE protos ${CMAKE_CURRENT_SOURCE_DIR}/proto/*.proto)

foreach(proto ${protos})
    set(_generated_srcs)

    get_filename_component(_file_name ${proto} NAME_WE)

    list(APPEND _generated_srcs
         "${CMAKE_CURRENT_SOURCE_DIR}/protocol/${_file_name}.pb.cc")
    list(APPEND _generated_srcs
         "${CMAKE_CURRENT_SOURCE_DIR}/protocol/${_file_name}.pb.h")

    add_custom_command(
        OUTPUT ${_generated_srcs}
        COMMAND
            ${PATH_BIN_PROTOBUF}/protoc ARGS
            --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/protocol
            --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/proto ${_file_name}.proto
        DEPENDS ${proto}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM)

    set_source_files_properties(${_generated_srcs} PROPERTIES GENERATED TRUE)

    target_sources(common PRIVATE ${_generated_srcs})
endforeach()
