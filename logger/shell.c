//=====================================================================//
/*!	@file
	@breif	コマンド・シェル
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include "xitoa.h"
#include "uart.h"
#include "fileio.h"
#include "mainloop.h"

#define SHELL_LINE_MAX	128

static uint8_t g_line_pos;
static uint8_t g_prompt;
static char g_line_buff[SHELL_LINE_MAX];

static void analize_command(char *line)
{
	char ch;
	char cb = ' ';
	uint8_t argc = 0;
	const char *argv[4];
	do {
		ch = *line;
		if(cb == ' ' && ch != ' ') {
			argv[argc] = line;
		} else if(cb != ' ' && (ch == ' ' || ch == 0)) {
			*line = 0;
			++argc;
		}
		++line;
		cb = ch;
	} while(ch != 0) ;

	if(argc > 0) {
		if(strcmp_P(argv[0], PSTR("ls")) == 0) {
			if(argc == 1) {
//				fs_list("/");
			} else {
//				fs_list(argv[1]);
			}
		} else {
			xprintf(PSTR("Command error: '%s'\n\n"), argv[0]);
		}
	}
}


//-----------------------------------------------------------------//
/*!
	@breif	コマンド・シェル初期化
*/
//-----------------------------------------------------------------//
void shell_init(void)
{
	xprintf(PSTR("\nG.P.S. Data Logger Command Shell\n"));
	const struct tm *sttm = get_time();
	xprint_date(sttm);
	xprintf(PSTR("\n"));
	g_line_pos = 0;
	g_prompt = 1;
}


//-----------------------------------------------------------------//
/*!
	@breif	コマンド・シェル・サービス
*/
//-----------------------------------------------------------------//
void shell_service(void)
{
	if(g_prompt) {
		uart_send(0, '#');
		g_prompt = 0;
	}

	// エコーバック
	while(uart_recv_length(0) > 0) {
		uint8_t ch = uart_recv(0);

		if(ch == 0x08) {
			if(g_line_pos) {
				--g_line_pos;
				uart_send(0, ch);
			}
		} else if(ch == 0x0d) {
			uart_send_crlf(0);
			g_line_buff[g_line_pos] = 0;
			analize_command(g_line_buff);
			g_line_pos = 0;
			g_prompt = 1;
		} else {
			if(g_line_pos < (SHELL_LINE_MAX - 1)) {
				g_line_buff[g_line_pos] = (char)ch;
				++g_line_pos;
				uart_send(0, ch);
			} else {
				uart_send(0, 0x7);	// BELL
				uart_send(0, '/');
				uart_send_crlf(0);
			}
		}
	}
}


/* ----- End Of File "shell.c" ----- */


