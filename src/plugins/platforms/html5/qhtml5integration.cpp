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

#include "qhtml5integration.h"
#include "qhtml5eventtranslator.h"
#include "qhtml5eventdispatcher.h"
//#include "qhtml5compositor.h"
//#include "qhtml5openglcontext.h"
#include "qhtml5theme.h"

//#include "qhtml5window.h"
#ifndef QT_NO_OPENGL
//# include "qhtml5backingstore.h"
#endif
#include "qhtml5fontdatabase.h"
#if defined(Q_OS_UNIX)
#include <QtEventDispatcherSupport/private/qgenericunixeventdispatcher_p.h>
#endif
#include <qpa/qplatformwindow.h>
#include <QtGui/QScreen>
#include <qpa/qwindowsysteminterface.h>
#include <QCoreApplication>

#include <emscripten/bind.h>

// this is where EGL headers are pulled in, make sure it is last
//#include "qhtml5screen.h"

using namespace emscripten;
QT_BEGIN_NAMESPACE

void browserBeforeUnload() {
    QHtml5Integration::QHtml5BrowserExit();
}

EMSCRIPTEN_BINDINGS(my_module) {
    // This is a good one to look through the compiled JS to make sure Embind works properly :)
    function("browserBeforeUnload", &browserBeforeUnload);
}

static QHtml5Integration *globalHtml5Integration;
QHtml5Integration *QHtml5Integration::get() { return globalHtml5Integration; }

void emscriptenOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    int emOutputFlags = (EM_LOG_CONSOLE | EM_LOG_DEMANGLE);
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
    case QtDebugMsg:
        emscripten_log(emOutputFlags, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtInfoMsg:
        emscripten_log(emOutputFlags, "Info: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        emOutputFlags |= EM_LOG_WARN;
        emscripten_log(emOutputFlags, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        emOutputFlags |= EM_LOG_ERROR;
        emscripten_log(emOutputFlags, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        emOutputFlags |= EM_LOG_ERROR;
        emscripten_log(emOutputFlags, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}

QHtml5Integration::QHtml5Integration()
    : mFontDb(0),
      //mCompositor(new QHtml5Compositor),
      //mScreen(new QHtml5Screen(mCompositor)),
      m_eventDispatcher(0)
{
    qSetMessagePattern(QString("(%{function}:%{line}) - %{message}"));
   // qInstallMessageHandler(emscriptenOutput);

    globalHtml5Integration = this;

    //updateQScreenAndCanvasRenderSize();
    //screenAdded(mScreen);
    //emscripten_set_resize_callback(0, (void *)this, 1, uiEvent_cb);

    m_eventTranslator = new QHtml5EventTranslator();
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QHtml5Integration\n");
#endif
    EM_ASM(// exit app if browser closes
           window.onbeforeunload = function () {
           Module.browserBeforeUnload();
           };
     );
}

QHtml5Integration::~QHtml5Integration()
{
    qDebug() << Q_FUNC_INFO;
    //delete mCompositor;
    //destroyScreen(mScreen);
    delete mFontDb;
    delete m_eventTranslator;
}

void QHtml5Integration::QHtml5BrowserExit()
{
    QCoreApplication *app = QCoreApplication::instance();
    app->quit();
}

bool QHtml5Integration::hasCapability(QPlatformIntegration::Capability cap) const
{
    switch (cap) {
    /*case ThreadedPixmaps: return true;
    case OpenGL: return false;
    case ThreadedOpenGL: return false;
    case RasterGLSurface: return true;
    case MultipleWindows: return true;
    case WindowManagement: return true;*/
    default: return QPlatformIntegration::hasCapability(cap);
    }
}

QPlatformWindow *QHtml5Integration::createPlatformWindow(QWindow *window) const
{
#ifdef QEGL_EXTRA_DEBUG
    qWarning("QHtml5Integration::createPlatformWindow %p\n",window);
#endif
    return NULL; /*
    QHtml5Window *w = NULL; // new QHtml5Window(window, mCompositor, m_backingStores.value(window));
    //w->create();

    return w;*/
}

QPlatformBackingStore *QHtml5Integration::createPlatformBackingStore(QWindow *window) const
{
//#ifdef QEGL_EXTRA_DEBUG
    qWarning("QHtml5Integration::createWindowSurface %p\n", window);
//#endif
#ifndef QT_NO_OPENGL
    QHtml5BackingStore *backingStore = NULL ; //new QHtml5BackingStore(mCompositor, window);
    //m_backingStores.insert(window, backingStore);
    return backingStore;
#else
    return nullptr;
#endif
}

#ifndef QT_NO_OPENGL
QPlatformOpenGLContext *QHtml5Integration::createPlatformOpenGLContext(QOpenGLContext *context) const
{
    return NULL; //new QHtml5OpenGLContext(context->format());
}
#endif

QPlatformFontDatabase *QHtml5Integration::fontDatabase() const
{
    if (mFontDb == 0)
        mFontDb = new QHtml5FontDatabase();

    return mFontDb;
}

QAbstractEventDispatcher *QHtml5Integration::createEventDispatcher() const
{
    return new QHtml5EventDispatcher();
}

QVariant QHtml5Integration::styleHint(QPlatformIntegration::StyleHint hint) const
{
    return QPlatformIntegration::styleHint(hint);
}

QStringList QHtml5Integration::themeNames() const
{
    return QStringList() << QLatin1String("html5");
}

QPlatformTheme *QHtml5Integration::createPlatformTheme(const QString &name) const
{
    if (name == QLatin1String("html5"))
        return new QHtml5Theme();
    return QPlatformIntegration::createPlatformTheme(name);
}

int QHtml5Integration::uiEvent_cb(int eventType, const EmscriptenUiEvent *e, void *userData)
{
    Q_UNUSED(e)
    Q_UNUSED(userData)

    if (eventType == EMSCRIPTEN_EVENT_RESIZE) {
        // This resize event is called when the HTML window is resized. Depending
        // on the page layout the the canvas might also have been resized, so we
        // update the Qt screen size (and canvas render size).
        //updateQScreenAndCanvasRenderSize();
    }

    return 0;
}

static void set_canvas_size(double width, double height)
{
    EM_ASM_({
        var canvas = Module.canvas;
        canvas.width = $0;
        canvas.height = $1;
    }, width, height);
}

void QHtml5Integration::updateQScreenAndCanvasRenderSize()
{
    // The HTML canvas has two sizes: the CSS size and the canvas render size.
    // The CSS size is determined according to standard CSS rules, while the
    // render size is set using the "width" and "height" attributes. The render
    // size must be set manually and is not auto-updated on CSS size change.
    // Setting the render size to a value larger than the CSS size enables high-dpi
    // rendering.
/*
    double css_width;
    double css_height;
    emscripten_get_element_css_size(0, &css_width, &css_height);
    QSizeF cssSize(css_width, css_height);

    QHtml5Screen *screen = QHtml5Integration::get()->mScreen;
    QSizeF canvasSize = cssSize * screen->devicePixelRatio();

    set_canvas_size(canvasSize.width(), canvasSize.height());
    screen->setGeometry(QRect(QPoint(0, 0), cssSize.toSize()));
    QHtml5Integration::get()->mCompositor->redrawWindowContent();*/
}

QT_END_NAMESPACE
