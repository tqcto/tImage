cmake_minimum_required(VERSION 3.20)
project(tImage CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 動的ライブラリ(DLL)か静的ライブラリ(.lib)かを切り替えるオプション
# ON: DLL、OFF: 静的ライブラリ(.lib)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(TIMAGE_ENABLE_OPENMP "Enable OpenMP acceleration" ON)
set(TIMAGE_BUNDLE_STATIC_DEFAULT OFF)
if(NOT BUILD_SHARED_LIBS AND MSVC)
    set(TIMAGE_BUNDLE_STATIC_DEFAULT ON)
endif()
option(TIMAGE_BUNDLE_STATIC_DEPENDENCIES
       "Bundle libpng and zlib into the static tImage library"
       ${TIMAGE_BUNDLE_STATIC_DEFAULT})

# Linuxなどで静的なlibpng/zlibを共有ライブラリへ組み込むために、
# 依存ライブラリも位置独立コードとしてコンパイルする。
if(BUILD_SHARED_LIBS AND NOT MSVC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(TIMAGE_ENABLE_OPENMP AND NOT EMSCRIPTEN)
    find_package(OpenMP QUIET)
endif()

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
    src/image/MatrixConverter.cpp
    src/manage/manage.cpp
    src/tool/converter.cpp
    src/tool/merge.cpp
    src/tool/split.cpp
    src/tool/transpose.cpp
    src/tool/padding.cpp
    src/tool/fft.cpp
)

add_library(tImage ${TIMAGE_SOURCES})

# インクルードパスの設定 (PUBLICにすることで依存元からもヘッダーを参照可能に設定)
target_include_directories(tImage 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/libpng
            ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/zlib
            ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/zlib
            ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/libpng
)

# DLLビルド時用のマクロ定義
if(BUILD_SHARED_LIBS)
    target_compile_definitions(tImage PRIVATE TIMAGE_BUILD_DLL)
endif()

if(MSVC AND TIMAGE_ENABLE_OPENMP)
    target_compile_options(tImage PRIVATE /openmp:experimental)
endif()

if(OpenMP_CXX_FOUND AND NOT MSVC)
    target_link_libraries(tImage PRIVATE OpenMP::OpenMP_CXX)
endif()

# 通常は依存ライブラリをtImageのリンク要件として伝播する。
# MSVCの統合静的ビルドでは、依存オブジェクトをtImage.libへ取り込むため、
# 利用側へ同じライブラリを再度渡さない。
if(NOT BUILD_SHARED_LIBS AND MSVC AND TIMAGE_BUNDLE_STATIC_DEPENDENCIES)
    add_dependencies(tImage png_static zlibstatic)
else()
    target_link_libraries(tImage PRIVATE
        png_static
        zlibstatic
    )
endif()

# MSVCの静的ライブラリは、依存する静的ライブラリをtImage.libへ取り込む
# ことで、利用側がtImage.libだけを指定できるようにする。
if(NOT BUILD_SHARED_LIBS AND MSVC AND TIMAGE_BUNDLE_STATIC_DEPENDENCIES)
    add_custom_command(TARGET tImage POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:tImage>.merged.lib"
        COMMAND "${CMAKE_AR}"
                "/OUT:$<TARGET_FILE:tImage>.merged.lib"
                "$<TARGET_FILE:tImage>"
                "$<TARGET_FILE:png_static>"
                "$<TARGET_FILE:zlibstatic>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "$<TARGET_FILE:tImage>.merged.lib"
                "$<TARGET_FILE:tImage>"
        COMMAND ${CMAKE_COMMAND} -E rm -f "$<TARGET_FILE:tImage>.merged.lib"
        VERBATIM
    )
endif()

# MSVCの場合、libpngヘッダーが要求するデフォルトのライブラリ自動リンク指定を無効化
if(MSVC)
    target_link_options(tImage PRIVATE "/NODEFAULTLIB:libpng16d.lib" "/NODEFAULTLIB:libpng16.lib")
endif()

# ------------------------------------------------------------------------------
# 4. デバッグ用実行ファイル (tImage_debug)
# ------------------------------------------------------------------------------
if(NOT EMSCRIPTEN)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debug/main.cpp")
        add_executable(tImage_debug debug/main.cpp)
        
        target_link_libraries(tImage_debug PRIVATE tImage)

        # デバッグexe側でDLLのインポートマクロを有効化
        if(BUILD_SHARED_LIBS)
            target_compile_definitions(tImage_debug PRIVATE TIMAGE_USE_DLL)
        endif()

        # デバッグexe側の自動リンク無視設定
        if(MSVC)
            target_link_options(tImage_debug PRIVATE "/NODEFAULTLIB:libpng16d.lib" "/NODEFAULTLIB:libpng16.lib")
        endif()

        # DLLとしてビルドされている場合、ビルド成功後に tImage.dll を exe の出力先にコピー
        if(BUILD_SHARED_LIBS)
            add_custom_command(TARGET tImage_debug POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:tImage>
                $<TARGET_FILE_DIR:tImage_debug>
            )
        endif()

        # DLLビルド時だけ、zlib DLLが存在していればコピー
        if(BUILD_SHARED_LIBS AND TARGET zlib)
            add_custom_command(TARGET tImage_debug POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:zlib>
                $<TARGET_FILE_DIR:tImage_debug>
            )
        endif()
    endif()
endif()