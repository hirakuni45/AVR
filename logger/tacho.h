#ifndef TACHO_H
#define TACHO_H
//=====================================================================//
/*!	@file
	@breif	エンジン回転表示関係（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void tacho_init(void);
void tacho_main(void);

#ifdef __cplusplus
};
#endif

#endif // TACHO_H
