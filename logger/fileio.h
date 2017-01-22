#ifndef FILEIO_H
#define FILEIO_H
//=====================================================================//
/*!	@file
	@breif	ファイル入出力関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include "ff.h"

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
/*!
	@breif	fileio ステータス・フラグ
 */
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
enum FILEIO_STS {
	FILEIO_INS_LEVEL    = 0x01,		///< カードスロットの状態
	FILEIO_INS_POSITIVE = 0x02,		///< カードスロットに刺さった
	FILEIO_INS_NEGATIVE = 0x04,		///< カードスロットから抜かれた
	FILEIO_DISK_INIT    = 0x08,		///< FATFS 初期化終了
	FILEIO_DISK_MOUNT   = 0x10,		///< FATFS マウント終了

	FILEIO_DISK_NG      = 0x80		///< カード不良で初期化、マウント出来ない
};

#ifdef __cplusplus
extern "C" {
#endif
	//-----------------------------------------------------------------//
	/*!
		@breif	ファイルシステム・初期化
	 */
	//-----------------------------------------------------------------//
	void fileio_init(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	日付を使ってディレクトリーを作成
		@param[in]	path	日付名のパスを返す
		@return	成功した場合「０」、失敗したら「-1」
	*/
	//-----------------------------------------------------------------//
	int mkdir_date(char *path);


	//-----------------------------------------------------------------//
	/*!
		@breif	ファイルシステム・サービス@n
				※毎フレーム（一定のインターバルで）呼び出す事。
	 */
	//-----------------------------------------------------------------//
	void fileio_service(void);


	//-----------------------------------------------------------------//
	/*!
		@breif	状況を返す
		@return FILEIO ステータス
	*/
	//-----------------------------------------------------------------//
	enum FILEIO_STS fileio_get_status(void);

#ifdef __cplusplus
};
#endif

#endif // FILEIO_H
