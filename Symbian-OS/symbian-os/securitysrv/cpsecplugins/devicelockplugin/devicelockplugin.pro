#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#

TEMPLATE = lib
TARGET = devicelockplugin

CONFIG += hb plugin		  

LIBS +=  -lsecuinotificationdialogplugin \
		 -llockclient \
		 -lautolockuseractivityservice \
		 -lIndicatorAutolockPlugin \
		 -lsecui \
		 -letelmm \
		 -letel \
		 -lcustomapi \
		 -lrlocksettings \
		 -lcentralrepository \
		 -lRLockSettings \
		 -lflogger
TRANSLATIONS += devicelocking.ts
include ( ../cpplugincommon.pri )
include ( devicelockplugin.pri )
include ( rom/devicelockplugin_rom.pri )

symbian { 
    TARGET.UID3 = 0X2002E683
    
    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE
    
}