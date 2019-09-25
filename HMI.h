/*
  HMI.h: wrap-around for the Scenario class.
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

  Nota Bene i #define di override delle funzioni base devono essere messi da soli e caricati per ultimi, 
  altrimenti nono trovano la classe completamente definita. Quindi questo file non può essere inglobato 
  in Scenario.h
*/


#ifndef HMI_
#define HMI_

// rev 2.0 6-2-19  aggiunti analog read/write, per sviluppi futuri

#include <Scenario.h>

#define digitalWrite(a,b) Simula.pinWrite(a,b)
#define digitalRead(a) Simula.pinRead(a)
#define analogWrite(a,b) Simula.intWrite(a,b)
#define analogRead(a) Simula.intRead(a)
#define pinMode(a,b) Simula.pinMode(a,b)
#define delay(a) Simula.delay(a)
#define synch() Simula.synch()

#endif