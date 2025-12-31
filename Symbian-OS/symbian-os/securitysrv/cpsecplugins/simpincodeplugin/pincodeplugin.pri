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
# Description: cppincodeplugin source files
#

# Input
HEADERS += inc/*.h

SOURCES += src/cppincodeplugin.cpp \
           src/cppincodepluginview.cpp \
           src/seccodesettings.cpp \
           src/seccodeedit.cpp \
           src/seccodeeditdataformviewitem.cpp \
           src/seccodemodel.cpp
           
symbian : {
    SOURCES += src/seccodemodel_symbian.cpp
}

win32 : {
    SOURCES += src/seccodemodel_win.cpp
}