cmake_minimum_required(VERSION 3.22)

project(
	OSLAroused_SKSE
	VERSION 2.2.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add options for specifying the version
# Only one version may be provided at a time
option(SKYRIM_AE "Compile plugin for Skyrim AE" OFF)
option(SKYRIM_SE "Compile plugin for Skyrim SE" ON)
option(SKYRIM_VR "Compile plugin for Skyrim VR" OFF)

# set(SKYRIM_SUPPORT_AE true)
# set(SKYRIM_SUPPORT_SE true)
# set(SKYRIM_SUPPORT_VR true)

source_group(TREE "${ROOT_DIR}" FILES ${SOURCE_FILES})

set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/Plugin.h")
configure_file(
	"cmake/Plugin.h.in"
	"${VERSION_HEADER}"
	@ONLY
)
source_group("src" FILES "${VERSION_HEADER}")

configure_file(
	"cmake/version.rc.in"
	"${CMAKE_CURRENT_BINARY_DIR}/version.rc"
	@ONLY
)

set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCE_DIR "${ROOT_DIR}/src")
file(
        GLOB
        sources
        "${SOURCE_DIR}/*.cpp"
        "${SOURCE_DIR}/*.h"
        "${SOURCE_DIR}/Papyrus/*.cpp"
        "${SOURCE_DIR}/Papyrus/*.h"
        "${SOURCE_DIR}/Managers/*.cpp"
        "${SOURCE_DIR}/Managers/*.h"
        "${SOURCE_DIR}/Utilities/*.cpp"
        "${SOURCE_DIR}/Utilities/*.h"
        "${SOURCE_DIR}/Integrations/*.cpp"
        "${SOURCE_DIR}/Integrations/*.h")

source_group(TREE "${ROOT_DIR}" FILES ${sources})

find_package(CommonLibSSE CONFIG REQUIRED)
find_package(ryml CONFIG REQUIRED)
#find_package(articuno CONFIG REQUIRED)
#find_path(ARTICUNO_INCLUDE_DIRS "articuno/articuno.h")

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${sources})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
  #      $<INSTALL_INTERFACE:src> ${ARTICUNO_INCLUDE_DIRS}
)

target_include_directories(
        ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

set(MOD_FOLDER "output/")
set(DLL_FOLDER "${MOD_FOLDER}/SKSE/Plugins")

# Create output directory if it does not exist
add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        VERBATIM)

# Copy the output .dll and debug .pdb to the configured output directory
add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}"
        VERBATIM
)

# Enable testing
#enable_testing()

# Process the .in templates in the cmake/ folder for Plugin.h and version.rc
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# Process the CMakeLists.txt in the src/ folder
# add_subdirectory(src)
#add_subdirectory(test)
