# コンパイル方法

## CMake プリセットを使う方法

PowerShell または Developer Command Prompt でリポジトリのルートをカレントディレクトリにして実行する．

```powershell
# 動的ライブラリ
cmake --preset x64-debug
cmake --build --preset x64-debug

# 静的ライブラリ
cmake --preset x64-debug-static
cmake --build --preset x64-debug-static
```

静的ビルドでは，MSVC なら `build/x64-debug-static/tImage.lib`，MinGW なら `build/x64-debug-static/libtImage.a` が生成される．DLL ビルドでは `build/x64-debug/tImage.dll` とインポートライブラリが生成される．

Release ビルドは `x64-release` または `x64-release-static` に置き換える．

## コマンドラインで直接指定する方法

プリセットを使わず，出力先とライブラリ形式を明示することもできる．

```powershell
# 動的ライブラリ
cmake -S . -B build/dll -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
cmake --build build/dll

# 静的ライブラリ
cmake -S . -B build/static -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF
cmake --build build/static
```

既存の構成を変更する場合は，`cmake -S . -B <ビルドディレクトリ> -DBUILD_SHARED_LIBS=ON|OFF` を再実行する．

## Visual Studio でビルドする方法

Visual Studio で **ファイル > 開く > フォルダー** を選択し，`tImage` のルートフォルダーを開く．次に，構成一覧から `x64-Debug-Static` または `x64-Release-Static` を選択する．**CMake > ビルド > すべてビルド** を実行．

これらの構成は `BUILD_SHARED_LIBS=OFF` を指定しているため，静的ライブラリが生成される．生成物はそれぞれ `build/x64-Debug-Static/tImage.lib` または `build/x64-Release-Static/tImage.lib` ．

Visual Studio のターミナルから実行する場合は，以下の Visual Studio 2022 を用いるようなコマンドでも同じ構成を作成できる．

```powershell
# Visual Studio 2022 のソリューション生成
cmake -S . -B build/vs2022-static -G "Visual Studio 17 2022" -A x64 `
  -DBUILD_SHARED_LIBS=OFF

# Release の tImage.lib をビルド
cmake --build build/vs2022-static --config Release --target tImage
```

生成物は `build/vs2022-static/Release/tImage.lib` ．Debug 版が必要な場合は，最後の `Release` を `Debug` に置き換える．

DLL をビルドする場合は，構成時の `-DBUILD_SHARED_LIBS=OFF` を `-DBUILD_SHARED_LIBS=ON` に変更する．

静的ビルドでは `z.dll` を実行時に配置する必要はない．ただし，利用側のリンク時には `tImage.lib` に加えて，CMake の `tImage` ターゲットが参照する `libpng16_static.lib` と `zs.lib` も必要．CMake の `add_subdirectory` で tImage を組み込む場合は，`target_link_libraries(app PRIVATE tImage)` とすることで依存関係を CMake に解決させられる．
