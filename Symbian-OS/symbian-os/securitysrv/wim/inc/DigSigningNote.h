/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for Digital Signing Note
*
*/



#ifndef MDIGSIGNINGNOTE_H
#define MDIGSIGNINGNOTE_H

//  INCLUDES
#include <e32base.h>
#include <secdlg.h>

// CLASS DECLARATION

/**
*  Digital Signing Note interface
*  
*  @since Series60 2.6
*/
class MDigSigningNote 
    {
    public:
        
        /**
        * TNoteType defines different possible note types.
        */
        enum TNoteType
            {
            ESignatureRequested = 0xFF00,   // Digital signature requested
            ESignatureDone,                 // Digital signature done.
            ESigningCancelled,              // Digital signing cancelled   
            ENoMatchCert,                   // No matchig certificate found
            ENoSecurityModule,              // Security Module unvailable
            EInternalError                  // Error note in any other case
            };

    public: // New functions
        
        /**
        * Informs the user during digital signing.
        * @param aNoteType Identifies the note type.
        * @param aStatus KErrNone or KErrNotSupported
        * @return void
        */
        virtual void ShowNote( MDigSigningNote::TNoteType aNoteType,
                               TRequestStatus& aStatus ) = 0;
        
        /**
        * Saves receipt to Notepad if receipt saving is on in .ini-file and
        * informs user about this.
        * @param aSignedText Signed text in readable form.
        * @param aStatus KErrNone or KErrNotSupported
        * @return void
        */
        virtual void SaveSignedText( const TDesC& aSignedText,
                                     TRequestStatus& aStatus ) = 0;
        
        /**
        * Shows instructions how to unblock PIN.
        * @param aLabel Label of the PIN.
        * @param aStatus KErrNone or KErrNotSupported
        * @return void
        */
        virtual void ShowPINBlockedInfo( const TPINLabel& aLabel,
                                         TRequestStatus& aStatus ) = 0;

        /**
        * Release is called to free resources of the MDigSigningNote class
        */
        virtual void Release() = 0;

    protected: 
        
        // Destructor
        inline virtual ~MDigSigningNote() = 0;

    };

inline MDigSigningNote::~MDigSigningNote(){}

/**
* Factory for creating the relevant concrete subclass
* of the digital signing note.
*/
class DigSigningNoteFactory
    {
    public:
    
    /**
     * Creates an instance of a subclass of MDigSigningNote. 
     * Implement to create the appropriate security dialog.
     * 
     * @param returns an object that implements MDigSigningNote methods
     */
    IMPORT_C static MDigSigningNote* CreateNoteL(); 
    
    };

#endif      // MDIGSIGNINGNOTE_H   
            
// End of File
