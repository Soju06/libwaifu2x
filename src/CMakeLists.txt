cmake_minimum_required(VERSION 3.9)

cmake_policy(SET CMP0091 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(libwaifu2x VERSION 0.0.1 DESCRIPTION "waifu2x-ncnn-vulkan library")

message("build type: ${CMAKE_BUILD_TYPE}")

get_directory_property(HAS_PARENT PARENT_DIRECTORY)

################################################################
########################### options ############################
################################################################

# ON: build with system libncnn
option(USE_SYSTEM_NCNN "build with system libncnn" FALSE)
# Options to avoid vulkan loader library links in MacOS
option(USE_STATIC_MOLTENVK "link moltenvk static library" OFF)

if(NOT DEFINED WAIFU2X_BUILD_TYPE)
    # compile type: executable, dynamic_library, static_library
    if(NOT "${HAS_PARENT}" STREQUAL "")
        set(WAIFU2X_BUILD_TYPE "static_library")
    else()
        set(WAIFU2X_BUILD_TYPE "dynamic_library")
    endif()
endif()
if(NOT DEFINED WAIFU2X_COPY_MODEL_FILE)
    # ON: Copy the model file to the output folder.
    set(WAIFU2X_COPY_MODEL_FILE true)
endif()

set(CMAKE_CXX_STANDARD 23)

################################################################
####################### build directory ########################
################################################################

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/build/debug")
else()
    set(OUTPUT_DIR "${CMAKE_CURRENT_LIST_DIR}/build/release")
endif()

# create binary dir
FILE(MAKE_DIRECTORY ${OUTPUT_DIR})
get_filename_component(CMAKE_BINARY_DIR ${OUTPUT_DIR} ABSOLUTE)
set(RUNTIME_DIR ${CMAKE_BINARY_DIR}/bin)
set(INCLUDE_DIR ${CMAKE_BINARY_DIR}/include)

# create library dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RUNTIME_DIR})

message("binary out dir: ${CMAKE_BINARY_DIR}")

################################################################
############################ model #############################
################################################################

set(MODELS_DIR "${CMAKE_CURRENT_LIST_DIR}/../models")
get_filename_component(MODELS_DIR ${MODELS_DIR} ABSOLUTE)

message("models dir: ${MODELS_DIR}")

if (${WAIFU2X_COPY_MODEL_FILE})
    if(NOT EXISTS "${MODELS_DIR}")
        message(FATAL_ERROR "The model folder does not exist!")
    endif()

    if ("${WAIFU2X_BUILD_TYPE}" STREQUAL "static_library")
        file(COPY ${MODELS_DIR} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    else()
        file(COPY ${MODELS_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    endif()
endif()

################################################################
############################# IPO ##############################
################################################################
# enable global link time optimization
cmake_policy(SET CMP0069 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_supported_output)
if(ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${ipo_supported_output}")
endif()

################################################################
########################### library ############################
################################################################

set(LIBRARY_DIR ${CMAKE_CURRENT_LIST_DIR}/library)
message("library dir: ${LIBRARY_DIR}")

find_package(Threads)
find_package(OpenMP)
find_package(Vulkan REQUIRED)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# NCNN
if(${USE_SYSTEM_NCNN})
    set(GLSLANG_TARGET_DIR "GLSLANG-NOTFOUND" CACHE PATH "Absolute path to glslangTargets.cmake directory")
    if(NOT GLSLANG_TARGET_DIR AND NOT DEFINED ENV{GLSLANG_TARGET_DIR})
        message(WARNING "GLSLANG_TARGET_DIR must be defined! USE_SYSTEM_NCNN will be turned off.")
        set(USE_SYSTEM_NCNN OFF)
    else()
        message(STATUS "Using glslang install located at ${GLSLANG_TARGET_DIR}")

        find_package(Threads)

        include("${GLSLANG_TARGET_DIR}/OSDependentTargets.cmake")
        include("${GLSLANG_TARGET_DIR}/OGLCompilerTargets.cmake")
        if(EXISTS "${GLSLANG_TARGET_DIR}/HLSLTargets.cmake")
            # hlsl support can be optional
            include("${GLSLANG_TARGET_DIR}/HLSLTargets.cmake")
        endif()
        include("${GLSLANG_TARGET_DIR}/glslangTargets.cmake")
        include("${GLSLANG_TARGET_DIR}/SPIRVTargets.cmake")

        if (NOT TARGET glslang OR NOT TARGET SPIRV)
            message(WARNING "glslang or SPIRV target not found! USE_SYSTEM_NCNN will be turned off.")
            set(USE_SYSTEM_NCNN OFF)
        endif()
    endif()

    find_package(ncnn)
    if(NOT TARGET ncnn)
        message(WARNING "ncnn target not found! USE_SYSTEM_NCNN will be turned off.")
        set(USE_SYSTEM_NCNN OFF)
    endif()
else()
    # build ncnn library
    if(NOT EXISTS "${LIBRARY_DIR}/ncnn/CMakeLists.txt")
        message(FATAL_ERROR "The submodules were not downloaded! Please update submodules with \"git submodule update --init --recursive\" and try again.")
    endif()

    option(NCNN_INSTALL_SDK "" OFF)
    option(NCNN_PIXEL_ROTATE "" OFF)
    option(NCNN_PIXEL_AFFINE "" OFF)
    option(NCNN_PIXEL_DRAWING "" OFF)
    option(NCNN_VULKAN "" ON)
    option(NCNN_VULKAN_ONLINE_SPIRV "" ON)
    option(NCNN_BUILD_BENCHMARK "" OFF)
    option(NCNN_BUILD_TESTS "" OFF)
    option(NCNN_BUILD_TOOLS "" OFF)
    option(NCNN_BUILD_EXAMPLES "" OFF)
    option(NCNN_DISABLE_RTTI "" ON)
    option(NCNN_DISABLE_EXCEPTION "" ON)
    option(NCNN_INT8 "" OFF)

    option(WITH_LAYER_absval "" OFF)
    option(WITH_LAYER_argmax "" OFF)
    option(WITH_LAYER_batchnorm "" OFF)
    option(WITH_LAYER_bias "" OFF)
    option(WITH_LAYER_bnll "" OFF)
    option(WITH_LAYER_concat "" OFF)
    option(WITH_LAYER_convolution "" ON)
    option(WITH_LAYER_crop "" ON)
    option(WITH_LAYER_deconvolution "" ON)
    option(WITH_LAYER_dropout "" OFF)
    option(WITH_LAYER_eltwise "" ON)
    option(WITH_LAYER_elu "" OFF)
    option(WITH_LAYER_embed "" OFF)
    option(WITH_LAYER_exp "" OFF)
    option(WITH_LAYER_flatten "" ON)
    option(WITH_LAYER_innerproduct "" ON)
    option(WITH_LAYER_input "" ON)
    option(WITH_LAYER_log "" OFF)
    option(WITH_LAYER_lrn "" OFF)
    option(WITH_LAYER_memorydata "" OFF)
    option(WITH_LAYER_mvn "" OFF)
    option(WITH_LAYER_pooling "" ON)
    option(WITH_LAYER_power "" OFF)
    option(WITH_LAYER_prelu "" OFF)
    option(WITH_LAYER_proposal "" OFF)
    option(WITH_LAYER_reduction "" OFF)
    option(WITH_LAYER_relu "" ON)
    option(WITH_LAYER_reshape "" OFF)
    option(WITH_LAYER_roipooling "" OFF)
    option(WITH_LAYER_scale "" ON)
    option(WITH_LAYER_sigmoid "" OFF)
    option(WITH_LAYER_slice "" OFF)
    option(WITH_LAYER_softmax "" OFF)
    option(WITH_LAYER_split "" ON)
    option(WITH_LAYER_spp "" OFF)
    option(WITH_LAYER_tanh "" OFF)
    option(WITH_LAYER_threshold "" OFF)
    option(WITH_LAYER_tile "" OFF)
    option(WITH_LAYER_rnn "" OFF)
    option(WITH_LAYER_lstm "" OFF)
    option(WITH_LAYER_binaryop "" OFF)
    option(WITH_LAYER_unaryop "" OFF)
    option(WITH_LAYER_convolutiondepthwise "" OFF)
    option(WITH_LAYER_padding "" ON)
    option(WITH_LAYER_squeeze "" OFF)
    option(WITH_LAYER_expanddims "" OFF)
    option(WITH_LAYER_normalize "" OFF)
    option(WITH_LAYER_permute "" OFF)
    option(WITH_LAYER_priorbox "" OFF)
    option(WITH_LAYER_detectionoutput "" OFF)
    option(WITH_LAYER_interp "" ON)
    option(WITH_LAYER_deconvolutiondepthwise "" OFF)
    option(WITH_LAYER_shufflechannel "" OFF)
    option(WITH_LAYER_instancenorm "" OFF)
    option(WITH_LAYER_clip "" OFF)
    option(WITH_LAYER_reorg "" OFF)
    option(WITH_LAYER_yolodetectionoutput "" OFF)
    option(WITH_LAYER_quantize "" OFF)
    option(WITH_LAYER_dequantize "" OFF)
    option(WITH_LAYER_yolov3detectionoutput "" OFF)
    option(WITH_LAYER_psroipooling "" OFF)
    option(WITH_LAYER_roialign "" OFF)
    option(WITH_LAYER_packing "" ON)
    option(WITH_LAYER_requantize "" OFF)
    option(WITH_LAYER_cast "" ON)
    option(WITH_LAYER_hardsigmoid "" OFF)
    option(WITH_LAYER_selu "" OFF)
    option(WITH_LAYER_hardswish "" OFF)
    option(WITH_LAYER_noop "" OFF)
    option(WITH_LAYER_pixelshuffle "" OFF)
    option(WITH_LAYER_deepcopy "" OFF)
    option(WITH_LAYER_mish "" OFF)
    option(WITH_LAYER_statisticspooling "" OFF)
    option(WITH_LAYER_swish "" OFF)
    option(WITH_LAYER_gemm "" OFF)
    option(WITH_LAYER_groupnorm "" OFF)
    option(WITH_LAYER_layernorm "" OFF)
    option(WITH_LAYER_softplus "" OFF)
    option(WITH_LAYER_gru "" OFF)
    option(WITH_LAYER_multiheadattention "" OFF)
    option(WITH_LAYER_gelu "" OFF)

    add_subdirectory(${LIBRARY_DIR}/ncnn)
endif()

# STB
if(NOT EXISTS "${LIBRARY_DIR}/stb/stb_image.h")
    message(FATAL_ERROR "The submodules were not downloaded! Please update submodules with \"git submodule update --init\" and try again.")
endif()
include_directories(${LIBRARY_DIR}/stb)

################################################################
########################### shader #############################
################################################################

set(SHADER_DIR ${CMAKE_CURRENT_LIST_DIR}/shader)
message("shader dir: ${SHADER_DIR}")

macro(waifu2x_add_shader SHADER_SRC)
    get_filename_component(SHADER_SRC_NAME_WE ${SHADER_SRC} NAME_WE)
    set(SHADER_COMP_HEADER ${SHADER_DIR}/${SHADER_SRC_NAME_WE}.comp.hex.h)

    add_custom_command(
        OUTPUT ${SHADER_COMP_HEADER}
        COMMAND ${CMAKE_COMMAND} -DSHADER_SRC=${SHADER_DIR}/${SHADER_SRC} -DSHADER_COMP_HEADER=${SHADER_COMP_HEADER} -P "${SHADER_DIR}/generate_shader_comp_header.cmake"
        DEPENDS ${SHADER_DIR}/${SHADER_SRC}
        COMMENT "Preprocessing shader source ${SHADER_SRC_NAME_WE}.comp"
        VERBATIM
    )
    set_source_files_properties(${SHADER_COMP_HEADER} PROPERTIES GENERATED TRUE)

    list(APPEND SHADER_SPV_HEX_FILES ${SHADER_COMP_HEADER})
endmacro()

waifu2x_add_shader(waifu2x_preproc.comp)
waifu2x_add_shader(waifu2x_postproc.comp)
waifu2x_add_shader(waifu2x_preproc_tta.comp)
waifu2x_add_shader(waifu2x_postproc_tta.comp)

add_custom_target(generate-spirv DEPENDS ${SHADER_SPV_HEX_FILES})

################################################################
############################ build #############################
################################################################

file(GLOB SOURCE_CODES "${CMAKE_CURRENT_LIST_DIR}/*.h" "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

if ("${WAIFU2X_BUILD_TYPE}" STREQUAL "static_library")
    add_library(libwaifu2x STATIC ${SOURCE_CODES})
    message("output type: static library")
elseif("${WAIFU2X_BUILD_TYPE}" STREQUAL "dynamic_library")
    add_library(libwaifu2x SHARED ${SOURCE_CODES})
    message("output type: dynamic library")
else()
    add_executable(libwaifu2x ${SOURCE_CODES})
    message("output type: executable")
endif()

message("include header dir: ${INCLUDE_DIR}")
file(GLOB INCLUDE_HEADER_CODES "${CMAKE_CURRENT_LIST_DIR}/*.h")
file(COPY ${INCLUDE_HEADER_CODES} DESTINATION ${INCLUDE_DIR})

################################################################
############################# link #############################
################################################################

if(USE_STATIC_MOLTENVK)
    find_library(CoreFoundation NAMES CoreFoundation)
    find_library(Foundation NAMES Foundation)
    find_library(Metal NAMES Metal)
    find_library(QuartzCore NAMES QuartzCore)
    find_library(CoreGraphics NAMES CoreGraphics)
    find_library(Cocoa NAMES Cocoa)
    find_library(IOKit NAMES IOKit)
    find_library(IOSurface NAMES IOSurface)

    list(APPEND WAIFU2X_LINK_LIBRARIES
        ${Metal}
        ${QuartzCore}
        ${CoreGraphics}
        ${Cocoa}
        ${IOKit}
        ${IOSurface}
        ${Foundation}
        ${CoreFoundation}
    )
endif()
# shader
add_dependencies(libwaifu2x generate-spirv)
# ncnn
set(WAIFU2X_LINK_LIBRARIES ncnn ${Vulkan_LIBRARY})

if(USE_STATIC_MOLTENVK)
    find_library(CoreFoundation NAMES CoreFoundation)
    find_library(Foundation NAMES Foundation)
    find_library(Metal NAMES Metal)
    find_library(QuartzCore NAMES QuartzCore)
    find_library(CoreGraphics NAMES CoreGraphics)
    find_library(Cocoa NAMES Cocoa)
    find_library(IOKit NAMES IOKit)
    find_library(IOSurface NAMES IOSurface)

    list(APPEND WAIFU2X_LINK_LIBRARIES
        ${Metal}
        ${QuartzCore}
        ${CoreGraphics}
        ${Cocoa}
        ${IOKit}
        ${IOSurface}
        ${Foundation}
        ${CoreFoundation}
    )
endif()

target_link_libraries(libwaifu2x PUBLIC ${WAIFU2X_LINK_LIBRARIES})
target_include_directories(libwaifu2x PUBLIC ${CMAKE_CURRENT_LIST_DIR})

################################################################
######################### parent scope #########################
################################################################

set(WAIFU2X_LINK_LIBRARIES ${WAIFU2X_LINK_LIBRARIES} PARENT_SCOPE)
set(WAIFU2X_MODEL_DIR ${MODELS_DIR} PARENT_SCOPE)