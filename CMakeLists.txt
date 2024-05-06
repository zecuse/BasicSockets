cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(is_msvc $<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>)
  set(debug $<CONFIG:Debug,RelWithDebInfo>)
  set(format $<IF:${is_msvc},$<${debug}:EditAndContinue>,$<${debug}:ProgramDatabase>>)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT ${format})
endif()

project (BasicSockets)

add_library(cxx_setup INTERFACE)
if (${is_msvc})
	target_compile_options(cxx_setup INTERFACE -W4)
elseif (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(cxx_setup INTERFACE -Wall)
	target_compile_options(cxx_setup INTERFACE -Wextra -Wpedantic)
endif()
target_compile_features(cxx_setup INTERFACE c_std_17 cxx_std_20)

add_library(Platform INTERFACE Platform.h)
target_link_libraries(Platform INTERFACE cxx_setup)
target_include_directories(Platform INTERFACE ${PROJECT_SOURCE_DIR})

add_subdirectory(Utilities)

add_executable (BasicSockets BasicSockets.c BasicSockets.h)
target_link_libraries(BasicSockets PRIVATE Platform Utilities)
target_include_directories(BasicSockets INTERFACE ${PROJECT_SOURCE_DIR})

if (WIN32)
	message("Building for Windows.")
	target_sources(BasicSockets PRIVATE "MyWindows/Backend.c")
	target_link_libraries(BasicSockets PRIVATE ws2_32)
elseif (UNIX)
	message("Building for Unix.")
	target_sources(BasicSockets PRIVATE "MyUnix/Backend.c")
else()
	message(FATAL_ERROR "You are building on an unsupported system.")
endif()
