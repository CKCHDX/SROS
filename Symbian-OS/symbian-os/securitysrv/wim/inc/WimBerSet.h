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
* Description:  An implementation of a CWimBerSet, which holds CWimBer objects
*
*/



#ifndef WIMBERSET_H
#define WIMBERSET_H


//INCLUDES
#include "WimBerConsts.h"
#include <e32base.h>
#include <bigint.h>



//FORWARD DECLARATIONS
class CWimBer;

/*
* Class CWimBerSet contains set of CWimBer objects
* 
*  @lib   WimBer
*  @since Series60 2.6 
*/

class CWimBerSet: public CArrayPtrSeg<CWimBer>
    {
                 
    public:

        /*
        * Two-phased constructor
        * @param aGranularity -Initial size of the created set
        * @return CWimBerSet* -initialized object which is inserted into 
        * cleanupstack
        */
        IMPORT_C static CWimBerSet* NewLC( const TInt aGranularity );

        /*
        * Two-phased constructor
        * @param aGranularity -Initial size of the created set
        * @return CWimBerSet* -initialized object
        */
        IMPORT_C static CWimBerSet* NewL( const TInt aGranularity );
        
        /* 
        * Destructor. Allocated memory is released
        */
        IMPORT_C  virtual ~CWimBerSet();

        /*   
        * Function creates CWimBer integer object.
        * @param aValue. -Integer for object creation
        * @return void
        */
        IMPORT_C void CreateIntL( const TInt aValue );
        
        /*
        * Funtion creates long CWimBer integer object.
        * @param aValue. -Integer for object creation.
        * @return void
        */
        IMPORT_C void CreateLongIntL( RInteger& aValue );

        /* 
        * Function creates CWimBer octet string object. The octet string is an 
        * arbitrarily long binary value.
        * @param aString -String for octet creation
        * @return void
        */
        IMPORT_C void CreateOctetL( TDesC8& aString );
        
        /* 
        * Function creates CWimBer NULL object ( 0x05 0x00 ).
        * @return void
        */
        IMPORT_C void CreateNullL();

        /* 
        * Function creates CWimBer object identifier object.
        * @param aString -object identifier content
        * @return void
        */
        IMPORT_C void CreateOidL( TDesC8& aString );


        /* 
        * Function creates CWimBer printable string object.
        * The printable string is defined to only contain
        * the characters A-Z, a-z, 0-9, space,
        * and the punctuation characters ()-+=:',./?.
        * @param aString -printable string
        * @return void
        */
        IMPORT_C void CreatePrintableL( TDesC8& aString );

        /* 
        * Function creates CWimBer IA5 string object.
        * IA5 ( International Alphabet 5 )
        * is equivalent to US-ASCII.
        * @param aString -IA5 string
        * @return void
        */
        IMPORT_C void CreateIA5L( TDesC8& aString );

        /*
        * Function creates CWimBer universal time object.
        * Note this value only represents years using two digits.
        * @param aString -universal time
        * @return void
        */
        IMPORT_C void CreateUTCL( TDesC8& aString );

        /* 
        * Append start of sequence ( 30 xx, xx = length )
        * @param aDefinite. -ETrue for definite length sequence, EFalse for 
        * indefinite length. 
        * @return void
        */
        IMPORT_C void CreateSeqStartL( TBool aDefinite );

        /*
        * Append start of set ( 31 xx, xx = length )
        * @param aDefinite. -ETrue for definite length sequence, EFalse for 
        * indefinite length. 
        * @return void
        */
        IMPORT_C void CreateSetStartL( TBool aDefinite );

        /*  
        * Append start of the constructed.
        * @param aTag -object type ( tag )
        * @param aDefinite -ETrue for definite length sequence, EFalse for 
        * indefinite length. 
        * @return void
        */
        IMPORT_C void CreateConstructedStartL( TBerTag aTag, TBool aDefinite );

        /*
        * Function closes last constructed object. 
        * @return void
        */
        IMPORT_C void CreateEndL();

        /*  
        * Used to create a CWimBer object
        * from buffer, which already contains a full CWimBer
        * encoded object.
        * @param aBuffer -contains full CWimBer encoded object.
        * @return void
        */
        IMPORT_C void CreateBEREncodedObjectL( TDesC8& aBuffer );

        /* 
        * Function writes all created CWimBer objects (which are 
        * Appended to set) to descriptor. Function also fixes all definite
        * lengths. 
        * @param aTarget -Descriptor where data is written. 
        * @return TInt -Error code(negative) or the number of WimBer objects
        *               (positive).
        */
        IMPORT_C TInt FlushL( HBufC8*& aTarget );

    public:

        /* 
        * Encode and write object identifier.
        * @param aString  -Object ID. e.g. _L8("1.2.3.4.5")
        * @param aOnlyLen - ETrue if You don't want to write object to iTarget
        * @return TInt    -Error code
        */
        TInt AppendObjectIdL( const TDesC8& aString, TBool aOnlyLen = EFalse );


    private:

        /* 
        * Write constructed type start.
        * @param aTag -type of constructed tag e.g. 04 for octetS.
        * @param aLength -Length or 0 for indefinite length
        * @return void
        */
        void AppendConstructedL( TBerTag aTag, TUint aLength = 0 );
        
        /* 
        * Close indefinite length. (= add 0x00 0x00) 
        * @return void
        */
        void CloseIndefinite(); 
        
        /* 
        * Encode and write boolean object. 
        * @param aBool -ETrue or EFalse
        * @return void
        */
        void AppendBoolL( TBool aBool );   
        
        /* 
        * Encode and write integer object.
        * @param aData -Integer to be written to data. 
        * @return void
        */
        void AppendIntL( TInt aData ); 

        /* 
        * Encode and write long integer object.
        * @param aData -Integer to be written to data. 
        * @return void
        */ 
        void AppendLongIntL( const RInteger* aData );
        
        /* 
        * Encode and write null object. (0x05 0x00)
        * @return void
        */
        void AppendNull();
        
        
        /* 
        * Encode and write string to iTarget
        * @param aTag - Object type ( tag )
        * @param aString - String to be written
        * @void
        */
        void AppendStringL( TBerTag aTag, const TDesC8& aString );
        
        /* 
        * Write CWimBer encoded object to iTarget.
        * Note that this function doesn't add any tags
        * etc. it trusts that the given object already
        * is a whole CWimBer encoded object.
        * @param aString -Buffer containing CWimBer encoded object
        * @return void
        */
        void AppendBerEncodedObject( const TDesC8& aString );

    private:

        /* 
        * Default constructor
        * @param aGranularity -Granularity for this array 
        */
        CWimBerSet( const TInt aGranularity ); 
        
    private:  

        // Descriptor for data. Not owned.
        HBufC8* iTarget;
        
        // Nesting level of object. ( encoding ).
        TInt iLevel;
        
        // Max level which is used in encoding.
        TInt iMaxLevel;

    };


#endif //WIMBERSET_H
