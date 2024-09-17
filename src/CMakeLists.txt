add_library(
    sdlog

    core/endianness.c
    core/encoder.c
    core/error.c
    core/memory.c
    core/model.c
    core/parser.c
    core/writer.c

    io/base.c
    io/buffer.c
    io/file.c
    io/null.c
)

set_property(TARGET sdlog PROPERTY C_STANDARD 99)

target_include_directories(
    sdlog
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(
	sdlog
	PRIVATE
	-Wdouble-promotion
)

install(TARGETS sdlog)
