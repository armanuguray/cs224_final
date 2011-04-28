#include "GLWidget.h"

#include <QTime>
#include <QTimer>
#include "CS123Algebra.h"
#include "DrawEngine.h"
#include "Settings.h"
#include <QMouseEvent>
#include <QWheelEvent>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::DoubleBuffer), parent), m_renderOverlay(true)
{
    this->setFocusPolicy(Qt::StrongFocus);
    this->setMouseTracking(true);
    //this->setAutoBufferSwap(false);
    this->setAutoFillBackground(false);
}

GLWidget::~GLWidget()
{
    delete m_timer;
    delete m_time;
    delete m_drawengine;
}

void GLWidget::initializeGL()
{
    m_drawengine = new DrawEngine(this->context(), this->width(), this->height());

    // start the rendering loop
    m_time = new QTime();
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(30.0);
}

void GLWidget::paintEvent(QPaintEvent *event)
{
    QGLWidget::paintEvent(event);

    float time = m_time->elapsed();
    m_time->restart();
    m_drawengine->drawFrame(time);
    glFlush();

    if (m_renderOverlay) {
        this->renderOverlayText();
    }
}

void GLWidget::resizeGL(int width, int height)
{
    m_drawengine->resize(width, height);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    m_drawengine->mouse_scroll(event->delta());
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector2 pos = Vector2(event->x(), event->y());
    Vector2 delta = pos - m_old;
    m_old = pos;
    if(event->buttons() & Qt::RightButton)
        m_drawengine->mouse_dragged(m_old, delta, MouseButtonRight);
    else {
        MouseButton button;
        if (event->buttons() & Qt::LeftButton)
        {
            if (event->modifiers() & Qt::ControlModifier)
                button = MouseButtonCTRLLeft;
            else button = MouseButtonLeft;

            m_drawengine->mouse_dragged(m_old, delta, button);
        }
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_old = Vector2(event->x(), event->y());
    if (event->buttons() & Qt::LeftButton && event->modifiers() & Qt::ControlModifier)
        m_drawengine->mouse_down(m_old, MouseButtonCTRLLeft);
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    bool ignore = true;

    switch (event->key()) {
    case Qt::Key_S:
        m_renderOverlay = !m_renderOverlay;
        ignore = false;
        break;

    default:
        break;
    }

    if (ignore) event->ignore();
}

void GLWidget::renderOverlayText() {
    glDisable(GL_CULL_FACE);
    const float &x = settings.overlay_text_offset.x();
    const float &y = settings.overlay_text_offset.y();
    const int &max_border = settings.overlay_text_max_border;

    painter.begin(this);

    painter.setFont(settings.overlay_text_font);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::white);

    QFontMetrics metrics = QFontMetrics(painter.font());
    int border = qMax(max_border, metrics.leading());

    QRect box = metrics.boundingRect(rect(), Qt::TextWordWrap, settings.overlay_text);

    painter.fillRect(QRect(x, y, box.width() + 2 * border, box.height() + 2 * border), QColor(0, 0, 0, 127));
    painter.drawText(x + border, y + border, box.width(), box.height(), Qt::TextWordWrap, settings.overlay_text);

    painter.end();
    glEnable(GL_CULL_FACE);
}
