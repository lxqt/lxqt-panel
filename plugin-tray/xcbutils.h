/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2.1+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2012, 2013 Martin Graesslin <mgraesslin@kde.org>
 *            2015 David Edmundson <davidedmundson@kde.org>
 *            2022 LXQt team
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#pragma once

#include <xcb/composite.h>
#include <xcb/damage.h>
#include <xcb/randr.h>
#include <xcb/shm.h>
#include <xcb/xcb.h>
#include <xcb/xcb_atom.h>
#include <xcb/xcb_event.h>

#include <memory>
#include <QList>

/** XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY 0
#define XEMBED_WINDOW_ACTIVATE 1
#define XEMBED_WINDOW_DEACTIVATE 2
#define XEMBED_REQUEST_FOCUS 3
#define XEMBED_FOCUS_IN 4
#define XEMBED_FOCUS_OUT 5
#define XEMBED_FOCUS_NEXT 6
#define XEMBED_FOCUS_PREV 7

namespace Xcb
{
typedef xcb_window_t WindowId;

struct ScopedCPointerDeleter
{
    static inline void cleanup(void *pointer) noexcept { free(pointer); }
    void operator()(void *pointer) const noexcept { cleanup(pointer); }
};

template<typename T>
using ScopedCPointer = std::unique_ptr<T, ScopedCPointerDeleter>;

class Atom
{
public:
    explicit Atom(const QByteArray &name, xcb_connection_t *c, bool onlyIfExists = false)
        : m_connection(c)
        , m_retrieved(false)
        , m_cookie(xcb_intern_atom_unchecked(m_connection, onlyIfExists, name.length(), name.constData()))
        , m_atom(XCB_ATOM_NONE)
        , m_name(name)
    {
    }
    Atom() = delete;
    Atom(const Atom &) = delete;

    ~Atom()
    {
        if (!m_retrieved && m_cookie.sequence) {
            xcb_discard_reply(m_connection, m_cookie.sequence);
        }
    }

    operator xcb_atom_t() const
    {
        (const_cast<Atom *>(this))->getReply();
        return m_atom;
    }
    bool isValid()
    {
        getReply();
        return m_atom != XCB_ATOM_NONE;
    }
    bool isValid() const
    {
        (const_cast<Atom *>(this))->getReply();
        return m_atom != XCB_ATOM_NONE;
    }

    inline const QByteArray &name() const
    {
        return m_name;
    }

private:
    void getReply()
    {
        if (m_retrieved || !m_cookie.sequence) {
            return;
        }
        ScopedCPointer<xcb_intern_atom_reply_t> reply(xcb_intern_atom_reply(m_connection, m_cookie, nullptr));
        if (reply) {
            m_atom = reply->atom;
        }
        m_retrieved = true;
    }
    xcb_connection_t *m_connection;
    bool m_retrieved;
    xcb_intern_atom_cookie_t m_cookie;
    xcb_atom_t m_atom;
    QByteArray m_name;
};

class Atoms
{
public:
    Atoms(xcb_connection_t *c, int defaultScreen)
        : xembedAtom("_XEMBED", c)
        , selectionAtom(xcb_atom_name_by_screen("_NET_SYSTEM_TRAY", defaultScreen), c)
        , opcodeAtom("_NET_SYSTEM_TRAY_OPCODE", c)
        , messageData("_NET_SYSTEM_TRAY_MESSAGE_DATA", c)
        , visualAtom("_NET_SYSTEM_TRAY_VISUAL", c)
    {
    }

    Atom xembedAtom;
    Atom selectionAtom;
    Atom opcodeAtom;
    Atom messageData;
    Atom visualAtom;
};

extern Atoms *atoms;

} // namespace Xcb
