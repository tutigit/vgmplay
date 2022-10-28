#include <stdio.h>
#include <sys/ndrclock.h>
#include <sys/file.h>
#include <sys/path.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <ndrcomp/sysclock.h>
#include "../../nkc_common/nkc/nkc.h"


//extern clock_t _clock(void (*clock_fu)(void));
//#define TIME (_clock(NULL))
//#define HZ CLOCKS_PER_SEC


#define GDP_WAIT() asm volatile ("dc.w 0x0839, 0x0002, 0xFFFF, 0xFF70, 0x67F6"); /* wait: btst.b #$02, $FFFFFF70.l  beq.s wait */


void delay_ms(uint16_t ms)
{
	for (int i = 0; i < 500 * ms; ++i)
	{
		GDP_WAIT();
	};
	
	return;


	//clock_t ticks = ((clock_t)ms*200u)/1000u;
	//clock_t start = _clock(NULL);
	
	//iprintf("=> %ld %ld\r\n", (uint32_t)start, (uint32_t)ticks);

	//while ((_clock(NULL)-start) < ticks) {};
}


void delay_sample(uint16_t n)
{
	delay_ms(5);
}


void myhexdump(char *a, long l)
{
	//iprintf("\r\n");
	//iprintf("%ld\r\n", (uint32_t)a);
	//iprintf("%ld\r\n", (uint32_t)a >> 16);
	//iprintf("%ld\r\n", (uint32_t)a & 0xFFFF);
	//iprintf("Basisadresse: 0x%04X%04X\r\n", (uint16_t)((uint32_t)a >> 16), (uint16_t)((uint32_t)a & 0xFFFF));
	//iprintf("Laenge: %ld\r\n\r\n", l);

	char ch[100];

	char c;
	int block = 0;
	int spalte = 0;
	int zeile = 0;
	
	char s[20] = "";
	int si = 0;

	
	for (int i = 0; i < l; ++i)
	{
		if (block == 0 && spalte == 0)
		{
			iprintf("%04X%04X: ", (uint16_t)((uint32_t)(a+i) >> 16), (uint16_t)((uint32_t)(a+i) & 0xFFFF));
		}
	
		c = a[i];
		iprintf("%02X ", (uint8_t)c);
		s[si++] = ( c >= 32 && c <= 128 ? c : '?' );
	
		if (++spalte >= 4)
		{
			iprintf(" ");
			spalte = 0;
			
			if (++block >= 4)
			{
				s[si] = '\0';
				iprintf("%s\r\n", s);
				block = 0;
				si = 0;
				
				if (++zeile > 20)
				{
					zeile = 0;
					puts("Return - weiter\n\r");
					gets(ch); 
				}
			}
		}
	}
	
	if (block > 0 || spalte > 0)
	{
		s[si] = '\0';
		for (int i = si; i < 16; ++i)
		{
			iprintf("   ");
			
			if ((i+1)%4 == 0)
			{
				iprintf(" ");
			}
		}
		iprintf("%s\r\n", s);
	}
		
	return;
}


uint32_t extractuint32_t(char *a)
{
	uint32_t b1 = (uint32_t)a[0];
	uint32_t b2 = (uint32_t)a[1] <<  8;
	uint32_t b3 = (uint32_t)a[2] << 16;
	uint32_t b4 = (uint32_t)a[3] << 24;
	
	return b1 + b2 + b3 + b4;
}


int vgmcheck(char *a)
{
	iprintf("File identification: %c%c%c%c\r\n", (char)a[0], (char)a[1], (char)a[2], (char)a[3]);

	uint32_t eofoffset = extractuint32_t(a+4);
	iprintf("Eof offset: %ld\r\n", eofoffset);

	iprintf("Version: %d%d%d%d%d%d%d%d\r\n", a[11]>>4, a[11]&0xF, a[10]>>4, a[10]&0xF, a[9]>>4, a[9]&0xF, a[8]>>4, a[8]&0xF);

	uint32_t sn76489clock = extractuint32_t(a+12);
	iprintf("SN76489 clock: %ld\r\n", sn76489clock);
	
	uint32_t ym2413clock = extractuint32_t(a+16);
	iprintf("YM2413 clock: %ld\r\n", ym2413clock);

	uint32_t gd3offset = extractuint32_t(a+20);
	iprintf("GD3 offset: %ld\r\n", gd3offset);

	uint32_t totalnumbersamples = extractuint32_t(a+24);
	iprintf("Total # samples: %ld\r\n", totalnumbersamples);

	uint32_t loopoffset = extractuint32_t(a+28);
	iprintf("Loop offset: %ld\r\n", loopoffset);

	uint32_t loopnumbersamples = extractuint32_t(a+32);
	iprintf("Loop # samples: %ld\r\n", loopnumbersamples);



	uint32_t vgmdataoffset = extractuint32_t(a+52);
	iprintf("VGM data offset: %ld\r\n", vgmdataoffset);

	uint32_t ay8910clock = extractuint32_t(a+116);
	iprintf("AY8910 clock: %ld\r\n", ay8910clock);

	iprintf("AY8910 Chip Type: ");
	switch (a[120])
	{
		case 0:
			iprintf("0x00 - AY8910\r\n");
			break;
		case 1:
			iprintf("0x01 - AY8912\r\n");
			break;
		case 2:
			iprintf("0x02 - AY8913\r\n");
			break;
		case 3:
			iprintf("0x03 - AY8930\r\n");
			break;
		case 16:
			iprintf("0x10 - YM2149\r\n");
			break;
		case 17:
			iprintf("0x11 - YM3439\r\n");
			break;
		case 18:
			iprintf("0x12 - YMZ284\r\n");
			break;
		case 19:
			iprintf("0x13 - YMZ294\r\n");
			break;
		default:
			iprintf("???\r\n");
			break;
	}

	uint8_t ay8910flags = a[121];
	iprintf("AY8910 flags: %d\r\n", ay8910flags);

	uint8_t ym2203ay8910flags = a[122];
	iprintf("YM2203/AY8910 flags: %d\r\n", ym2203ay8910flags);

	uint8_t ym2608ay8910flags = a[123];
	iprintf("YM2608/AY8910 flags: %d\r\n", ym2608ay8910flags);


	if (vgmdataoffset > 0 && a[120] == 0)
	{
		// Das sieht nach einem Datenstrom für den AY8910 aus. Also die Adresse der Daten zurückgeben...
		return 52 + vgmdataoffset;
	}

	return -1;
}


/* Routinen, um die SOUND3 auf Adresse 0x50 und 0x51 anzusteuern */
/* ...work in progress... aus einem Beispiel von smed... */

#define BYTE_AT(adr) (*(unsigned char volatile *) adr)

#define SNDCTRL BYTE_AT(0xFFFFFF50) // Steuerport
#define SNDDATA BYTE_AT(0xFFFFFF51) // Datenport


void vgmplay(char *a, long l)
{
	//char ch[100];
	
	// Kanal A initialisieren
	SNDCTRL = 0;		// Adresse Tonperiode LSB Kanal A
	SNDDATA = 0x55;		// LSB Wert Tonperiode
	SNDCTRL = 1;		// Adresse Tonperiode MSB Kanal A
	SNDDATA = 0;		// MSB Wert Tonperiode
	SNDCTRL = 8;		// Amplitude Kanal A
	//SNDDATA = 15;		// Maximum
	SNDDATA = 10;		// Leiser...

	// Kanal B initialisieren
	SNDCTRL = 2;		// Adresse Tonperiode LSB Kanal B
	SNDDATA = 0x45;		// LSB Wert Tonperiode
	SNDCTRL = 3;		// Adresse Tonperiode MSB Kanal B
	SNDDATA = 0;		// MSB Wert Tonperiode
	SNDCTRL = 9;		// Amplitude Kanal B
	//SNDDATA = 15;		// Maximum
	SNDDATA = 10;		// Leiser...

	// Kanal C initialisieren
	SNDCTRL = 4;		// Adresse Tonperiode LSB Kanal C
	SNDDATA = 0x35;		// LSB Wert Tonperiode
	SNDCTRL = 5;		// Adresse Tonperiode MSB Kanal C
	SNDDATA = 0;		// MSB Wert Tonperiode
	SNDCTRL = 10;		// Amplitude Kanal C
	//SNDDATA = 15;		// Maximum
	SNDDATA = 10;		// Leiser...

/*
**	iprintf("Ton auf Kanal A\r\n");
**
**	SNDCTRL = 7;            // Adresse Freigabekanal
**	SNDDATA = 0b11111110;   // Kanal A
**
**	puts("Return - weiter\n\r");
**	gets(ch); 
**	
**	iprintf("Ton auf Kanal B\r\n");
**
**	SNDCTRL = 7;            // Adresse Freigabekanal
**	SNDDATA = 0b11111101;   // Kanal B
**
**	puts("Return - weiter\n\r");
**	gets(ch); 
**	
**	iprintf("Ton auf Kanal C\r\n");
**
**	SNDCTRL = 7;            // Adresse Freigabekanal
**	SNDDATA = 0b11111011;   // Kanal C
**
**	puts("Return - weiter\n\r");
**	gets(ch); 
**	
**	iprintf("Ton auf Kanal B + C\r\n");
**
**	SNDCTRL = 7;            // Adresse Freigabekanal
**	SNDDATA = 0b11111001;   // Kanal B + C
**
**	puts("Return - weiter\n\r");
**	gets(ch); 
**	
**	iprintf("Ton aus\r\n");
**
**	SNDCTRL = 7;            // Adresse Freigabekanal
**	SNDDATA = 0b11111111;   // kein Kanal
*/


	int i = 0;
	
	while (i < l)
	{
		switch ((uint8_t)a[i])
		{
			case 97: // 0x61 nn nn : Wait n samples, n can range from 0 to 65535 (approx 1.49 seconds).
				//iprintf("%05d: 0X61 %d %d\r\n", i, (uint8_t)a[i+1], (uint8_t)a[i+2]);
				delay_sample((uint8_t)a[i+1] * 256 + (uint8_t)a[i+2]);
				i += 3;
				break;
			
			case 98: // 0x62 : wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
				//iprintf("%05d: 0X62\r\n", i);
				delay_sample(735);
				i += 1;
				break;

			case 99: // 0x63 : wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
				//iprintf("%05d: 0X63\r\n", i);
				delay_sample(882);
				i += 1;
				break;

			case 102: // 0x66 : end of sound data
				//iprintf("%05d: 0X66\r\n", i);
				//return;
				i += 1;
				break;

			case 112: // 0x7n : wait n+1 samples, n can range from 0 to 15.
			case 113:
			case 114:
			case 115:
			case 116:
			case 117:
			case 118:
			case 119:
			case 120:
			case 121:
			case 122:
			case 123:
			case 124:
			case 125:
			case 126:
			case 127:
				//iprintf("%05d: %02X\r\n", i, (uint8_t)a[i]);
				delay_sample((uint8_t)a[i] + 1);
				i += 1;
				break;

			case 160: // 0xA0 aa dd : AY8910, write value dd to register aa
				//iprintf("%05d: 0XA0 %d %d\r\n", i, (uint8_t)a[i+1], (uint8_t)a[i+2]);
				SNDCTRL = (uint8_t)a[i+1];
				SNDDATA = (uint8_t)a[i+2];
				i += 3;
				break;
				
			default:
				iprintf("-> %d\r\n", (uint8_t)a[i]);
				i += 1;
				break;
		}
		
		//puts("Return - weiter\n\r");
		//gets(ch); 
	}


	// Ton aus
	SNDCTRL = 7;            // Adresse Freigabekanal
	SNDDATA = 0b11111111;   // kein Kanal

	return;
}


int main(int argc, char **argv, char **envp)
{
	char ch[100];

	if (argc != 2)
	{
		iprintf("Nutzung: VGMPLAY DATEINAME\r\n");
		return -1;
	}
	
	jdfcb_t myfcb = {0};
	uint8_t result = jd_fillfcb(&myfcb, argv[1]);
	//iprintf("FillFcb-Result: 0x%X filename:%s\r\n", result, myfcb.filename);

	if (result != 0)
	{
		iprintf("Fehler beim Finden der Datei %s!\r\n", argv[1]);
		return -1;
	}
	
	jdfile_info_t info __attribute__ ((aligned (4))) = {0};
	char * p_buf = NULL;
	result = jd_fileinfo(&myfcb, &info);
	//iprintf("Fileinfo-Result: 0x%X length: %u date:0x%lX, att:0x%X\r\n", result, (unsigned int)info.length, info.date, info.attribute);

	if (result != 0)
	{
		iprintf("Fehler beim Oeffnen der Datei %s!\r\n", argv[1]);
		return -1;
	}
	
	p_buf = malloc(info.length);
	p_buf[0] = '\0';
	result = jd_fileload(&myfcb, p_buf);
	//iprintf("Fileread-Result: 0x%X\r\n", result);

	if (result != 0)
	{
		iprintf("Fehler beim Laden der Datei %s!\r\n", argv[1]);
		return -1;
	}

	//puts("Return - weiter\n\r");
	//gets(ch); 
	
	int i = vgmcheck(p_buf);
	if (i < 0)
	{
		iprintf("Hm? Das sieht nach einem nicht kompatiblen Datenstrom aus :-(\r\n");
		return -1;
	}
	
	puts("Return - weiter\n\r");
	gets(ch); 


	// Den Datenstrom abspielen...
	//vgmplay(p_buf + i, 1024 * (unsigned int)info.length);
	vgmplay(p_buf + i, extractuint32_t(p_buf+4) - (i-8));


	puts("Return - weiter\n\r");
	gets(ch); 


	//myhexdump(p_buf, (long)info.length);
	myhexdump(p_buf+i, 256);


	puts("Return - weiter\n\r");
	gets(ch); 

	
	// Nikolaushaus
	gp_clearscreen();
	gp_moveto(100,  50);
	gp_drawto(100, 100);
	gp_drawto(150, 125);
	gp_drawto(200, 100);
	gp_drawto(200,  50);
	gp_drawto(100, 100);
	gp_drawto(200, 100);
	gp_drawto(100,  50);
	gp_drawto(200,  50);


	puts("Return - weiter\n\r");
	gets(ch); 
}
