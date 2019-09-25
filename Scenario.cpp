/*
  Scenario.cpp: classe per la comunicazione con PC_SIMU.
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

#include "Scenario.h"

extern void nuovoCarattere(const unsigned char carattere, Scenario *plc);

//----------------------------------------------------

// oggetto globale
Scenario Simula;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//----------------------------------------------------

Scenario::Scenario(){
	int n, i;
	
	for(n=0; n<numeroByteIngresso; n++)
		for(i=0; i<8; i++)
			I[n][i] = false;

	for(n=0; n<numeroByteUscita; n++)
		for(i=0; i<8; i++)
			Q[n][i] = false;
}

//----------------------------------------------------
void Scenario::pinMode(int pin, int modo) {
	pin = pin;
	modo = modo;
	return;
}
//----------------------------------------------------
//----------------------------------------------
void Scenario::synch(){
	immagineIngresso();
}
//----------------------------------------------
void Scenario::immagineIngresso(void){
	unsigned char nuovo;
	while (Serial.available()){
		nuovo = (unsigned char)Serial.read();
		nuovoCarattere(nuovo, this);
	}
}

//----------------------------------------------
void Scenario::cambiaBitIngresso(const unsigned char *buffer, const unsigned char numByte){
	const unsigned char mask = 1;

	for(int n=0; n<numByte; n++)
		for(int i=0; i<8; i++){
			if (buffer[n] & mask << i)
				I[n][i] = true;
			else
				I[n][i] = false;
		}
}

//----------------------------------------------
void Scenario::leggiBitUscita(unsigned char *buffer, const unsigned char numByte)const{
	const unsigned char mask = 1;
	for(int n=0; n<numByte; n++){
		buffer[n] = 0;
		for(int i=0; i<8; i++)
			if (Q[n][i] == true)
				buffer[n] |= mask << i;
	}
}

//------------------------------

#define BIT_INGRESSO 5
#define BIT_USCITA 7
//------------------------------
boolean *const Scenario::keepPin(const int codicePin) {

  int valore = codicePin % 1000;          // limita
  int senso = 0;
  int banco = 0;

  const int bitte = valore % 10;
  if (bitte > 7)
    goto fuori ;
  valore /= 10;

  banco = valore % 10;
  if (banco > 1)
    goto fuori ;

  senso = valore / 10;
  switch (senso) {
    case BIT_INGRESSO:
      return &Simula.I[banco][bitte];
    case BIT_USCITA:
      return &Simula.Q[banco][bitte];
    default:
      break;
  }

fuori:
  return NULL;
}
//------------------------------
// 30/11/18 scoperto che è possibile leggere ilo stato di un pin
// impostato come uscita, quindi si implementa anche nell'HMI

const boolean Scenario::pinRead(const int pin) {

  int codicePin ;

  if (pin>=0 && pin <= 15)
    codicePin =pin / 8 * 10 + pin % 8 + 500;
  else
    if (pin>=20 && pin <= 35)
      codicePin = (pin-20) / 8 * 10 + (pin-20) % 8 + 700;
    else
	return false;

  const boolean *const pinValue = keepPin(codicePin);  // converte in senso, banco e bit
  if (pinValue == NULL)
    return false;
  else
    return *pinValue;
}
//------------------------------
void Scenario::pinWrite(const int pin, const boolean value) {

  if (pin<20 || pin > 35)
	return;

   boolean *const pinValue = keepPin((pin-20) / 8 * 10 + (pin-20) % 8 + 700);  // da Q00 a Q17

  if (pinValue != NULL)
    *pinValue = value;
}

//--------------------------------------------
void Scenario::delay(const unsigned long ms){
  unsigned long primo = millis();

	do {
		synch();	
	}  while(millis()-primo < ms);
  }

//--------------------------------------------
//--------------------------------------------
//--------------------------------------------
//--------------------------------------------

