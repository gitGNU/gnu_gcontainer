/* gcontainer - A generic container for the glib-2.0 library.
 * Copyright (C) 2006 - Fontana Nicola <ntd@users.sourceforge.net>
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


#ifndef __G_CHILD_H__
#define __G_CHILD_H__

#include <gcontainer/gcontainerable.h>


G_BEGIN_DECLS

#define G_TYPE_CHILD             (g_child_get_type ())
#define G_CHILD(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_CHILD, GChild))
#define G_CHILD_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_CHILD, GChildClass))
#define G_IS_CHILD(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_CHILD))
#define G_IS_CHILD_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_CHILD))
#define G_CHILD_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_CHILD, GChildClass))

typedef struct _GChild       GChild;
typedef struct _GChildClass  GChildClass;

struct _GChild
{
  GInitiallyUnowned	 initially_unowned;

  /*< private >*/

  GContainerable	*parent;
};

struct _GChildClass
{
  GInitiallyUnownedClass parent_class;
};


GType		g_child_get_type		(void) G_GNUC_CONST;

GObject *	g_child_new			(void);

G_END_DECLS


#endif /* __G_CHILD_H__ */

