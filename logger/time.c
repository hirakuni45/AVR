//=====================================================================//
/*!	@file
	@breif	時間関数
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <stdio.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "time.h"
#include "xitoa.h"

// 各月に対する日数（通常年）
static const prog_char mday_tbl[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const prog_char g_week_tbl[] = {
	"Sun\0Mon\0Tue\0Wed\0Thu\0Fri\0Sat\0"
};

static const prog_char g_mon_tbl[] = {
	"Jan\0Feb\0Mar\0Apr\0May\0Jun\0Jul\0Aug\0Sep\0Oct\0Nov\0Dec\0"
};

/// 大阪、札幌、東京のタイムゾーン +9
static char g_timezone_offset = 9;
static struct tm g_time_st;

//-----------------------------------------------------------------//
/*!
	@breif	西暦から、その年がうるう年かを判定
	@param[in]	year	西暦
	@return		うるう年なら「0」以外
*/
//-----------------------------------------------------------------//
char check_leap_year(short year)
{
	if((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0)) return 1;
	return 0;
}


//-----------------------------------------------------------------//
/*!
	@breif	西暦と、月から、その月の最大日数を得る。
	@param[in]	year	西暦
	@param[in]	mon		月（0 to 11)
	@return		「月」に対応する最大日数
*/
//-----------------------------------------------------------------//
char get_mday(short year, char mon)
{
	char	d;

	d = pgm_read_byte_near(mday_tbl + mon);
	if(mon == 1) {	// 2月
		if(check_leap_year(year)) d++;
	}
	return d;
}


//-----------------------------------------------------------------//
/*!
	@breif	西暦から、その年の総日数を得る。
	@param[in]	year	西暦
	@return		「年」に対応する総日数
*/
//-----------------------------------------------------------------//
short get_yday(short year)
{
	if(check_leap_year(year)) return 366;
	else return 365;
}


//-----------------------------------------------------------------//
/*!
	@breif	西暦、月から、1970 年からの総日数を得る。
	@param[in]	year	西暦 1970 〜
	@param[in]	mon		月	[0..11]
	@param[in]	day		日	[1..31]
	@return		1970 年1月1日からの総日数
*/
//-----------------------------------------------------------------//
long get_total_day(short year, char mon, char day)
{
	if(year < 1970) return -1L;

	long d = 0;
	short i = 1970;
	while(i < year) {
		d += (long)get_yday(i);
		i++;
	}

	i = 0;
	while(i < mon) {
		d += get_mday(year, i);
		i++;
	}

	d += day - 1;

	return d;
}


//-----------------------------------------------------------------//
/*!
	@breif	世界標準時間（グリニッジ）からのオフセットを取得
	@return	オフセット時間（秒）
*/
//-----------------------------------------------------------------//
time_t get_timezone_offset(void)
{
	return (time_t)g_timezone_offset * 3600;
}


//-----------------------------------------------------------------//
/*!
	@breif	世界標準時間（グリニッジ）から、tm 構造体のメンバー@n
			を生成する。
	@param[in]	tp
	@return		グローバル tm 構造体のポインター
*/
//-----------------------------------------------------------------//
struct tm *gmtime(const time_t *tp)
{
	time_t	t;
	short	i, j, k;

	t = *tp;

// GMT から JST へ(+9時間）
	t += (time_t)(g_timezone_offset) * 3600;

	g_time_st.tm_sec  = t % (time_t)60;
	t /= (time_t)60;

	g_time_st.tm_min  = t % (time_t)60;
	t /= (time_t)60;

	g_time_st.tm_hour = t % (time_t)24;
	t /= (time_t)24;

	g_time_st.tm_wday = (t + 4) % 7;

	j = 1970;
	while(t > (i = get_yday(j))) {
		t -= (time_t)i;
		j++;
	}
	g_time_st.tm_year = j - 1900;

	g_time_st.tm_yday = t;

	k = 0;
	while(t > (i = get_mday(j, k))) {
		t -= (time_t)i;
		k++;
	}
	g_time_st.tm_mon = k;
	g_time_st.tm_mday = t + 1;

	return &g_time_st;
}


//-----------------------------------------------------------------//
/*!
	@breif	tm 構造体から、世界標準(グリニッジ)時間を得る@n
			※メンバー変数 tm_yday は再計算される。@n
			※メンバー変数 tm_wday は再計算される。@n
			※ tm_isdgt は無視される。
	@param[in]	tmp	tm 構造体のポインター@n
				※NULLの場合は、システムの構造体が使われる
	@return		GMT:1970年1月1日0時0分0秒(4:THU)からの経過時間（秒）
*/
//-----------------------------------------------------------------//
time_t mktime(const struct tm *tmp)
{
	time_t	t;
	long	td;

	if(tmp == NULL) tmp = &g_time_st;

	t  = (time_t)tmp->tm_sec;
	t += (time_t)(tmp->tm_min * 60);
	t += (time_t)tmp->tm_hour * 3600L;
	td = get_total_day(tmp->tm_year + 1900, tmp->tm_mon, tmp->tm_mday);
	t += (time_t)td * 86400L;

// GMT からの偏差(-9時間）
	t -= (time_t)g_timezone_offset * 3600;

	return t;
}


//-----------------------------------------------------------------//
/*!
	@breif	このモジュールで利用している tm 構造体のポインターを得る。
	@return		tm 構造体のポインター
*/
//-----------------------------------------------------------------//
struct tm *get_tm(void)
{
	return &g_time_st;
}


//-----------------------------------------------------------------//
/*!
	@breif	曜日のテキストを返す。
	@param[in]	week	曜日インデックス（0 to 6)
	@return		曜日のテキスト・ポインター
*/
//-----------------------------------------------------------------//
const prog_char *get_week_text(uint8_t week)
{
	if(week > 6) return &g_week_tbl[3];
	return &g_week_tbl[week << 2];
}


//-----------------------------------------------------------------//
/*!
	@breif	月のテキストを返す。
	@param[in]	mon_index	月のインデックス（0 to 11)
	@return		月のテキスト・ポインター
*/
//-----------------------------------------------------------------//
const prog_char *get_mon_text(uint8_t mon)
{
	if(mon > 11) return &g_mon_tbl[3];
	return &g_mon_tbl[mon << 2];
}


//-----------------------------------------------------------------//
/*!
	@breif	時間と日付の表示
	@param[in]	sttm	時間構造体
*/
//-----------------------------------------------------------------//
void xprint_date(const struct tm* sttm)
{
	xprintf(get_week_text(sttm->tm_wday));
	xprintf(PSTR(" "));
	xprintf(get_mon_text(sttm->tm_mon));
	xprintf(PSTR(" "));
	xprintf(PSTR("%d %02d:%02d.%02d %d"),
			sttm->tm_mday, sttm->tm_hour, sttm->tm_min, sttm->tm_sec, sttm->tm_year + 1900);
}


/* ----- End Of File "time.c" ----- */
