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
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#ifndef __G_CONTAINER_PRIVATE_H__
#define __G_CONTAINER_PRIVATE_H__


G_BEGIN_DECLS


struct _GContainerPrivate
{
  GSList		*children;
};


G_END_DECLS


#endif /* __G_CONTAINER_PRIVATE_H__ */
