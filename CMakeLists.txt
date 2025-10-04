cmake_minimum_required(VERSION 3.16)

project(QF_Player VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia Concurrent)
if(UNIX AND NOT APPLE)
  find_package(Qt${QT_VERSION_MAJOR} COMPONENTS DBus)
endif()

# Указываем пути к mpv
set(MPV_DIR "${CMAKE_SOURCE_DIR}/mpv")
set(MPV_INCLUDE_DIR "${MPV_DIR}/include")
set(MPV_LIB_DIR "${MPV_DIR}/lib")

if(WIN32)
    # Проверка файлов Windows
    if(NOT EXISTS "${MPV_DIR}/libmpv-2.dll")
        message(FATAL_ERROR "libmpv-2.dll not found in ${MPV_DIR}")
    endif()
    if(NOT EXISTS "${MPV_DIR}/libmpv.dll.a")
        message(FATAL_ERROR "libmpv.dll.a not found in ${MPV_DIR}")
    endif()

    add_library(mpv SHARED IMPORTED)
    set_target_properties(mpv PROPERTIES
        IMPORTED_IMPLIB "${MPV_DIR}/libmpv.dll.a"
        IMPORTED_LOCATION "${MPV_DIR}/libmpv-2.dll"
    )
else()
    # Linux / Unix
    set(MPV_SO "")
    file(GLOB MPV_SO_FILES "${MPV_LIB_DIR}/libmpv.so*")
    list(GET MPV_SO_FILES 0 MPV_SO)
    if(NOT MPV_SO)
        message(FATAL_ERROR "libmpv.so not found in ${MPV_LIB_DIR}")
    endif()

    add_library(mpv SHARED IMPORTED)
    set_target_properties(mpv PROPERTIES
        IMPORTED_LOCATION "${MPV_SO}"
        INTERFACE_INCLUDE_DIRECTORIES "${MPV_INCLUDE_DIR}"
    )
endif()

# Добавляем исходники
set(PROJECT_SOURCES
    src/main.cpp
    src/view/mainwindow.cpp
    src/view/mainwindow.h
    src/view/mainwindow.ui
    src/controller/durationcontroller.h
    src/controller/durationcontroller.cpp
    src/integration/mediaosd.h
    src/integration/mediaosd.cpp
    src/integration/linuxmediakeys.h
    src/integration/linuxmediakeys.cpp
    src/controller/playercontroller.h
    src/controller/playercontroller.cpp
    resources/resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QF_Player
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/player/player.h src/player/player.cpp
        src/model/track.h src/model/track.cpp
    )
else()
    if(ANDROID)
        add_library(QF_Player SHARED ${PROJECT_SOURCES})
    else()
        add_executable(QF_Player ${PROJECT_SOURCES})
    endif()
endif()

# Линкуем Qt и mpv
target_link_libraries(QF_Player PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::Concurrent
    mpv
)
if(UNIX AND NOT APPLE AND TARGET Qt${QT_VERSION_MAJOR}::DBus)
  target_link_libraries(QF_Player PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
  target_compile_definitions(QF_Player PRIVATE HAVE_MPRIS=1)
endif()

# Linux X11 для медиа-клавиш
if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)
  target_link_libraries(QF_Player PRIVATE X11)
  target_compile_definitions(QF_Player PRIVATE HAVE_X11=1)
endif()

# Копирование DLL в папку сборки (Windows)
if(WIN32)
    add_custom_command(TARGET QF_Player POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            "${MPV_DIR}/libmpv-2.dll"
            "$<TARGET_FILE_DIR:QF_Player>"
        COMMENT "Copying libmpv-2.dll to output directory"
    )
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QF_Player)
endif()

set_target_properties(QF_Player PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QF_Player
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QF_Player)
endif()

add_subdirectory(tests)