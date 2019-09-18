![logo](./doc/logo.png)

aimai: ESP32で動作するFMトランシーバー付き名札

![badge](./doc/badge.png)

## 機能

- FMラジオ受信機能
  - 音声の受信
  - [Radio Data System (RDS)](https://ja.wikipedia.org/wiki/Radio_Data_System) によるテキストデータの受信
- FMラジオ送信機能
  - 音声の送信
  - RDSによるテキストデータの送信
- ボードにあなたの名前を書けます！！

## 使い方

### 受信 (RXモード)

強く推奨: イヤホンケーブルをFMラジオのアンテナとして使用するため、イヤホンをRXモジュール (紫色のもの) のイヤホンジャックに挿入してください。そうしない場合、RDSを介した文字情報受信ができない場合があります。

- 左上のトグルスイッチを使って電源を入れてください
- 右上のトグルスイッチを使ってRXモードに切り替えてください
- 左のつまみ: 音量を調整します
- 右のつまみ: 受信周波数を調整します
- OLEDディスプレイ: 受信したテキストデータを表示します

### 送信 (TXモード)

- 左上のトグルスイッチを使って電源を入れてください
- 右上のトグルスイッチを使ってTXモードに切り替えてください
- このモードはTXモジュールのイヤホンジャックに入力された音声データを送信します
- 左のつまみ: RDSのテキストデータを入力します
  - つまみを回すと文字を選択できます
  - つまみを押し込む (クリックする) と文字を決定します
  - 20文字入力する必要があります (ヒント: スペース文字をサポートしています)
- 右のつまみ: 送信周波数を調整します
  - つまみを押し込むとテキストデータ入力モードから抜けます
- OLEDディスプレイ: 送信しているテキストデータを送信します

## ビルドとESP32へのアップロード方法

### 必要なもの

- [pythonz](https://github.com/saghul/pythonz)
- [direnv](https://github.com/direnv/direnv)

### プロジェクトのブートストラップ方法

```
$ make bootstrap
```

### ビルド方法

```
$ make build
```

### ESP32へのアップロード方法

```
$ make upload
```

もし何か問題が生じた場合は [platformio.ini](./platformio.ini) の `upload_port` を編集して調整してください。

## ボード制作

回路図とPCBデータが以下にあるので参照してください: [pcb](./pcb/)

## Makers

- [uzulla](https://github.com/uzulla)
- [mackee](https://github.com/mackee)
- [moznion](https://github.com/moznion)

