/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCanvas3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "program3d_p.h"

QT_BEGIN_NAMESPACE
QT_CANVAS3D_BEGIN_NAMESPACE

/*!
 * \qmltype Program3D
 * \since QtCanvas3D 1.0
 * \inqmlmodule QtCanvas3D
 * \brief Contains a shader program.
 *
 * An uncreatable QML type that contains a compiled shader program. You can get it by calling
 * \l{Context3D::createProgram()}{Context3D.createProgram()} method.
 */

/*!
 * \internal
 */
CanvasProgram::CanvasProgram(CanvasGlCommandQueue *queue, QObject *parent) :
    CanvasAbstractObject(queue, parent),
    m_programId(queue->createResourceId()),
    m_linked(false)
{
    Q_ASSERT(m_commandQueue);

    m_commandQueue->queueCommand(CanvasGlCommandQueue::glCreateProgram, m_programId);
}

/*!
 * \internal
 */
CanvasProgram::~CanvasProgram()
{
    del();
}

/*!
 * \internal
 */
bool CanvasProgram::isAlive()
{
    return bool(m_programId);
}

/*!
 * \internal
 */
void CanvasProgram::attach(CanvasShader *shader)
{
    if (m_programId) {
        if (m_attachedShaders.count(shader) == 0) {
            m_attachedShaders.append(shader);
            m_commandQueue->queueCommand(CanvasGlCommandQueue::glAttachShader,
                                         m_programId, shader->id());
        }
    }
}

/*!
 * \internal
 */
void CanvasProgram::detach(CanvasShader *shader)
{
    if (m_programId) {
        if (m_attachedShaders.count(shader) > 0) {
            m_attachedShaders.removeOne(shader);
            m_commandQueue->queueCommand(CanvasGlCommandQueue::glDetachShader,
                                         m_programId, shader->id());
        }
    }
}

/*!
 * \internal
 */
const QList<CanvasShader *> &CanvasProgram::attachedShaders() const
{
    return m_attachedShaders;
}

/*!
 * \internal
 */
void CanvasProgram::link()
{
    if (m_programId) {
        m_commandQueue->queueCommand(CanvasGlCommandQueue::glLinkProgram, m_programId);
        m_linked = true;
    }
}

/*!
 * \internal
 */
bool CanvasProgram::isLinked()
{
    // This method reports true if linking has been attempted for this program.
    // We don't know if linking will be successful.
    return m_linked;
}

/*!
 * \internal
 */
void CanvasProgram::useProgram()
{
    if (m_programId) {
        m_commandQueue->queueCommand(CanvasGlCommandQueue::glUseProgram, m_programId);
    }
}

/*!
 * \internal
 */
void CanvasProgram::bindAttributeLocation(int index, const QString &name)
{
    if (m_programId) {
        GlCommand &command = m_commandQueue->queueCommand(CanvasGlCommandQueue::glBindAttribLocation,
                                                          m_programId, GLint(index));
        command.data = new QByteArray(name.toLatin1());
    }
}

/*!
 * \internal
 */
void CanvasProgram::del()
{
    if (m_programId) {
        m_commandQueue->queueCommand(CanvasGlCommandQueue::glDeleteProgram, m_programId);
        m_programId = 0;
    }
    m_attachedShaders.clear();
}

/*!
 * \internal
 */
void CanvasProgram::validateProgram()
{
    if (m_programId)
        m_commandQueue->queueCommand(CanvasGlCommandQueue::glValidateProgram, m_programId);
}

/*!
 * \internal
 */
GLint CanvasProgram::id()
{
    return m_programId;
}

/*!
 * \internal
 */
QDebug operator<<(QDebug dbg, const CanvasProgram *program)
{
    if (program)
        dbg.nospace() << "Program3D("<< program->name() << ", id:" << program->m_programId << ")";
    else
        dbg.nospace() << "Program3D("<< ((void*) program) <<")";
    return dbg.maybeSpace();
}

QT_CANVAS3D_END_NAMESPACE
QT_END_NAMESPACE
