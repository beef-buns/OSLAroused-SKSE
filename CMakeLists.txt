cmake_minimum_required(VERSION 3.22)

project(
	OSLAroused_SKSE
	VERSION 2.2.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

      #source_group(TREE "${ROOT_DIR}" FILES ${sources})

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

# target_include_directories(
#        ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME} PRIVATE ryml::ryml)

# add_commonlibsse_plugin(${PROJECT_NAME} SOURCES src/main.cpp src/Utility.h src/Events.h src/QuickTorch.h src/QuickTorch.cpp) # <--- specifies plugin.cpp
# target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(DEFINED ENV{SKYRIM_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_FOLDER}/Data")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_FOLDER}/Data")
endif()

if(DEFINED ENV{SKYRIM_MODS_FOLDER} AND IS_DIRECTORY "$ENV{SKYRIM_MODS_FOLDER}")
    set(OUTPUT_FOLDER "$ENV{SKYRIM_MODS_FOLDER}/OSL Aroused NG")
endif()

if(DEFINED OUTPUT_FOLDER)
    # If you specify an <OUTPUT_FOLDER> (including via environment variables)
    # then we'll copy your mod files into Skyrim or a mod manager for you!

    # Copy the SKSE plugin .dll files into the SKSE/Plugins/ folder
    set(DLL_FOLDER "${OUTPUT_FOLDER}/SKSE/Plugins")

    message(STATUS "SKSE plugin output folder: ${DLL_FOLDER}")

    add_custom_command(
        TARGET "${PROJECT_NAME}"
        POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "${DLL_FOLDER}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
        VERBATIM
    )

    # If you perform a "Debug" build, also copy .pdb file (for debug symbols)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET "${PROJECT_NAME}"
            POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_PDB_FILE:${PROJECT_NAME}>" "${DLL_FOLDER}/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
            VERBATIM
        )
    endif()
endif()
