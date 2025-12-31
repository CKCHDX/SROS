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
# Description:gba project file
#

TEMPLATE = lib

QT -= gui

CONFIG += dll

DEFINES += BUILD_QTGBA_DLL

TARGET = gbautility

LIBS += -lgba2

MOC_DIR = .

INCLUDEPATH += . \
               $$MW_LAYER_SYSTEMINCLUDE
								
DEPENDPATH  += $$INCLUDEPATH

HEADERS += gbautility_qt.h \
           gbauilityexport.h \
           gbautilitybody.h
			
SOURCES += gbautility_qt.cpp \
           gbautilitybody.cpp

#Symbian specific definitions
DEFINES += SYMBIAN
TARGET.UID2 = 0x10009D8D
TARGET.UID3 = 0x20031E53
TARGET.EPOCALLOWDLLDATA = 1
TARGET.CAPABILITY = CAP_CLIENT_DLL

SYMBIAN_PLATFORMS = DEFAULT


symbian*: { 
                MMP_RULES -= "OPTION_REPLACE ARMCC --export_all_vtbl -D__QT_NOEFFECTMACRO_DONOTUSE"
                }

