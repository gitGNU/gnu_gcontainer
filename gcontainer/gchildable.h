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


#ifndef __G_CHILDABLE_H__
#define __G_CHILDABLE_H__

#include <glib-object.h>


G_BEGIN_DECLS

/* Forward declarations */

typedef struct _GContainerable   GContainerable;


#define G_TYPE_CHILDABLE            (g_childable_get_type ())
#define G_CHILDABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_CHILDABLE, GChildable))
#define G_IS_CHILDABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_CHILDABLE))
#define G_CHILDABLE_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), G_TYPE_CHILDABLE, GChildableIface))

typedef struct _GChildable       GChildable; /* Dummy typedef */
typedef struct _GChildableIface  GChildableIface;

struct _GChildableIface
{
  GTypeInterface base_iface;


  /* Virtual Table */

  GContainerable *      (*get_parent)           (GChildable     *childable);
  void                  (*set_parent)           (GChildable     *childable,
                                                 GContainerable *new_parent);

  /* Signals */

  void                  (*parent_set)           (GChildable     *childable,
                                                 GContainerable *old_parent);
};


GType		g_childable_get_type		(void) G_GNUC_CONST;

GContainerable *g_childable_get_parent		(GChildable	*childable);
void		g_childable_set_parent		(GChildable	*childable,
						 GContainerable	*new_parent);
void		g_childable_unparent		(GChildable	*childable);
void            g_childable_reparent            (GChildable     *childable,
                                                 GContainerable *new_parent);

void	        g_childable_dispose		(GObject	*object);

G_END_DECLS


#endif /* __G_CHILDABLE_H__ */

