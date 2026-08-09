# OpenGLLearn

OpenGLを学びながら、リアルタイムレンダリングの仕組みを一つずつ実装している個人プロジェクトです。

単なるチュートリアルコードではなく、OpenGLオブジェクトのRAII化、ステージ管理、プロシージャルメッシュ、MRT、ポストプロセスなどを再利用できる形にまとめつつ、小さなレンダリング基盤へ育てています。

> [!NOTE]
> 現在も学習・開発中です。APIやディレクトリ構成は変更される可能性があります。

## 主な機能

- OpenGL 4.6 Core Profile
- シェーダー、テクスチャ、メッシュ、フレームバッファなどのRAIIラッパー
- 3D・2Dプロシージャルメッシュ生成
- インスタンシング描画
- Compute Shader
- Multi Render Target（MRT）
- ポストプロセスチェーン
- sRGBを考慮した描画
- 2D・3Dカメラ
- ステージの動的切り替え
- Dear ImGuiによるデバッグUI
- FPS制御とVSync

## デモステージ

| キー | ステージ | 内容 |
| --- | --- | --- |
| `F1` | Earth | 地球テクスチャ、法線・スペキュラ・夜景マップ、Compute Shader、1,000個のインスタンシング、MRT出力の可視化 |
| `F2` | Black Hole | フルスクリーンのレイマーチングによるブラックホールとHDR背景 |
| `F3` | Test 2D | プロシージャル生成した円を10,000個インスタンシング描画 |

EarthステージではImGuiから最終色、法線、各種テクスチャ、深度、UVなどの出力を切り替えられます。

## 操作

### 共通

- `F1` / `F2` / `F3`: ステージ切り替え

### Earth

- `W` / `S`: 前後移動
- `A` / `D`: 左右移動
- `R` / `F`: 上下移動
- 矢印キー: カメラ回転
- ImGui: 移動・回転速度、ブラー、MRT表示の変更

### Test 2D

- 矢印キー: カメラ移動
- ImGui: スケール、角度、移動速度の変更

## 構成

```text
OpenGLLearn/
├─ Apps/                 アプリケーション実装
├─ Core/                 ウィンドウ、入力、カメラ、ステージ、FPS制御
├─ Graphics/
│  ├─ OpenGL/            OpenGLリソースのRAIIラッパー
│  └─ UI/                ImGui統合
├─ Stages/               各デモステージ
├─ Utils/                ファイル・パス関連の補助機能
├─ External/             プロジェクトに含めた外部ソース
└─ assets/
   ├─ shaders/           GLSLシェーダー
   └─ data/texture/      実行時に使用するテクスチャ（Git管理外）
```

## 開発環境

- Windows 11
- Visual Studio 2026 / MSVC v145
- C++20
- x64

使用ライブラリ:

- GLFW 3.4
- GLAD
- GLM 1.0.3
- Dear ImGui 1.92.9
- stb_image
- spdlog 1.17.0

## ビルド方法

1. リポジトリをクローンします。
2. 上記の依存ライブラリを用意します。
3. `OpenGLLearn/OpenGLLearn.vcxproj` の追加インクルードディレクトリと追加ライブラリディレクトリを、自分の環境に合わせて変更します。
4. `OpenGLLearn.slnx` をVisual Studioで開きます。
5. 構成を `Debug | x64` または `Release | x64` にしてビルドします。
6. 作業ディレクトリが `OpenGLLearn/` になる状態で実行します。

現在のプロジェクト設定には `C:\prog\...` を起点とするローカル絶対パスが含まれています。別の環境では、そのままでは依存ライブラリを見つけられないため設定変更が必要です。

## アセットについて

GLSLシェーダーはリポジトリに含まれていますが、容量の大きいテクスチャやHDR画像は含まれていません。各ファイルを次の場所へ配置してください。

```text
OpenGLLearn/assets/data/texture/
├─ 8k_earth_daymap.jpg
├─ 8k_earth_nightmap.jpg
├─ 8k_earth_normal_map.png
├─ 8k_earth_specular_map.png
└─ milkyway_2020_8k.hdr
```

これらがない場合、EarthステージやBlack Holeステージは正しく表示されません。

## 今後やりたいこと

- カメラと入力システムの拡張
- 2D / 3D描画APIの整理
- Actor・Componentなど、シーンを構成する仕組みの追加
- ライティングとマテリアルの拡張
- アセット管理の改善
- 外部ライブラリ設定のポータブル化

OpenGLとリアルタイムグラフィックスを理解することを第一に、必要になった機能を自分で実装しながら進めています。
