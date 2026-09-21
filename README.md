# tImage: 汎用・高速画像処理ライブラリ

あらゆる環境で高速に動作する画像処理フレームワークを提供する．
メモリの確保および解放は，基本的に利用環境に任せる設計としている．

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

## WebAssembly (Emscripten) でビルドする方法

Emscripten SDK をインストールし，Emscripten の環境を有効化したシェルで実行する．Windows PowerShell では，例えば次のようにする．

```powershell
# emsdk のパスは環境に合わせて変更する
cd C:\path\to\emsdk
.\emsdk_env.ps1
cd C:\path\to\tImage

cmake --preset wasm-release
cmake --build --preset wasm-release --target tImage
```

生成物は `build/wasm-release/libtImage.a` である．このプリセットでは，ブラウザーまたはNode.jsからリンクできる静的ライブラリとして生成するため，`BUILD_SHARED_LIBS=OFF` と `TIMAGE_ENABLE_OPENMP=OFF` を指定している．JavaScript APIやWasmモジュールまで生成する場合は，このライブラリを利用する側のEmscriptenターゲットで `emcc` または `em++` にリンクし，`-sMODULARIZE=1` などの実行環境向けオプションを指定する．

Emscripten の環境が有効かどうかは，`emcc --version` で確認できる．`emcc` が見つからない場合は，先に emsdk をインストールして `emsdk_env.ps1` を実行する．

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

静的ビルドでは，MSVC用の `x64-Debug-Static`／`x64-Release-Static` 構成により，libpng と zlib のオブジェクトを `tImage.lib` に取り込む．そのため利用側は `tImage.lib` だけをリンクすればよく，`z.dll`，`libpng16_static.lib`，`zs.lib` を配置・指定する必要はない．Visual Studio コンソールアプリケーションで利用する例は `debug/static` にあるので適宜参照されたい．

Visual Studio のフォルダーを開く構成以外で同じ動作にする場合は，次のオプションを指定する．

```text
-DBUILD_SHARED_LIBS=OFF -DTIMAGE_BUNDLE_STATIC_DEPENDENCIES=ON
```

この機能は MSVC の静的ビルドで有効である．CMake の `add_subdirectory` で tImage を組み込む場合は，通常どおり `target_link_libraries(app PRIVATE tImage)` と指定できる．

## 使用ライブラリ

- zlib
- libpng
- libjpeg
