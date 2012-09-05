/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "propertyfield.h"
#include <QDebug>

PropertyField::PropertyField(QObject* subject, const QMetaProperty& prop, QWidget *parent) :
    QLineEdit(parent), m_subject(subject), m_lastChangeTime(QTime::currentTime()), m_prop(prop)
{
    setReadOnly(true);
    if (prop.hasNotifySignal()) {
        QMetaMethod signal = prop.notifySignal();
        QMetaMethod updateSlot = metaObject()->method(metaObject()->indexOfSlot("propertyChanged()"));
        connect(m_subject, signal, this, updateSlot);
    }
    propertyChanged();
}

QString PropertyField::valueToString(QVariant val)
{
    QString text = val.toString();
    if (val.type() == QVariant::Size)
        text = QString("%1 x %2").arg(val.toSize().width()).arg(val.toSize().height());
    else if (val.type() == QVariant::SizeF)
        text = QString("%1 x %2").arg(val.toSizeF().width()).arg(val.toSizeF().height());
    else if (val.type() == QVariant::Rect)
        text = QString("%1 x %2 +%3 +%4").arg(val.toRect().width())
                .arg(val.toRect().height()).arg(val.toRect().x()).arg(val.toRect().y());
    return text;
}

void PropertyField::propertyChanged()
{
    if (m_prop.isReadable()) {
        QVariant val = m_prop.read(m_subject);
        QString text = valueToString(val);
        QPalette modPalette = palette();

        // If we are seeing a value for the first time,
        // pretend it was that way for a while already.
        if (m_lastText.isEmpty()) {
            m_lastText = text;
            m_lastTextShowing = text;
            m_lastChangeTime = QTime::currentTime().addSecs(-5);
        }

        qDebug() << "  " << QString::fromUtf8(m_prop.name()) << ":" << val;
        // If the value has recently changed, show the change
        if (text != m_lastText || m_lastChangeTime.elapsed() < 1000) {
            setText(m_lastTextShowing + " -> " + text);
            modPalette.setBrush(QPalette::Text, Qt::red);
            m_lastChangeTime.start();
            m_lastText = text;
        }
        // If the value hasn't changed recently, just show the current value
        else {
            setText(text);
            m_lastText = text;
            m_lastTextShowing = text;
            modPalette.setBrush(QPalette::Text, Qt::black);
        }
        setPalette(modPalette);
    }
}
