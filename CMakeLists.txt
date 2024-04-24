cmake_minimum_required(VERSION 3.20)

project(example VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/.cmake/)
include(GetGitRevisionDescription)

if(MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
endif()

if(APPLE)
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}.app/Contents/MacOS)
else()
    set(APPLICATION_DIR_PATH ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick Svg Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Quick Svg Network)

find_program(QT_LUPDATE NAMES lupdate)
find_program(QT_LRELEASE NAMES lrelease)
file(GLOB TS_FILE_PATHS ${CMAKE_SOURCE_DIR}/ *.ts)
add_custom_target(script-update_translations
    COMMAND ${QT_LUPDATE} ${CMAKE_SOURCE_DIR} -ts ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${QT_LUPDATE} ${CMAKE_SOURCE_DIR} -ts ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_en_US.ts WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${QT_LRELEASE} ${PROJECT_NAME}_zh_CN.ts WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${APPLICATION_DIR_PATH}/i18n
    COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_NAME}_en_US.qm ${PROJECT_NAME}_zh_CN.qm ${APPLICATION_DIR_PATH}/i18n
    SOURCES ${TS_FILE_PATHS}
)

# set(HEADER_FILE_VERSION_PATH ${CMAKE_SOURCE_DIR}/Version.h)
configure_file(
    ${CMAKE_SOURCE_DIR}/.cmake/Version.h.in
    ${CMAKE_SOURCE_DIR}/Version.h
)

file(GLOB_RECURSE CPP_FILES *.cpp *.h)
foreach(filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach(filepath)

if(WIN32)
    list(APPEND sources_files "src/fluentUI/app_dmp.h")
endif()

set(VERSION_RC_PATH "")
if(WIN32)
    set(VERSION_RC_PATH ${CMAKE_BINARY_DIR}/version.rc)
    configure_file(
        ${CMAKE_SOURCE_DIR}/.cmake/version.rc.in
        ${VERSION_RC_PATH}
    )
configure_file(
    ${CMAKE_SOURCE_DIR}/.cmake/InstallerScript.iss.in
    ${CMAKE_SOURCE_DIR}/dist/InstallerScript.iss
)
endif()

# qt_add_big_resources(QRC_RESOURCES res.qrc)
qt_add_resources(QRC_RESOURCES res.qrc)
list(APPEND QRC_RESOURCES res.qrc)
set_property(SOURCE res.qrc PROPERTY SKIP_AUTORCC ON)
list(APPEND sources_files ${QRC_RESOURCES})

if(WIN32)
    list(APPEND sources_files ${VERSION_RC_PATH})
endif()
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${sources_files}
    )
else()
    add_executable(${PROJECT_NAME}
        ${sources_files}
    )
endif()
add_dependencies(${PROJECT_NAME} script-update_translations)

if(WIN32)
    if(MSVC)
        if(CMAKE_SIZEOF_VOID_P EQUAL 4)
            file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/msvc/x86/*.dll)
        elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
            file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/msvc/x64/*.dll)
        endif()
    elseif(MINGW)
        file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/mingw/*.dll)
    endif()
    file(COPY ${3RDPARTY_DLL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/source/)
# file(COPY ${CMAKE_SOURCE_DIR}/src/ DESTINATION ${APPLICATION_DIR_PATH}/source/)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src/fluentUI/componet
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.${PROJECT_NAME}.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_MESSAGELOGCONTEXT
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Network
)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    if (APPLE)
        find_program(QT_DEPLOY_QT NAMES macdeployqt)
        add_custom_target(script-deploy_release
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${QT_DEPLOY_QT} ${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}.app -qmldir=${CMAKE_SOURCE_DIR}
            COMMENT "Deploying Qt Dependencies...................."
            SOURCES ${CMAKE_SOURCE_DIR}/CMakeLists.txt
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif()
    if(WIN32)
        find_program(QT_DEPLOY_QT NAMES windeployqt)
        add_custom_target(script-deploy_release
            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ${CMAKE_SOURCE_DIR}/dist
            COMMAND ${QT_DEPLOY_QT} ${CMAKE_SOURCE_DIR}/dist/${PROJECT_NAME}.exe -qmldir=${CMAKE_SOURCE_DIR}
            COMMENT "Deploying Qt Dependencies...................."
            SOURCES ${CMAKE_SOURCE_DIR}/CMakeLists.txt
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
endif()
endif()