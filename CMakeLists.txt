cmake_minimum_required(VERSION 3.11)
project(qt_opengl_wearglasses VERSION 0.1 LANGUAGES CXX)
set(texture_dir ${CMAKE_BINARY_DIR}/texture)
file(COPY ${CMAKE_SOURCE_DIR}/shape_predictor_68_face_landmarks.dat DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shader.vs DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shader.fs DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/obj_use DESTINATION ${CMAKE_BINARY_DIR})
# execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${texture_dir})
file(COPY ${CMAKE_SOURCE_DIR}/texture/out.obj DESTINATION ${CMAKE_BINARY_DIR}/texture)
file(COPY ${CMAKE_SOURCE_DIR}/texture/out.texture.png DESTINATION ${CMAKE_BINARY_DIR}/texture)
file(COPY ${CMAKE_SOURCE_DIR}/texture/out.mtl DESTINATION ${CMAKE_BINARY_DIR}/texture)
file(COPY ${CMAKE_SOURCE_DIR}/texture DESTINATION ${CMAKE_BINARY_DIR})


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core Gui  Multimedia  OpenGL OpenGLWidgets MultimediaWidgets)
find_package(OpenCV REQUIRED)
find_package(ASSIMP REQUIRED)
# find_package(eos REQUIRED)

include_directories(${EOS_INCLUDE_DIRS})
# include_directories("/usr/local/3rdparty")



include(/home/guoyi/Desktop/shejigame/dlib/dlib/cmake)
include_directories(${Assimp_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(./)
set(eos_3RDPARTY_DIR "/usr/local/3rdparty")
set(CEREAL_INCLUDE_DIR "${eos_3RDPARTY_DIR}/cereal/include")
set(glm_INCLUDE_DIR "${eos_3RDPARTY_DIR}/glm")
set(nanoflann_INCLUDE_DIR "${eos_3RDPARTY_DIR}/nanoflann/include")
set(eigen3_nnls_INCLUDE_DIR "${eos_3RDPARTY_DIR}/eigen3-nnls/src")
set(toml11_INCLUDE_DIR "${eos_3RDPARTY_DIR}/toml11")

add_library(fit_model fit_model.h)
target_link_libraries(fit_model ${OpenCV_LIBS} ${Boost_LIBRARIES} ${EOS_LIBRARIES})
target_link_libraries(fit_model "$<$<CXX_COMPILER_ID:GNU>:-pthread>$<$<CXX_COMPILER_ID:Clang>:-pthreads>")
target_include_directories(fit_model PUBLIC ${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}  ${EOS_INCLUDE_DIRS})
target_include_directories(fit_model PUBLIC  ${CEREAL_INCLUDE_DIR} ${glm_INCLUDE_DIR} ${nanoflann_INCLUDE_DIR} ${eigen3_nnls_INCLUDE_DIR} ${toml11_INCLUDE_DIR} )
set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        points_68.h
        mainwindow.ui
        MainGlwight.h
        Shader.h
        Camera.h
        # fit_model.h
        Model.h
        stb_image.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt_opengl_wearglasses
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET wearglasses APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qt_opengl_wearglasses SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qt_opengl_wearglasses
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(qt_opengl_wearglasses PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
    ${OpenCV_LIBS}
    dlib::dlib
    assimp
    fit_model
)

set_target_properties(qt_opengl_wearglasses PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS qt_opengl_wearglasses
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qt_opengl_wearglasses)
endif()
# set(MY_VAR "${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_SOURCE_DIR is ${CMAKE_SOURCE_DIR}")





