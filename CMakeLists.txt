cmake_minimum_required(VERSION 3.26)

project(amx_base64
	DESCRIPTION ""
	HOMEPAGE_URL ""
	LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS OFF)

set(
	src_files 
	src/amx_base64.c
)

add_library(${PROJECT_NAME} OBJECT ${src_files})
target_include_directories(${PROJECT_NAME} PUBLIC src)

add_compile_options(-mcpu=apple-m4)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_compile_options(-O3)
	set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
else()
	add_compile_options(-Og -fcolor-diagnostics)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_executable(amx_base64_tests tests/amx_base64_tests.c)
target_link_libraries(amx_base64_tests PRIVATE ${PROJECT_NAME})
