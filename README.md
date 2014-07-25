#GASHA#
**Gakimaru's standard library for C++**  
Copyright (c) 2014 Itagaki Mamoru  
Released under the [MIT license][MIT].  
<https://github.com/gakimaru/gasha_src/blob/master/LICENSE>
[MIT]: http://www.opensource.org/licenses/mit-license.php

###ライブラリソース用リポジトリ###
**このリポジトリは、**GASHA** のソースファイル用です。**
> ライブラリをビルドするためのソースファイル（`.cpp` ファイル）とプロジェクトファイル（`.vcxproj` / `.sln` ファイルおよび `Makefile`）を扱っています。  

#「GASHA」とは？#
###個人制作による、オープンソースのC++用基本ライブラリです。###
> 詳しくは、「GASHA」の[サンプルプログラム][gasha_examples]に記述しています。

#構成リポジトリ#
**GASHA**は、下記のように複数のリポジトリで構成しています。  
* [`gasha` ライブラリ本体用リポジトリ][gasha]  
* [`gasha_settings` プロジェクト固有のライブラリ挙動カスタマイズ用リポジトリ][gasha_settings]  
* [`gasha_src` ライブラリソース用リポジトリ][gasha_src]  
* [`gasha_examples` サンプルプログラム用リポジトリ][gasha_examples]  
* [`gasha_proj` ライブラリビルド用リポジトリ][gasha_proj]  
[gasha]: https://github.com/gakimaru/gasha
[gasha_settings]: https://github.com/gakimaru/gasha_settings
[gasha_src]: https://github.com/gakimaru/gasha_src
[gasha_examples]: https://github.com/gakimaru/gasha_examples
[gasha_proj]: https://github.com/gakimaru/gasha_proj

> **GASHA** を使用する開発プロジェクトのソースコード管理に、（対象バージョンの）**GASHA** を、サブモジュールとしてえ組み込むことを想定した構成です。  
> また、開発プロジェクトごとに **GASHA** の挙動をカスタマイズできるように、リポジトリを分割しています。  
> 必要に応じて、ソースコードを隠蔽した状態で **GASHA** を配布することにも対応できるように構成しています。  

■■
