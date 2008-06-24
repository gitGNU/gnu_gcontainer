/* gcontainer - A generic container for the glib-2.0 library
 * Copyright (C) 2006, 2008 - Fontana Nicola <ntd@entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */

/*
 * This is a collection of macros and functions needed by gcontainer but
 * not found in gobject (at least, they are not publicly available).
 * Someone provides private (from libgobject point of view) informations,
 * others are here just to feed my lazyness.
 * 
 * Keep in sync with libgobject.
 */


#ifndef __G_OBJECT_MISSINGS_H__
#define __G_OBJECT_MISSINGS_H__


/**
 * G_TYPE_INSTANCE_FROM_INTERFACE:
 * @g_iface: location of a valid #GTypeInterface structure
 *
 * This information is obtained by accessing the @g_instance_type
 * private field of the #GTypeInterface structure.
 *
 * Returns: the instance type which implements @g_iface
 **/
#define G_TYPE_INSTANCE_FROM_INTERFACE(g_iface) \
  (((GTypeInterface*) (g_iface))->g_instance_type)

/**
 * G_TYPE_INSTANCE_WHICH_IMPLEMENTS:
 * @instance: location of the #GTypeInstance structure
 * @iface_type: the interface type
 *
 * Useful in the interface implementations while writing default methods
 * which must chain-up the instance parent methods.
 * A good example is g_containerable_dispose() and g_childable_dispose().
 *
 * Returns: the type in the @instance hierarchy which implements @iface_type
 **/
#define G_TYPE_INSTANCE_WHICH_IMPLEMENTS(instance,iface_type) \
  G_TYPE_INSTANCE_FROM_INTERFACE (G_TYPE_INSTANCE_GET_INTERFACE (object,iface_type,GTypeInterface))

#endif /* __G_OBJECT_MISSINGS_H__ */
