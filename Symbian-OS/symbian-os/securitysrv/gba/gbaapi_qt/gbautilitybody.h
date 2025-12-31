/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: GBA utility body to perform GBA bootstrapping operation.
*/

#ifndef GBAUTILITYBODY_H
#define GBAUTILITYBODY_H
#include <e32base.h>
#include <gbautility.h>
#include <gbautility_qt.h>

// CLASS DECLARATION  
class CGbaUtilityBody : public CBase,
                        public MGbaObserver
{
public:
    
    static CGbaUtilityBody* NewL(GbaUtility *aGbautility);
    
    //Destructor
    virtual ~CGbaUtilityBody();
    
    /*Bootstapping function*/
    GbaUtility::GbaErrorStatus bootstrap(const GbaBootstrapInputData *gbainputdata, GbaBootstrapOutputData *gbaoutputdata);
    
    //Cancel any outstanding bootstapping process
    void cancelBootstrap();
    
    //set the bsf address in the local cache,which would be used while bootstrapping
    GbaUtility::GbaErrorStatus setBsfAddress(const QString &aBsfaddress);
    
    //MGbaObserver
    void BootstrapComplete(TInt aError);
private:
    // Constructor
    CGbaUtilityBody(GbaUtility *aGbautility);
    
    void ConstructL();
    
private:
    CGbaUtility *iGbaUtility;
    TGBABootstrapInputParams iInputGbaParameters;
    TGBABootstrapOutputParams iOutputGbaParameters;
    GbaBootstrapOutputData *iGbaOutPutData;
    GbaUtility *iQtGbautility;
};

#endif //GBAUTILITYBODY_H
//EOF
