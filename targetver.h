#pragma once

// // SDKDDKVer.h をインクルードすると、利用できる最も高いレベルの Windows プラットフォームが定義されます。
// 以前の Windows プラットフォーム用にアプリケーションをビルドする場合は、WinSDKVer.h をインクルードし、
// サポートしたいプラットフォームに _WIN32_WINNT マクロを設定してから SDKDDKVer.h をインクルードします。
#include <WinSDKVer.h>
#define _WIN32_WINNT 0x0601 //_WIN32_WINNT_WIN7
#include <SDKDDKVer.h>
