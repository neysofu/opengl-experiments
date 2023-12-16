cmake_minimum_required(VERSION 3.5)

set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0042 NEW)

project(OPENGL_EXPERIMENTS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} \
   -Wall \
   -Wpedantic \
   -Wextra \
   -Wshadow \
   -Wcomment \
   -Wno-missing-braces \
   -Wno-missing-field-initializers \
   -Wswitch-default \
   -Wcast-align \
   -Wpointer-arith \
   -Wundef \
   -Wuninitialized \
   -Wredundant-decls \
   -Wunreachable-code \
   -Wunused-macros")

set(default_build_type "Debug")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CPP_FLAGS_RELEASE} -O3 -fdata-sections -ffunction-sections -Wl,--gc-sections")

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(MAKE_DIRECTORY "${TEST_TMP_DIR}")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

# Dependencies:
# -----------------

# Glad
add_subdirectory("${LIB_DIR}/glad")

# GLFW
# See <https://www.glfw.org/docs/3.3/build_guide.html#build_link_cmake_source>
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("${LIB_DIR}/glfw")
set_target_properties(glfw PROPERTIES COMPILE_FLAGS "-w")

# Main target
file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")
add_executable(opengl_experiments "${SRC_FILES}")
target_include_directories(opengl_experiments SYSTEM PUBLIC "${LIB_DIR}")
target_link_libraries(opengl_experiments PUBLIC glfw PRIVATE glad)
target_compile_definitions(opengl_experiments PRIVATE
   PROJECT_BUILD_DATE=\"${BUILD_DATE}\"
   PROJECT_VERSION=\"${PROJECT_VERSION}\")

function(add_compile_flag FLAG)
   set_target_properties(opengl_experiments PROPERTIES COMPILE_FLAGS "${FLAG}")
endfunction()

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
   add_compile_flag("-fcolor-diagnostics")
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")
   add_compile_flag("-fdiagnostics-color=always")
endif()
