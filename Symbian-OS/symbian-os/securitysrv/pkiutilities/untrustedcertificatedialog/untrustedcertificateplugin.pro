#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:  Device dialog plugin that shows untrusted certificate
#               dialog for TLS server authentication failure errors.
#

TEMPLATE = lib
TARGET = untrustedcertdialog
CONFIG += hb plugin

INCLUDEPATH += .
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
DEPENDPATH += .

HEADERS += inc/untrustedcertificateplugin.h \
    inc/untrustedcertificatedefinitions.h \
    inc/untrustedcertificatedialog.h \
    inc/untrustedcertificatewidget.h \
    inc/untrustedcertificateinfobase.h

SOURCES += src/untrustedcertificateplugin.cpp \
    src/untrustedcertificatedialog.cpp \
    src/untrustedcertificatewidget.cpp \
    src/untrustedcertificateinfobase.cpp

symbian: {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.UID3 = 0x20030000

    HEADERS += inc/untrustedcertificateinfo_symbian.h
    SOURCES += src/untrustedcertificatewidget_symbian.cpp \
        src/untrustedcertificateinfo_symbian.cpp
    
    LIBS += -lcrypto -lx509 -lx500 -lhash -lX509CertNameParser
    
    pluginstub.sources = untrustedcertdialog.dll
    pluginstub.path = /resource/plugins/devicedialogs
    DEPLOYMENT += pluginstub
}

BLD_INF_RULES.prj_exports += \
    "$${LITERAL_HASH}include <platform_paths.hrh>" \
    "qmakepluginstubs/untrustedcertdialog.qtplugin /epoc32/data/z/pluginstub/untrustedcertdialog.qtplugin" \
    "rom/untrustedcertdialog.iby CORE_MW_LAYER_IBY_EXPORT_PATH(untrustedcertdialog.iby)" \
    "rom/untrustedcertdialog_resources.iby LANGUAGE_MW_LAYER_IBY_EXPORT_PATH(untrustedcertdialog_resources.iby)"

