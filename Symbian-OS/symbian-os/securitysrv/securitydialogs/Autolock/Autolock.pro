#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
# All rights reserved.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, version 2.1 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, 
# see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
#
# Description:
#

TEMPLATE=app
TARGET=Autolock

CONFIG += service
CONFIG += Hb

CONFIG += mobility
MOBILITY = publishsubscribe

#TRANSLATIONS = autolock.ts

XQSERVICE_ROOT=../..
#include(../../xqservicebase.pri)
include(src/Autolock.pri)

LIBS+=-lxqservice -lxqserviceutil -lflogger
LIBS += -L../../../../../bin/release -lautolockuseractivityservice
LIBS += -lsecui -letelmm -letel -lcustomapi -lcentralrepository
LIBS += -lcone -lws32 -lkeylockpolicyapi
LIBS += -lpower_save_display_mode
LIBS += -ltstaskmonitorclient		# for TsTaskSettings
LIBS += -lavkon									# for KeySounds
LIBS += -lapgrfx								# for CApaWindowGroupName
LIBS += -lscpclient							# SCP server         

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

libFiles.sources = Autolock.exe 
#libFiles.path = "!:\sys\bin"
DEPLOYMENT += libFiles

RESOURCES += Autolock.qrc

symbian*: {
				TARGET.CAPABILITY = CAP_APPLICATION
				TARGET.UID3 = 0x100059B5
				crmlFiles.sources = ./qcrml/autolock.qcrml
				crmlFiles.sources += ./qcrml/callstate.qcrml
				crmlFiles.sources += ./qcrml/dismissdialog.qcrml
				crmlFiles.sources += ./qcrml/grip.qcrml
				crmlFiles.sources += ./qcrml/keyguard.qcrml
				crmlFiles.sources += ./qcrml/lights.qcrml
				crmlFiles.sources += ./qcrml/profile.qcrml
				crmlFiles.sources += ./qcrml/screensaver.qcrml
				crmlFiles.sources += ./qcrml/securitysettings.qcrml
				crmlFiles.sources += ./qcrml/TestCode.qcrml
				
				crmlFiles.path = /resource/qt/crml
				DEPLOYMENT += crmlFiles
}

BLD_INF_RULES.prj_exports += "./rom/Autolock.iby            CORE_APP_LAYER_IBY_EXPORT_PATH(Autolock.iby)"
BLD_INF_RULES.prj_exports += "./conf/Autolock.confml        MW_LAYER_CONFML(Autolock.confml)"
BLD_INF_RULES.prj_exports += "./conf/lock.confml        MW_LAYER_CONFML(lock.confml)"
# remove CI_autolock.confml because CI already exports it
# BLD_INF_RULES.prj_exports += "./conf/CI_Autolock.confml     MW_LAYER_CONFML(CI_Autolock.confml)"
BLD_INF_RULES.prj_exports += "./rom/AutolockSrv.iby         CORE_APP_LAYER_IBY_EXPORT_PATH(AutolockSrv.iby)"
BLD_INF_RULES.prj_exports += "./PubSub/SecurityUIsPrivatePSKeys.h |../../inc/securityuisprivatepskeys.h"

BLD_INF_RULES.prj_exports += "./conf/lock_10283322.crml 	     MW_LAYER_CRML(lock_10283322.crml)"

symbian:MMP_RULES += SMPSAFE
