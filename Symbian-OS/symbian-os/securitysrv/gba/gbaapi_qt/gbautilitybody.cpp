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

#include "gbautilitybody.h"

CGbaUtilityBody* CGbaUtilityBody::NewL(GbaUtility* aGbautility)
{
    CGbaUtilityBody* self = new (ELeave) CGbaUtilityBody(aGbautility);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;   
}

void CGbaUtilityBody::ConstructL()
{
    iGbaUtility = CGbaUtility::NewL(*this);
}

CGbaUtilityBody::CGbaUtilityBody(GbaUtility* aGbautility) : iQtGbautility(aGbautility)
{
}

CGbaUtilityBody::~CGbaUtilityBody()
{
    delete iGbaUtility;
}

GbaUtility::GbaErrorStatus CGbaUtilityBody::bootstrap(const GbaBootstrapInputData* gbainputdata, GbaBootstrapOutputData* gbaoutputdata)
{
    iGbaOutPutData = gbaoutputdata;
    iInputGbaParameters.iNAFName.Copy(reinterpret_cast<const TUint8*>(gbainputdata->nafName.toAscii().data())
            , gbainputdata->nafName.toAscii().length());
    iInputGbaParameters.iFlags = gbainputdata->bootstrapFlag;
    iInputGbaParameters.iUICCLabel.Copy(reinterpret_cast<const TUint8*>(gbainputdata->uiccLabel.toAscii().data())
            , gbainputdata->uiccLabel.toAscii().length());
    iInputGbaParameters.iProtocolIdentifier.Copy(reinterpret_cast<const TUint8*>(gbainputdata->protocolIdentifier.toAscii().data())
            , gbainputdata->protocolIdentifier.toAscii().length());
    iInputGbaParameters.iAPID = gbainputdata->accessPoint;
    TInt error = iGbaUtility->Bootstrap( iInputGbaParameters, iOutputGbaParameters );
    if(error != KErrNone) {
        if (error == KErrInUse)
            return GbaUtility::GbaErrorInUse;
        else if (error == KErrGeneral)
            return GbaUtility::GbaErrorGeneral;
    }
    return GbaUtility::GbaNoError;
}

void CGbaUtilityBody::cancelBootstrap()
{
    iGbaUtility->CancelBootstrap();
}

GbaUtility::GbaErrorStatus CGbaUtilityBody::setBsfAddress(const QString& aBsfAddress)
{
    HBufC8*bsfAddress = HBufC8::NewL(aBsfAddress.toAscii().length());
    bsfAddress->Des().Copy(reinterpret_cast<const unsigned char*>(aBsfAddress.toAscii().data()), aBsfAddress.toAscii().length());
    TInt error = iGbaUtility->SetBSFAddress(bsfAddress->Des());
    delete bsfAddress;
    if (error != KErrNone) {
        if (error == KErrPermissionDenied)
            return GbaUtility::GbaErrorPermissionDenied;
        else
            return GbaUtility::GbaErrorGeneral;
    }
    return GbaUtility::GbaNoError;
}

void CGbaUtilityBody::BootstrapComplete(TInt aError)
{
    if (aError != KErrNone) {
        emit iQtGbautility->bootstrapCompleted(GbaUtility::GbaErrorBootstrapFailed);
        return;
    }
    // Output: B-TID
    iGbaOutPutData->bTid = QString::fromAscii(reinterpret_cast<const char*>(iOutputGbaParameters.iBTID.Ptr())
            ,iOutputGbaParameters.iBTID.Length());
    // Output: Ks_NAFs
    QByteArray ksNaf;
    ksNaf.append(reinterpret_cast<const char*>(iOutputGbaParameters.iKNAF.Ptr()));
    iGbaOutPutData->ksNaf = ksNaf;
    // Output: lifetime
    QTime time(iOutputGbaParameters.iLifetime.DateTime().Hour()
                ,iOutputGbaParameters.iLifetime.DateTime().Minute()
                ,iOutputGbaParameters.iLifetime.DateTime().Second()
                ,0/*milli seconds component*/);
    QDate date(iOutputGbaParameters.iLifetime.DateTime().Year()
                ,iOutputGbaParameters.iLifetime.DateTime().Month()
                ,iOutputGbaParameters.iLifetime.DateTime().Day());
    iGbaOutPutData->lifetime.setDate(date);
    iGbaOutPutData->lifetime.setTime(time);
    // Output: IMPI
    iGbaOutPutData->impi = QString::fromAscii(reinterpret_cast<const char*>(iOutputGbaParameters.iKIMPI.Ptr())
            ,iOutputGbaParameters.iKIMPI.Length());
    // Output: GBA run-type
    switch (iOutputGbaParameters.iGbaRunType) {
    case E2GGBA:
        iGbaOutPutData->gbaRunType = RunType2gGba;
        break;
    case E3GGBAME:
        iGbaOutPutData->gbaRunType = RunType3gGbaMe;
        break;
    case EGBAU:
        iGbaOutPutData->gbaRunType = RunType3gGbaU;
        break;
    case ENoType:
        iGbaOutPutData->gbaRunType = RunTypeNone;
        break;
    default:
        break;
    }
    // Output: type of UICC application used
    iGbaOutPutData->uiccLabel = QString::fromAscii(reinterpret_cast<const char*>(iOutputGbaParameters.iOutputUICCLabel.Ptr())
            ,iOutputGbaParameters.iOutputUICCLabel.Length());
    emit iQtGbautility->bootstrapCompleted(GbaUtility::GbaNoError);
}

