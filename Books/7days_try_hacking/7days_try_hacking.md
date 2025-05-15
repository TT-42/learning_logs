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
            - フォアグラウンドで実行中の場合はCtrl+C、バックグラウンドで実行中の場合は`pkill openvpn`でVPN接続解除
        1. ホストOSにてTryHackMeへログイン
        1. ホストOSにて任意のルームのターゲットマシンを起動する
    - `7DaysTryHacking 1`として環境構築直後の状態をスナップショットとして保存