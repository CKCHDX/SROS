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
* Description: 
*
*/


// INCLUDE FILES
#include    "TCMSTimeUtil.h"
#include 	<asn1dec.h>

// CONSTANTS
// UTC formatting string
_LIT( KUTCFormatString, "%*Y%3%M%2%D%1%H%T%SZ" );
const TUint8 KFormatLength = 13;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TCMSTimeUtil::ConvertToEncoderLC
// Converts TTime to ASN encoder base
// -----------------------------------------------------------------------------
//
CASN1EncBase* TCMSTimeUtil::ConvertToEncoderLC( const TTime& aTime )
    {
	CASN1EncBase* time = NULL;
	//    Dates between 1 January 1950 and 31 December 2049 (inclusive) must be
	//    encoded as UTCTime.  Any dates with year values before 1950 or after
	//    2049 must be encoded as GeneralizedTime.
	if( aTime.DateTime().Year() >= 1950 &&
		aTime.DateTime().Year() <= 2049 )
		{
		TBuf<KFormatLength> dateString;
		// Formatting for UTC time
		aTime.FormatL( dateString, KUTCFormatString );
		TBuf8<KFormatLength> timeDes;
		timeDes.Copy( dateString );
		time = CASN1EncOctetString::NewLC( timeDes );
		// change type to UTCTime
		time->SetTag( EASN1UTCTime, EUniversal );
		}
	else
		{
		time = CASN1EncGeneralizedTime::NewLC( aTime );
		}
	return time;
    }

// -----------------------------------------------------------------------------
// TCMSTimeUtil::ConvertToTimeL
// Converts DER encoded source to TTime
// -----------------------------------------------------------------------------
//
TTime TCMSTimeUtil::ConvertToTimeL( const TDesC8& aSource )
	{
	TASN1DecGeneric time( aSource );
	time.InitL();
	TTime returnValue = 0;
	if( time.Tag() == EASN1GeneralizedTime )
		{
		TASN1DecGeneralizedTime genTime;
		returnValue = genTime.DecodeDERL( time );
		}
	else if( time.Tag() == EASN1UTCTime )
		{
		TASN1DecUTCTime utcTime;
		returnValue = utcTime.DecodeDERL( time );
		}
	else
		{
		User::Leave( KErrArgument );
		}
	return returnValue;
	}
//  End of File
