cmake_minimum_required(VERSION 3.20)
project(tImage CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自動リンク無効化と静的リンク用マクロ設定
add_compile_definitions(PNG_STATIC PNG_NO_DLL)

# ------------------------------------------------------------------------------
# 1. zlib (thirdparty) の設定
# ------------------------------------------------------------------------------
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)

# zlib をサブプロジェクトとして追加
add_subdirectory(thirdparty/zlib)

# ------------------------------------------------------------------------------
# 2. libpng (thirdparty) の設定
# ------------------------------------------------------------------------------
set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)
set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib" CACHE PATH "" FORCE)
set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib;${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zlib" CACHE PATH "" FORCE)

set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
set(ZLIB_LIBRARIES zlibstatic CACHE STRING "" FORCE)

# libpng のビルドオプション
set(PNG_SHARED OFF CACHE BOOL "Build shared libpng" FORCE)
set(PNG_TESTS OFF CACHE BOOL "Build libpng tests" FORCE)
set(PNG_TOOLS OFF CACHE BOOL "Build libpng tools" FORCE)

# libpng をサブプロジェクトとして追加
add_subdirectory(thirdparty/libpng)

# ------------------------------------------------------------------------------
# 3. tImage ライブラリターゲットの作成
# ------------------------------------------------------------------------------
set(TIMAGE_SOURCES
    src/effect/gaussianBlur.cpp
    src/image/Image.cpp
    src/manage/manage.cpp
    src/tool/converter.cpp
    src/tool/merge.cpp
    src/tool/split.cpp
)

add_library(tImage ${TIMAGE_SOURCES})

# インクルードパスの設定
target_include_directories(tImage PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib
    ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zlib
    ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/libpng
)

# tImage に png_static と zlibstatic をリンク
target_link_libraries(tImage PRIVATE
    png_static
    zlibstatic
)

# ------------------------------------------------------------------------------
# 4. デバッグ用実行ファイル (tImage_debug)
# ------------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debug/main.cpp")
        add_executable(tImage_debug debug/main.cpp)
        
        target_link_libraries(tImage_debug PRIVATE tImage)

        # ★【追加】ビルド成功後、zd.dll を exe と同じディレクトリにコピーする
        add_custom_command(TARGET tImage_debug POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:zlib>
            $<TARGET_FILE_DIR:tImage_debug>
        )
    endif()
endif()

# リンカーに libpng16d.lib の自動リンク要求を無視させる指示
if(MSVC)
    target_link_options(tImage_debug PRIVATE "/NODEFAULTLIB:libpng16d.lib")
endif()