/*
  PPI.cpp: classe per la comunicazione con PC_SIMU via protocollo PPI.
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


// risposte fisse, la lettura deve essere modificata

const unsigned char negozioRisposta[] = {
0x68, 0x17, 0x17, 0x68, 0x00, 0x02, 0x08, 0x32,
0x03, 0x00, 0x00, 0xCC, 0xC1, 0x00, 0x08, 0x00,
0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x01, 0x00,
0x01, 0x00, 0xF0, 0xB7, 0x16};

const unsigned char runRisposta[] = {
0x68, 0x10, 0x10, 0x68, 0x00, 0x02, 0x08, 0x32,
0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
0x00, 0x00, 0x00, 0x28, 0x68, 0x16};


const unsigned char scritturaRisposta[] = {
0x68, 0x1E, 0x1E, 0x68, 0x01, 0x02, 0x08, 0x32,
0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00,
0x05, 0x00, 0x01, 0x12, 0x08, 0x12, 0x88, 0x0B,
0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x09, 0x00,
0x01, 0xFF, 0x1E, 0x16};

// lettura const su uC ma da cambiare
const unsigned char letturaRisposta[] = {
0x68, 0x23, 0x23, 0x68, 0x01, 0x02, 0x08, 0x32,
0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
0x12, 0x00, 0x00, 0x04, 0x02, 0xFF, 0x04, 0x00,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xFF, 0x04, 0x00, 0x10, 0x00, 0x00, 0xB0, 0x16};

const unsigned char stopRisposta[] = {
0x68, 0x10, 0x10, 0x68, 0x00, 0x02, 0x08, 0x32, 
0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
0x00, 0x00, 0x00, 0x29, 0x69, 0x16};

//Risposta  di scrittura dopo lo stop

const unsigned char dopoRisposta[] = {
0x68, 0x1D, 0x1D, 0x68, 0x00, 0x02, 0x08, 0x32,
0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 
0x04, 0x00, 0x01, 0x12, 0x08, 0x12, 0x88, 0x0B, 
0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x09, 0x00, 
0x00, 0x1C, 0x16};

enum {
	DLE = 0x10,
	SYN = 0x16,
	START = 0x68

} codici;

// stati
enum {
	HOME,
	ATTESA_DIMENSIONE,  // è arrivato il primo ESC, aspetta per sapere la dimensione del pacchetto
	IN_RICHIESTA,        // conosce la dimensione, aspetta che sia completa
	IN_SOLLECITO

} stati;

int stato = HOME;

// massima dimensione pacchetto
#define MAXDIM 64

//unsigned long ultimoTempo = 0;
//const unsigned long massimoTempo = 100; // millisecondi
unsigned char buffer[MAXDIM]; // ATTENZIONE !!!!!!!!!!!!!!!!!!!!!!!!!!!!
unsigned char indice = 0; // del buffer;
unsigned char dimensione = 0; // del buffer;
const unsigned char *rispostaCorrente = 0;

//---------------------------------------------
void nuovo(const unsigned char carattere);
void scegliRisposta(Scenario *simu);
void inviaRisposta(Scenario *simu);
void reset(void);
void azzera(void);
boolean errore(const int numero);
void inviaConferma(const unsigned char *risposta);
void leggiBitIngresso(const unsigned char *forzata);
void scriviBitUscita(unsigned char *uscite);

//---------------------------------------------
void nuovo(unsigned char carattere){
	buffer[indice++] = carattere;
}
//---------------------------------------------
// solo il canale può vedere il timeout
void timeout(void){
}
//---------------------------------------------
void inviaRisposta(Scenario *sim){

#ifdef WIN32
	DocumentaText::instance()->riga("Risposta: ");
#endif

	if (rispostaCorrente == 0){

#ifdef WIN32
		DocumentaText::instance()->errore(9);
#endif

		reset();
		return;
	}
	if (rispostaCorrente == letturaRisposta){
		// prima parte fissa fuori checksum
		int i=0;
		for (i=0; i< 4; i++)
			Serial.write(rispostaCorrente[i]);

		// parte fissa nella checksum
		unsigned char checksum = 0;
		for (; i< 25; i++){
			checksum += rispostaCorrente[i];
			Serial.write(rispostaCorrente[i]);
		}
		
		// parte variabile con i valori dll'uscita
		unsigned char buffer[sim->numeroByteUscita];
		sim->leggiBitUscita(buffer,sim->numeroByteUscita);
		for (int n=0; n< sim->numeroByteUscita ; n++,i++){
			checksum += buffer[n];
			Serial.write(buffer[n]);
		}
		
		// coda fissa
		for (; i< rispostaCorrente[1]+6-2; i++){
			checksum += rispostaCorrente[i];
			Serial.write(rispostaCorrente[i]);
		}
		
		Serial.write(checksum);
		Serial.write(SYN);
	}
	else
		for (int i=0; i< rispostaCorrente[1]+6; i++){
			Serial.write(rispostaCorrente[i]);
	}
	reset();

#ifdef WIN32
	DocumentaText::instance()->riga("Richiesta: ");
#endif
}
//---------------------------------------------
void azzera(void){
	indice = 0;	
	dimensione = 0;
}
//---------------------------------------------
boolean errore(const int numero){
	reset();
#ifdef WIN32
	DocumentaText::instance()->errore(numero);
#endif
	// DocumentaText::instance()->chiudi();
	return false;
}
//---------------------------------------------
void reset(void){
	azzera();
	rispostaCorrente = NULL;
	stato = HOME;
}

//---------------------------------------------
void nuovoCarattere(const unsigned char carattere, Scenario *sim){
//controlla il timeout
	switch(stato){
	case HOME:
		
		indice = 0;	
		dimensione = 0;
		rispostaCorrente = 0;

		nuovo(carattere);
		if (carattere == START || carattere == DLE){
			if (carattere == START)
				stato = ATTESA_DIMENSIONE;
			else
				stato = IN_SOLLECITO;
		}
		else
			errore(1);

		break;

	case ATTESA_DIMENSIONE:			// è arrivato il primo ESC, aspetta per sapere la dimensione del pacchetto
		nuovo(carattere);
		if (carattere >= MAXDIM+6) // potrebbe essere >250 quindi non confronta dimensione
			errore(2);
		else{
			dimensione = carattere + 6; // esc dim dim esc sa da
			stato = IN_RICHIESTA;
		}
		break;

	case IN_RICHIESTA:				// conosce la dimensione, aspetta che sia completa
		nuovo(carattere);
		if (indice == dimensione) {
			if (carattere == SYN) {
				scegliRisposta(sim);
				stato = IN_SOLLECITO;
				indice = 0;
			}
			else
				errore(3);
		}
		break;
	
	case IN_SOLLECITO:
		dimensione = 6;
		nuovo(carattere);
		
		if (!indice && carattere != DLE)
			errore(4);
		else 
			if (indice == dimensione)
				if (carattere == SYN){
					if (rispostaCorrente)
						inviaRisposta(sim);
					else
						inviaConferma(0);
					stato = HOME;			// comunque
				}
				else
					errore(8);
		break;

	default:
		errore(5);

	}
}

//---------------------------------------------
// dopo lo stop, ping pong di solleciti e conferme
// quindi può arrivare un sollecito senza una risposta
// corrente. E' pericolo perchè non fa scattare il timeout del client

void inviaConferma(const unsigned char *risposta){

#ifdef WIN32
	DocumentaText::instance()->riga("Risposta: ");
#endif

	Serial.write(0xE5);
#ifdef WIN32
	DocumentaText::instance()->riga("Sollecito: ");
#endif
	rispostaCorrente = risposta;
}
//---------------------------------------------
void scegliRisposta(Scenario *sim){
	// cerca negoziazione dimensione PDU
	if ((dimensione == 0x15+6) && (buffer[dimensione-2] == 0xEB))
		inviaConferma(negozioRisposta);
	else
		// cerca richiesta di run
		if ((dimensione == 0x21+6) && (buffer[dimensione-2] == 0xBA))
			inviaConferma(runRisposta);
		else
			// cerca richiesta di stop
			if ((dimensione == 0x1D+6) && (buffer[dimensione-2] == 0xBA))
				inviaConferma(stopRisposta);
			else
				// cerca richiesta di lettura
				if ((buffer[8] == 0x01) && (buffer[6] == 0x6C)){
					// modifica il valore delle uscite
					//scriviBitUscita(_uscite);
					inviaConferma(letturaRisposta);
				}
				else
					// cerca richiesta di scrittura forzata
					if ((buffer[8] == 0x07) && (buffer[6] == 0x6C)) {
						// carica i valori degli ingressi
						sim->cambiaBitIngresso(&buffer[45], sim->numeroByteIngresso);
						inviaConferma(scritturaRisposta);
					}
					else
						// cerca richiesta di scrittura dopo lo stop
						if ((buffer[8] == 0x07) && (buffer[6] == 0x7C)) {
							// risposta e basta
							inviaConferma(dopoRisposta);
						}
						else
							errore(7);

}
//----------------------------------------------------
/*
ProtocolloPPI(PLC *plc_, Canale *canale_){
	_plc = plc_;
	_canale = canale_;
}*/
//----------------------------------------------------
/*
void leggiBitIngresso(const unsigned char *forzata){
	const unsigned char mask = 1;
	for(int i=0; i<8; i++){
		if (forzata[45] & mask << i)
			_plc->I0[i] = true;
		else
			_plc->I0[i] = false;
	}
}*/
//----------------------------------------------------
/*
void scriviBitUscita(unsigned char *uscite){
	const unsigned char mask = 1;
	*uscite = 0;
	for(int i=0; i<8; i++){
		if (_plc->Q0[i] = true)
			*uscite |= mask << i;
	}
}*/
//----------------------------------------------------
