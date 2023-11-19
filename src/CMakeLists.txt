add_library(
    sdlog

    error.c
)

set_property(TARGET sdlog PROPERTY C_STANDARD 99)

target_include_directories(
    sdlog
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_options(
	sdlog
	PRIVATE
	-Wdouble-promotion
)

install(TARGETS sdlog)
