/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Definitions for Ber objects
*
*/

#ifndef WIMBERCONSTS_H
#define WIMBERCONSTS_H

#include <e32base.h>

//  DATA TYPES
typedef TUint8 TBerTag;

// BER types.
const TBerTag KBerEndOfContent  = 0x00;     // End of contents tag
const TBerTag KBerBoolean       = 0x01;     // Boolean tag
const TBerTag KBerInteger       = 0x02;     // Integer tag
const TBerTag KBerBitString     = 0x03;     // Bit string tag
const TBerTag KBerOctetString   = 0x04;     // Octet string tag
const TBerTag KBerNull          = 0x05;     // NULL tag
const TBerTag KBerOid           = 0x06;     // Object identifier tag
const TBerTag KBerNumS          = 0x12;     // Numeric string       
const TBerTag KBerPrS           = 0x13;     // Printable string tag
const TBerTag KBerT61S          = 0x14;     // T61 string tag
const TBerTag KBerVideoS        = 0x15;     // Video string tag              
const TBerTag KBerIA5S          = 0x16;     // IA5 string tag
const TBerTag KBerUtc           = 0x17;     // UTC time tag
const TBerTag KBerGenTime       = 0x18;     // Generalized Time tag                 
const TBerTag KBerGraphS        = 0x19;     // Graphics string tag          
const TBerTag KBerVisibleS      = 0x1A;     // Visible string                   
const TBerTag KBerGeneralS      = 0x1B;     // Generalised string   
const TBerTag KBerBmpS          = 0x1E;     // Bmp string
const TBerTag KBerSeq           = 0x30;     // Sequence tag
const TBerTag KBerSet           = 0x31;     // Set tag

const TBerTag KBerUnknown       = 0xff;     // Unknown tag

const TBerTag KBerEncodedObject = 0xfe;     // Tag for ready-made
                                            // BER encoded objects

const TBerTag KBerLongLengthBit = 0x80;     // Long length bit
const TBerTag KBerConstructedBit= 0x20;     // Constructed bit
const TBerTag KBerConstructed   = 0x20;

const TBerTag KBerImplicit = 0x80;   // Implicit tag
const TBerTag KBerExplicit = 0x80;   // Explicit tag

const TBerTag KBerImplicitConstructed = 0xA0;   // Implicit constructed tag
const TBerTag KBerExplicitConstructed = 0xA0;   // Explicit constructed tag

const TBerTag KBerBooleanTrue   = 0xFF;     // Boolean true value   (DER)
const TBerTag KBerBooleanFalse  = 0x00;     // Boolean false value  (DER)
const TBerTag KBerNullContent   = 0x00;     // Null objects content

const TBerTag KBerBooleanLen    = 0x03;     // Boolean object length
const TBerTag KBerNullLen       = 0x02;     // Null object length
const TBerTag KBerShortLen      = 0x02;     // Tag + (short) length byte
const TBerTag KBerIndefiniteLen = 0x04;     // Indefinite length (xx 80 00 00)

const TInt KReadBufMax          = 256;      // Length of buffer used
                                            // when reading from files.
const TUint KOpenAllLevels      = 255;      // OpenL(....) function
const TInt KOctetWidth          = 8;        
const TInt KObjectIDDot         = 1;        // object id encoding
const TInt KObjectIDFirstFactor = 4;        // object id encoding
const TInt KObjectIDSecondFactor = 10;      // object id encoding

const TInt KIntBufSize          = 6;        // Integer encoding buffer size 



const TInt KMaxLenBytes   = 4;  // Max amount of length bytes in BER encoded
                            // object.

const TInt KFlushTargetMaxSize = 2048;
const TInt KBerMask80 = 0x80;
const TInt KBerMask81Negative = -0x81;
const TInt KBerMask03 = 0x03;
const TInt KBerMask8000 = 0x8000;
const TInt KBerMask8001Negative = -0x8001;
const TInt KBerMask04 = 0x04;
const TInt KBerMask800000 = 0x800000;
const TInt KBerMask800001Negative = -0x800001;
const TInt KBerMask05 = 0x05;
const TInt KBerMask06 = 0x06;
const TInt KBerMask8 = 8;


#endif 
