//=========================================================
// src/V3_I2CSLAVE_main.c: generated by Hardware Configurator
//
// This file will be updated when saving a document.
// leave the sections inside the "$[...]" comment tags alone
// or they will be overwritten!!
//=========================================================

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <SI_EFM8SB1_Register_Enums.h>                  // SFR declarations
#include "InitDevice.h"
// $[Generated Includes]
// [Generated Includes]$
#include "global.h"
#include "smb_0.h"

SI_SEGMENT_VARIABLE(cmd[5], uint8_t, SI_SEG_XDATA);
SI_SEGMENT_VARIABLE(response, uint8_t, SI_SEG_XDATA);

#define CMD_VER 0x01  // return FW version
#define CMD_SPK 0x02  // control Piezo beeps
#define CMD_HAP 0x03  // control Haptic vibration timing
#define CMD_GET 0x04  // Get bits of I/O
#define CMD_SET 0x05  // SET I/O bits - Currently only EN5V is supported Update as BIOSENSOR is defined
#define CMD_LAST 0x06

#define CMD_ACK 0xAA
#define CMD_ERR 0x55

#define FW_VER 03

U8 tick = false;
U16 seconds = 0;

#define TICSPERSEC 100

U8 tick_count = 0;
U8 spktime = 0;
U8 haptime = 0;

//Haptic drive levels
#define HAP_BIT 0x01
#define HAP_PORT P1

#define SET_PORT P1
#define SET_BITS 0x2A //Currently only EN5V, MFIO & RESET are supported. MFIO is open collector, high = input.

//-----------------------------------------------------------------------------
// SiLabs_Startup() Routine
// ----------------------------------------------------------------------------
// This function is called immediately after reset, before the initialization
// code is run in SILABS_STARTUP.A51 (which runs before main() ). This is a
// useful place to disable the watchdog timer, which is enable by default
// and may trigger before main() in some instances.
//-----------------------------------------------------------------------------
void SiLabs_Startup(void) {
// $[SiLabs Startup]
// [SiLabs Startup]$
}

void SMB0_errorCb(SMB0_TransferError_t error) {
}

void SMB0_transferCompleteCb() {

}

void HapticOn(U8 time) {

	HAP_PORT |= HAP_BIT;
	haptime = time;
}

void HapticOff(void) {

	HAP_PORT &= ~(HAP_BIT);
}

void SpeakerOn(U8 freq, U8 time) {
	static U8 first = 0;

	PCA0CN0 &= ~PCA0CN0_CR__RUN;
	PCA0L = 0;

	PCA0CPL0 = 0;
	PCA0CPL1 = 0;
	if (first == 0) {
		first = 1;
		PCA0CPL1 = 1;
	}
	PCA0CPH0 = freq;
	PCA0CPH1 = freq;

	PCA0CN0 |= PCA0CN0_CR__RUN;
	spktime = time;
	XBR1 |= XBR1_PCA0ME__CEX0_CEX1;
}

void SpeakerOff(void) {

	XBR1 &= ~0x03;
	PCA0CN0 &= ~PCA0CN0_CR__RUN;

}

//This is called whenever the master finishes a write of data.
void SMB0_commandReceivedCb() {
	//static uint8_t response = 0;

// Get FW Version
	if (cmd[0] == CMD_VER) {
		response = FW_VER;
	}

// Buzzer actuation
	if (cmd[0] == CMD_SPK) {
		SpeakerOn(cmd[1], cmd[2]);
		response = CMD_ACK;
	}

// Haptic Motor actuation
	if (cmd[0] == CMD_HAP) {
		HapticOn (cmd[2]);
		response = CMD_ACK;
	}

// GET I/O PINS
	if (cmd[0] == CMD_GET) {

		response = P0 & 0xF0;  // read upper bits of P0
		response |= ((P1 & 0xC0) >> 6);
	}

// SET I/O PINS	
	if (cmd[0] == CMD_SET) {
		SET_PORT = (~SET_BITS & SET_PORT) | ( SET_BITS & cmd[2]); // Preserve other I/O on SET_PORT
		response = CMD_ACK;
	}

	if (cmd[0] >= CMD_LAST)
		response = CMD_ERR;

	//Inform driver we have single byte a response
	SMB0_sendResponse(&response, 1);

	//SCL held low during the execution of this function ensuring
	//  that data is ready before the master can attempt to read.
}
//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
int main(void) {

	// Call hardware initialization routine
	enter_DefaultMode_from_RESET();

#if 1
	// enable bias double for RTC
	RTC0ADR = RTC0XCN0;
	RTC0DAT |= RTC0XCN0_BIASX2__ENABLED;
	while ((RTC0ADR & RTC0ADR_BUSY__BMASK) == RTC0ADR_BUSY__SET)
		;
#endif

	//Set up SMB bus
	SMB0_init(SMB0_TIMER1, false);
	SMB0_initSlave(0x37, cmd, 3);

	//SpeakerOn(140, 100);

	while (1) {
// $[Generated Run-time code]
// [Generated Run-time code]$

		if (tick) {
			tick = false;
			if (tick_count++ == TICSPERSEC) {
				tick_count = 0;
				seconds++;

			}
		}

	}
}
