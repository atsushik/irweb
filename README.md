# irweb
iirkitもどきを安く作るための何か

## 材料
- [ESP8266なWIFIモジュール](http://akizukidenshi.com/catalog/g/gK-09758/) とARDUINOとして使うための一式
- [５ｍｍ赤外線ＬＥＤ　ＯＳＩ５ＬＡ５１１３Ａ　（１０個入）](http://akizukidenshi.com/catalog/g/gI-04311/)
- [赤外線リモコン受信モジュール　ＧＰ１ＵＸＣ４１ＱＳ](http://akizukidenshi.com/catalog/g/gI-06487/)
- [角型２色ＬＥＤ　赤・黄緑　ＧＬ８ＥＤ４８　（１０個)](http://akizukidenshi.com/catalog/g/gI-03255/)

## 使い方
### 受信
受信したリモコンの信号は下記のURLにアクセスするとjson形式で取得できる

http://_esp8266のIPかFQDN_/messages
``` json:aquos_power.json
{"format":"raw","freq":38,"len":99,"data":[3400,1700,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,450,450,1250,450,1250,450,450,450,1250,450,450,450,1250,450,1250,450,1250,450,1250,450,450,450,450,450,450,450,1250,450,450,450,1250,450,450,450,450,450,1250,450,450,450,450,450,450,450,450,450,1250,450,1250,450,450,450,1250,450,450,450,450,450,450,450,1250,450,450,450,450,450,450,450,1250,450,450,450,1250,450,1250,450]}
```


