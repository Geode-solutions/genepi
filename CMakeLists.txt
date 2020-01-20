# Copyright (c) 2019 - 2020 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.5)

project(genepi)

set(UTILS_FILE "${PROJECT_SOURCE_DIR}/cmake/utils.cmake")
file(READ "${UTILS_FILE}" GENEPI_UTILS)
include("${UTILS_FILE}")

set(genepi_include_dir "${PROJECT_SOURCE_DIR}/include/genepi")
set(genepi_source_dir "${PROJECT_SOURCE_DIR}/src/genepi")
add_library(genepi "${genepi_source_dir}/common.cpp")
add_library(genepi::genepi ALIAS genepi)
set_target_properties(genepi PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_sources(genepi
    PRIVATE
        "${genepi_include_dir}/arg_from_napi_value.h"
        "${genepi_include_dir}/bind_class.h"
        "${genepi_include_dir}/bind_class_base.h"
        "${genepi_include_dir}/binding_std.h"
        "${genepi_include_dir}/binding_type.h"
        "${genepi_include_dir}/caller.h"
        "${genepi_include_dir}/checker.h"
        "${genepi_include_dir}/class_definer.h"
        "${genepi_include_dir}/class_wrapper.h"
        "${genepi_include_dir}/common.h"
        "${genepi_include_dir}/creator.h"
        "${genepi_include_dir}/function_definer.h"
        "${genepi_include_dir}/function_definition.h"
        "${genepi_include_dir}/genepi.h"
        "${genepi_include_dir}/method_definition.h"
        "${genepi_include_dir}/signature/base_signature.h"
        "${genepi_include_dir}/signature/constructor_signature.h"
        "${genepi_include_dir}/signature/function_signature.h"
        "${genepi_include_dir}/signature/method_signature.h"
        "${genepi_include_dir}/signature/signature_param.h"
        "${genepi_include_dir}/signature/templated_base_signature.h"
        "${genepi_include_dir}/singleton.h"
        "${genepi_include_dir}/type_list.h"
        "${genepi_include_dir}/type_transformer.h"
        "${genepi_source_dir}/singleton.cpp"
)
target_include_directories(genepi
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}
    PRIVATE
        ${NODE_ADDON_API_DIR}
        ${CMAKE_JS_INC}
)

target_link_libraries(genepi PUBLIC ${CMAKE_JS_LIB})
export(TARGETS genepi NAMESPACE genepi:: FILE genepi_target.cmake)
include(GenerateExportHeader)
generate_export_header(genepi
    EXPORT_MACRO_NAME genepi_api
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/genepi/genepi_export.h
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/genepiConfig.cmake.in
    genepiConfig.cmake
    INSTALL_DESTINATION .
)

if(EXISTS ${PROJECT_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif()
