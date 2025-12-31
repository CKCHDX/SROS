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
# Description:  Advanced security settings (control panel plugin)
#

TEMPLATE = lib
TARGET = cpadvancedsecplugin
CONFIG += hb plugin
LIBS += -lcpframework

HEADERS += inc/advsecsettingsloader.h \
    inc/advsecsettingscertificatemodel.h \
    inc/advsecsettingscertificate.h \
    inc/advsecsettingstrustedcertusageuids.h \
    inc/advsecsettingsstoreuids.h \
    inc/advsecsettingsviewbase.h \
    inc/advsecsettingsmainview.h \
    inc/advsecsettingscertificatelistview.h \
    inc/advsecsettingscertificatedetailview.h \
    inc/advsecsettingscerttrustsettingsview.h \
    inc/advsecsettingssecuritymodulemodel.h \
    inc/advsecsettingssecuritymoduleview.h \
    inc/advsecsettingssecuritymoduledetailview.h

SOURCES += src/advsecsettingsloader.cpp \
    src/advsecsettingscertificatemodel.cpp \
    src/advsecsettingscertificate.cpp \
    src/advsecsettingsviewbase.cpp \
    src/advsecsettingsmainview.cpp \
    src/advsecsettingscertificatelistview.cpp \
    src/advsecsettingscertificatedetailview.cpp \
    src/advsecsettingscerttrustsettingsview.cpp \
    src/advsecsettingssecuritymodulemodel.cpp \
    src/advsecsettingssecuritymoduleview.cpp \
    src/advsecsettingssecuritymoduledetailview.cpp

symbian: {
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    TARGET.UID3 = 0X2002E684
    TARGET.EPOCALLOWDLLDATA = 1

    INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

    LIBS += -lefsrv                # RFs
    LIBS += -lcertstore            # CUnifiedCertStore
    LIBS += -lctframework          # CCTCertInfo
    LIBS += -lx509                 # CX509Certificate
    LIBS += -lpkixcert             # CPKIXValidationResult
    LIBS += -lX509CertNameParser   # X509CertNameParser
    LIBS += -lhash                 # CMD5
    LIBS += -lDevTokenClient       # CTrustSitesStore
    LIBS += -lcryptography         # CRSAParameters, CDSAPublicKey, TInteger::BitCount
    LIBS += -lcrypto               # CValidityPeriod, CSignedObject, CAlgorithmIdentifier

    PLUGIN_STUB_PATH = /resource/qt/plugins/controlpanel

    deploy.path = C:
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_STUB_PATH
    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin

    BLD_INF_RULES.prj_exports += \
        "$${LITERAL_HASH}include <platform_paths.hrh>" \
        "rom/cpadvancedsecplugin.iby CORE_APP_LAYER_IBY_EXPORT_PATH(cpadvancedsecplugin.iby)" \
        "rom/cpadvancedsecplugin_resources.iby LANGUAGE_APP_LAYER_IBY_EXPORT_PATH(cpadvancedsecplugin_resources.iby)"

    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin $$deploy.path$$qtplugins.path/$$basename(qtplugin)"

    MMP_RULES += SMPSAFE
    
    HEADERS += inc/advsecsettingscertificatemodel_symbian_p.h \
        inc/advsecsettingscertlistbuilder_symbian.h \
        inc/advsecsettingscertdetailsbuilder_symbian.h \
        inc/advsecsettingslabeledcertinfo_symbian.h \
        inc/advsecsettingscertmover_symbian.h \
        inc/advsecsettingssecuritymodulemodel_symbian_p.h \
        inc/advsecsettingssecuritymodule_symbian.h \
        inc/advsecsettingssecuritymoduleeraser_symbian.h
    SOURCES += src/advsecsettingscertificatemodel_symbian_p.cpp \
        src/advsecsettingscertlistbuilder_symbian.cpp \
        src/advsecsettingscertdetailsbuilder_symbian.cpp \
        src/advsecsettingslabeledcertinfo_symbian.cpp \
        src/advsecsettingscertmover_symbian.cpp \
        src/advsecsettingssecuritymodulemodel_symbian_p.cpp \
        src/advsecsettingssecuritymodule_symbian.cpp \
        src/advsecsettingssecuritymoduleeraser_symbian.cpp
} else {
    HEADERS += inc/advsecsettingscertificatemodel_stub_p.h \
        inc/advsecsettingssecuritymodulemodel_stub_p.h
    SOURCES += src/advsecsettingscertificatemodel_stub_p.cpp \
        src/advsecsettingssecuritymodulemodel_stub_p.cpp
}

TRANSLATIONS = certificate_manager.ts
