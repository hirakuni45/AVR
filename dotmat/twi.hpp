#pragma once
//=====================================================================//
/*!	@file
	@brief	TWI(I2C)制御クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <avr/io.h>
#include <stdint.h>

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	TWI クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class twi {

		uint8_t	dev_adr_;

	public:
		twi() : dev_adr_(0b11010000) { }

		void init() {
			// 20MHz ---> 100KHz(clock)
			TWBR = 23;
			TWSR = 0x01;	// 1/4
			TWCR = 1 << TWEN;
		}

		void set_device_adr(uint8_t adr) { dev_adr_ = adr; }

		bool send(const uint8_t* data, uint8_t num) {
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);	// start condition

			// start condition state
			while(!(TWCR & (1 <<TWINT))) ;

			// START: 0x08
			if((TWSR & 0xf8) != 0x08) {
				return false;
			}

			TWDR = dev_adr_;	// write mode
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 <<TWINT))) ;

			// MT_SLA_ACK: 0x18
			if((TWSR & 0xf8) != 0x18) {
				return false;
			}

			while(num) {
				TWDR = *data++;
				TWCR = (1 << TWINT) | (1 << TWEN);
				while(!(TWCR & (1 <<TWINT))) ;

				// MT_DATA_ACK: 0x28
				if((TWSR & 0xf8) != 0x28) {
					return false;
				}
				--num;
			}

			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);	// 停止

			return true;
		}


		bool recv(uint8_t* data, uint8_t num) {
			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);	// start condition

			while(!(TWCR & (1 <<TWINT))) ;

			// START: 0x08
			if((TWSR & 0xf8) != 0x08) {
//				return false;
			}

			TWDR = dev_adr_ | 0x01;	// read mode
			TWCR = (1 << TWINT) | (1 << TWEN);
			while(!(TWCR & (1 <<TWINT))) ;

			// MT_SLA_ACK: 0x18
			if((TWSR & 0xf8) != 0x18) {
//				return false;
			}

			while(num) {
				TWCR = (1 << TWINT) | (1 << TWEN);
				while(!(TWCR & (1 <<TWINT))) ;
				*data++ = TWDR;

				// MT_DATA_ACK: 0x40
				if((TWSR & 0xf8) != 0x40) {
//					return false;
				}
				--num;
			}

			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);	// 停止

			return true;
		}

	};
}
