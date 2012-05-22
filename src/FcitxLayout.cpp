/***************************************************************************
 *   Copyright (C) 2011~2011 by CSSlayer                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

// Qt
#include <QDBusArgument>
#include <QDBusMetaType>

// self
#include "FcitxLayout.h"

const QString& FcitxLayout::layout() const
{
    return m_layout;
}
const QString& FcitxLayout::langCode() const
{
    return m_langCode;
}
const QString& FcitxLayout::name() const
{
    return m_name;
}

const QString& FcitxLayout::variant() const
{
    return m_variant;
}

void FcitxLayout::setLayout(const QString& layout)
{
    m_layout = layout;
}

void FcitxLayout::setLangCode(const QString& lang)
{
    m_langCode = lang;
}

void FcitxLayout::setName(const QString& name)
{
    m_name = name;
}

void FcitxLayout::setVariant(const QString& variant)
{
    m_variant = variant;
}

void FcitxLayout::registerMetaType()
{
    qRegisterMetaType<FcitxLayout>("FcitxLayout");
    qDBusRegisterMetaType<FcitxLayout>();
    qRegisterMetaType<FcitxLayoutList>("FcitxLayoutList");
    qDBusRegisterMetaType<FcitxLayoutList>();
}

QDBusArgument& operator<<(QDBusArgument& argument, const FcitxLayout& layout)
{
    argument.beginStructure();
    argument << layout.layout();
    argument << layout.variant();
    argument << layout.name();
    argument << layout.langCode();
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, FcitxLayout& layout)
{
    QString l;
    QString variant;
    QString name;
    QString langCode;
    argument.beginStructure();
    argument >> l >> variant >> name >> langCode;
    argument.endStructure();
    layout.setLayout(l);
    layout.setVariant(variant);
    layout.setName(name);
    layout.setLangCode(langCode);
    return argument;
}
