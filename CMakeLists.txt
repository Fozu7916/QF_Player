cmake_minimum_required(VERSION 3.16)

project(TextRedactor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Указываем пути к mpv (важно использовать абсолютные пути)
set(MPV_DIR "${CMAKE_SOURCE_DIR}/mpv")
set(MPV_INCLUDE_DIR "${MPV_DIR}/include")
set(MPV_LIB_DIR "${MPV_DIR}")

# Проверка существования файлов mpv
if(NOT EXISTS "${MPV_DIR}/libmpv-2.dll")
    message(FATAL_ERROR "libmpv-2.dll not found in ${MPV_DIR}")
endif()

if(NOT EXISTS "${MPV_DIR}/libmpv.dll.a")
    message(FATAL_ERROR "libmpv.dll.a not found in ${MPV_DIR}")
endif()

# Добавляем библиотеку mpv
add_library(mpv SHARED IMPORTED)
set_target_properties(mpv PROPERTIES
    IMPORTED_IMPLIB "${MPV_DIR}/libmpv.dll.a"
    IMPORTED_LOCATION "${MPV_DIR}/libmpv-2.dll"
)
target_include_directories(mpv INTERFACE "${MPV_INCLUDE_DIR}")

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TextRedactor
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/player.h src/player.cpp
        src/track.h src/track.cpp

    )
else()
    if(ANDROID)
        add_library(TextRedactor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(TextRedactor
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Линкуем как Qt, так и mpv
target_link_libraries(TextRedactor PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    mpv
)

# Копирование DLL в папку сборки (Windows)
if(WIN32)
    add_custom_command(TARGET TextRedactor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${MPV_DIR}/libmpv-2.dll"
            "$<TARGET_FILE_DIR:TextRedactor>"
        COMMENT "Copying libmpv-2.dll to output directory"
    )
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TextRedactor)
endif()

set_target_properties(TextRedactor PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TextRedactor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TextRedactor)
endif()
