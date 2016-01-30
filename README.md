# irweb
iirkitもどきを安く作るための何か

## 材料
- [ESP8266なWIFIモジュール](http://akizukidenshi.com/catalog/g/gK-09758/) とARDUINOとして使うための一式
- [５ｍｍ赤外線ＬＥＤ　ＯＳＩ５ＬＡ５１１３Ａ　（１０個入）](http://akizukidenshi.com/catalog/g/gI-04311/)
- [赤外線リモコン受信モジュール　ＧＰ１ＵＸＣ４１ＱＳ](http://akizukidenshi.com/catalog/g/gI-06487/)
- [角型２色ＬＥＤ　赤・黄緑　ＧＬ８ＥＤ４８　（１０個)](http://akizukidenshi.com/catalog/g/gI-03255/)

## 配線
NODEMCUの絵を使っているけど、WROOMに適宜読み替える
![配線図](https://raw.githubusercontent.com/atsushik/irweb/master/irweb.png)

## 依存しているライブラリ
- [tzapu/WiFiManager](https://github.com/tzapu/WiFiManager)
- [markszabo/IRremoteESP8266](https://github.com/markszabo/IRremoteESP8266)
- [bblanchon/ArduinoJson](https://github.com/bblanchon/ArduinoJson)

## 使い方

### 初回起動時
「irweb_c1a1」みたいなWIFI-APが出現するので、そこにつなぐ。
少し待つと「Configure WiFi」と書かれたボタンが出るので、クリックして、表示されるAP一覧から接続したいのを選びパスワードを入力する。
次回起動時からはその情報を使ってWIFIに接続するようになる。
AP情報を消すにはWIFIモジュールの起動時に GPIO 5 をGNDに繋いでおくと、保存していた情報を消す。

### esp8266の探し方
osxならdns-sd コマンドで _irweb._tcp を探すとホスト名がわかる。
「irweb_」+「MACアドレスの下４文字」になっている
``` sh:find_irweb.sh
$ dns-sd -B _irweb._tcp
Browsing for _irweb._tcp
DATE: ---Sun 31 Jan 2016---
 2:21:45.561  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
 2:21:46.040  Add        2   5 local.               _irweb._tcp.         irweb_c1a1
```

### 受信
受信したリモコンの信号は下記のURLにアクセスするとjson形式で取得できる

http://irweb_c1a1.local/messages
``` json:aquos_power.json
{"format":"raw","freq":38,"len":99,"data":[3400,1700,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,1250,450,450,450,1250,450,450,450,1250,450,1250,450,1250,450,1250,450,450,450,450,450,450,450,1250,450,450,450,1250,450,450,450,450,450,1250,450,450,450,450,450,450,450,450,450,1250,450,1250,450,450,450,1250,450,450,450,450,450,450,450,1250,450,450,450,450,450,450,450,1250,450,450,450,1250,450,1250,450]}
```

### 送信
上記で取得したjsonをPOSTするとそれに応じてリモコン信号を送信する

``` sh:sendIrSignal.sh
curl -i "http://irweb_c1a1.local/messages" -H "X-Requested-With: curl" --data-binary @/irweb/data/sharp/ac-222fd/warm20_Louver2.json
```



