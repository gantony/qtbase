/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QHTML5INTEGRATION_H
#define QHTML5INTEGRATION_H

#include "qhtml5window.h"

#include <qpa/qplatformintegration.h>
#include <qpa/qplatformscreen.h>

#include <QtCore/qhash.h>

#include <emscripten.h>
#include <emscripten/html5.h>

QT_BEGIN_NAMESPACE

class QHtml5EventTranslator;
class QHtml5FontDatabase;
//class QHtml5Window;
class QHtml5EventDispatcher;
//class QHtml5Screen;
class QHtml5Compositor;
class QHtml5BackingStore;

class QHtml5Integration : public QObject, public QPlatformIntegration
{
    Q_OBJECT
public:
    QHtml5Integration();
    ~QHtml5Integration();

    bool hasCapability(QPlatformIntegration::Capability cap) const override;
    QPlatformWindow *createPlatformWindow(QWindow *window) const override;
    QPlatformBackingStore *createPlatformBackingStore(QWindow *window) const override;
#ifndef QT_NO_OPENGL
    QPlatformOpenGLContext *createPlatformOpenGLContext(QOpenGLContext *context) const override;
#endif
    QPlatformFontDatabase *fontDatabase() const override;
    QAbstractEventDispatcher *createEventDispatcher() const override;
    QVariant styleHint(QPlatformIntegration::StyleHint hint) const override;
    QStringList themeNames() const override;
    QPlatformTheme *createPlatformTheme(const QString &name) const override;

    static QHtml5Integration *get();
    //QHtml5Screen *screen() { return mScreen; }
    //QHtml5Compositor *compositor() { return mCompositor; }
    static void QHtml5BrowserExit();
    static void updateQScreenAndCanvasRenderSize();

private:
    mutable QHtml5FontDatabase *mFontDb;
   // QHtml5Compositor *mCompositor;
   // mutable QHtml5Screen *mScreen;
    mutable QHtml5EventTranslator *m_eventTranslator;
    mutable QHtml5EventDispatcher *m_eventDispatcher;
    static int uiEvent_cb(int eventType, const EmscriptenUiEvent *e, void *userData);
    mutable QHash<QWindow *, QHtml5BackingStore *> m_backingStores;
};

QT_END_NAMESPACE

#endif // QHTML5INTEGRATION_H
