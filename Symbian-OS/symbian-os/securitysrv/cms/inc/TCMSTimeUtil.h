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


#ifndef TCMSTimeUtil_H
#define TCMSTimeUtil_H

//  INCLUDES
#include 	<asn1enc.h>

// CLASS DECLARATION
/**
*  This util is used for creating either
*  UTCTime or Generalized time from TTime
*  depending on date
*
*  Dates between 1 January 1950 and 31 December 2049 (inclusive) must be
*  encoded as UTCTime.  Any dates with year values before 1950 or after
*  2049 must be encoded as GeneralizedTime.
*
*  @lib cms.lib
*  @since 2.8
*/
class TCMSTimeUtil
    {
    public:  // Constructors and destructor
		/**
        * Conversion util
        * @param aTime to be converted
        * @return given TTime in as encoder
        */
        static CASN1EncBase* ConvertToEncoderLC( const TTime& aTime );
		
		/**
        * Conversion util
        * @param aSource DER encoded source to be converted
        * @return source converted to TTime
        */
		static TTime ConvertToTimeL( const TDesC8& aSource );
    };

#endif      // TCMSTimeUtil_H

// End of File
