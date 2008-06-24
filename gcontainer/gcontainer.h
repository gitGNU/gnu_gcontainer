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


#ifndef __G_CONTAINER_H__
#define __G_CONTAINER_H__

/* This is not only the include file for the GContainer object, but also
 * for the whole gcontainer library so I must include all the headers */

#include <gcontainer/gchild.h>
#include <gcontainer/gbin.h>


G_BEGIN_DECLS


#define G_TYPE_CONTAINER             (g_container_get_type ())
#define G_CONTAINER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_CONTAINER, GContainer))
#define G_CONTAINER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_CONTAINER, GContainerClass))
#define G_IS_CONTAINER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_CONTAINER))
#define G_IS_CONTAINER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_CONTAINER))
#define G_CONTAINER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_CONTAINER, GContainerClass))


typedef struct _GContainer	  GContainer;
typedef struct _GContainerClass   GContainerClass;
typedef struct _GContainerPrivate GContainerPrivate;

struct _GContainer
{
  GChild		 child;

  /*< private >*/
  GContainerPrivate	*priv;
};

struct _GContainerClass
{
  GChildClass		 parent_class;
};


GType			g_container_get_type	(void) G_GNUC_CONST;
GObject *		g_container_new		(void);


G_END_DECLS


#endif /* __G_CONTAINER_H__ */
