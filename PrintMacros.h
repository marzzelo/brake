/*
 * PrintMacros.h
 *
 *  Created on: 25 oct. 2021
 *      Author: valdez
 */

#ifndef PRINTMACROS_H_
#define PRINTMACROS_H_

#define DEBUG 1
#define PDAQ  1

#if DEBUG
#define PRINT(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PRINTS(s) Serial.print(F(s))
#define PRINTX(x) Serial.print(x)
#define TPRINT(s, x) { Serial.print("\t"); Serial.print(F(s)); Serial.print(x); }
#define TPRINTS(s) { Serial.print("\t"); Serial.print(F(s)); }
#define TPRINTX(x) { Serial.print("\t"); Serial.print(x); }
#define NPRINT(s, x) { Serial.print("\n"); Serial.print(F(s)); Serial.print(x); }
#define NPRINTS(s) { Serial.print("\n"); Serial.print(F(s)); }
#define NPRINTX(x) { Serial.print("\n"); Serial.print(x); }
#else
#define PRINT(s, x)
#define PRINTS(s)
#define PRINTX(x)
#define TPRINT(s, x)
#define TPRINTS(s)
#define TPRINTX(x)
#define NPRINT(s, x)
#define NPRINTS(s)
#define NPRINTX(x)
#endif

#if PDAQ
#define PDAQ(s, x) { Serial.print(F(s)); Serial.print(x); }
#define PDAQS(s) Serial.print(F(s))
#define PDAQX(x) Serial.print(x)
#define TPDAQ(s, x) { Serial.print("\t"); Serial.print(F(s)); Serial.print(x); }
#define TPDAQS(s) { Serial.print("\t"); Serial.print(F(s)); }
#define TPDAQX(x) { Serial.print("\t"); Serial.print(x); }
#define NPDAQ(s, x) { Serial.print("\n"); Serial.print(F(s)); Serial.print(x); }
#define NPDAQS(s) { Serial.print("\n"); Serial.print(F(s)); }
#define NPDAQX(x) { Serial.print("\n"); Serial.print(x); }

//#define PDAQ(s, x) Serial << (F(s)) << _FLOAT(x, 2)
//#define PDAQS(s) Serial << (F(s))
//#define PDAQX(x) Serial << _FLOAT(x, 2)
//#define TPDAQ(s, x) Serial << "\t" << (F(s)) << _FLOAT(x, 2)
//#define TPDAQS(s) Serial << "\t" << (F(s))
//#define TPDAQX(x) Serial << "\t" << _FLOAT(x, 2)
//#define NPDAQ(s, x) Serial << "\n" << (F(s)) << _FLOAT(x, 2)
//#define NPDAQS(s) Serial << "\n" << (F(s))
//#define NPDAQX(x) Serial << "\n" << _FLOAT(x, 2)


#else
#define PDAQ(s, x)
#define PDAQS(s)
#define PDAQX(x)
#define TPDAQ(s, x)
#define TPDAQS(s)
#define TPDAQX(x)
#define NPDAQ(s, x)
#define NPDAQS(s)
#define NPDAQX(x)
#endif


#endif /* PRINTMACROS_H_ */
