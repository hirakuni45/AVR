#ifndef UART_H
#define UART_H
//=====================================================================//
/*!	@file
	@breif	AVR シリアル入出力の為のクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//

enum UART_RECV_STATUS {
	FRAME_ERR = 0x10,
	OVER_RUN_ERR = 0x08,
	PARITY_ERR = 0x04,
};


#ifdef __cplusplus
extern "C" {
#endif

	//-----------------------------------------------------------------//
	/*!
		@breif	UART の初期化@n
				初期化では、リセットの初期化に依存するので、それを変更@n
				した場合は、適切な値を設定する事。@n
				※通常、８ビット、１ストップビット、パリティ無し
		@param[in]	chn		UART チャネル
		@param[in]	baud	ボーレート
	*/
	//-----------------------------------------------------------------//
	void uart_init(uint8_t chn, unsigned long baud);


	//-----------------------------------------------------------------//
	/*!
		@breif	レシーブステータスを得る
		@param[in]	chn		UART チャネル
		@return ステータス情報
	*/
	//-----------------------------------------------------------------//
	enum UART_RECV_STATUS uart_get_status(uint8_t chn);


	//-----------------------------------------------------------------//
	/*!
		@breif	現在のレシーブデータサイズを得る
		@param[in]	chn		UART チャネル
		@return レシーブバッファに溜まっているデータ長を返す
	*/
	//-----------------------------------------------------------------//
	uint8_t uart_recv_length(uint8_t chn);


	//-----------------------------------------------------------------//
	/*!
		@breif	データを受信する
		@param[in]	chn		UART チャネル
		@return 取り出したレシーブデータ
	*/
	//-----------------------------------------------------------------//
	uint8_t uart_recv(uint8_t chn);


	//-----------------------------------------------------------------//
	/*!
		@breif	データを送信する
		@param[in]	chn		UART チャネル
		@param[in]	ch	送信するデータ
	*/
	//-----------------------------------------------------------------//
	void uart_send(uint8_t chn, uint8_t ch);


	//-----------------------------------------------------------------//
	/*!
		@breif	文字列の送信（プログラムエリア専用）
		@param[in]	chn		UART チャネル
		@param[in]	text	送信する文字列
	*/
	//-----------------------------------------------------------------//
	void uart_send_text_p(uint8_t chn, const char* text);


	//-----------------------------------------------------------------//
	/*!
		@breif	文字列の送信
		@param[in]	chn		UART チャネル
		@param[in]	text	送信する文字列
	*/
	//-----------------------------------------------------------------//
	void uart_send_text(uint8_t chn, const char* text);


	//-----------------------------------------------------------------//
	/*!
		@breif	CR/LF の送信
		@param[in]	chn		UART チャネル
	*/
	//-----------------------------------------------------------------//
	void uart_send_crlf(uint8_t chn);


#ifdef __cplusplus
};
#endif

#endif // UART_H