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
* Description: Utility to perform GBA bootstrapping operation.
*/

#include "gbautilitybody.h"

GbaUtility::GbaUtility(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_SYMBIAN
    QT_TRAP_THROWING(gbaUtilityBody = CGbaUtilityBody::NewL(this));
#else
    gbaUtilityBody = CGbaUtilityBody::NewL(this);
#endif
}

GbaUtility::~GbaUtility()
{
    delete gbaUtilityBody;
}
		
GbaUtility::GbaErrorStatus GbaUtility::bootstrap(const GbaBootstrapInputData *gbainputdata, GbaBootstrapOutputData *gbaoutputdata)
{
    return gbaUtilityBody->bootstrap(gbainputdata, gbaoutputdata);
}

void GbaUtility::cancelBootstrap()
{
    gbaUtilityBody->cancelBootstrap();
}

GbaUtility::GbaErrorStatus GbaUtility::setBsfAddress(const QString &bsfaddress)
{
    return gbaUtilityBody->setBsfAddress(bsfaddress);
}

