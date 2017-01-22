
ATMEGA1284P を使ったデータロガー

ATmega1284P を使ったデータロガーのソフトウェアー
・128×64 のモノクロ液晶を表示装置として使用（秋月電子で販売）
・12ビット8チャネルのA/Dコンバーター（MCP3208）
・SUP500F GPS モジュール利用

ファイルの構成

avr_mmc/
		・ChaN 氏の FatFs 関係ソースコード
		・mmc.c 関係に必要なコードのみ収集
		・Shift-JIS Unicode 変換表（cc932_avr.c)に、いくつかのコードを追加

bitmap/
		・グラフィックスのデータ（PNG）とモノクロに変換したバイナリー
		・変換に必要なツールは別途用意する
		※変換用ツールのリンクは以下を参照（ソースコードもある）


コネクターのピンアサイン：

アナログ入力

8PIN-KEY1
1: N.C.(KEY)
2: Vref(4.096V)
3: Analog-0
4: Analog-1
5: AGND
6: AGND
7: Analog-2
8: Analog-3

8PIN-KEY2
1: Vref(4.096V)
2: N.C.(KEY)
3: Analog-4
4: Analog-5 水温測定用 (0 度 to 163.84 度)
5: AGND
6: AGND
7: Analog-6(non bufferd) 車載バッテリー監視用（0V to 16.384V)
8: VCC(5V)

4PIN-KEY2
1: +5V
2: N.C.(KEY)
3: GND
4: Puls Input
