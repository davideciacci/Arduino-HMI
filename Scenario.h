/*
  Scenario.h: classe per la comunicazione con PC_SIMU.
  Written by Davide Ciacci
  rev 2.0 June 2019
  Copyright (c) 2019 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */
//////////////////////////////////////////////////////////////////////

#ifndef Scenario_H_
#define Scenario_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"	// for digitalRead, digitalWrite, etc
#else
#include "WProgram.h"
#endif


/*
#define I00 500
#define I01 501
#define I02 502
#define I03 503
#define I04 504
#define I05 505
#define I06 506
#define I07 507

#define Q00 700
#define Q01 701
#define Q02 702
#define Q03 703
#define Q04 704
#define Q05 705
#define Q06 706
#define Q07 707

#define I10 510
#define I11 511
#define I12 512
#define I13 513
#define I14 514
#define I15 515
#define I16 516
#define I17 517

#define Q10 710
#define Q11 711
#define Q12 712
#define Q13 713
#define Q14 714
#define Q15 715
#define Q16 716
#define Q17 717
*/


class Scenario  
{
public:
		
	enum {
		numeroByteIngresso = 2,
		numeroByteUscita = 2
	};

	const boolean pinRead(const int codicePin);
	const int intRead(const int codicePin);       // 6-2-19
	void pinMode(int codicePin, int value); 
	void pinWrite(const int codicePin, const boolean value);
	void intWrite(const int codicePin, const int value);       // 6-2-19
	void delay(unsigned long ms);
	void synch();
	Scenario();

	void cambiaBitIngresso(const unsigned char *buffer, const unsigned char numByte);
	void leggiBitUscita(unsigned char *buffer, const unsigned char numByte) const;
protected:

private:
	boolean *const keepPin(const int codicePin);

	boolean I[numeroByteIngresso][8];
	boolean Q[numeroByteUscita][8];

	void immagineIngresso(void);
};

extern Scenario Simula;


#endif
