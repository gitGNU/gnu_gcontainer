/* gcontainer - A generic container for the glib-2.0 library
 * Copyright (C) 2006, 2008 - Fontana Nicola <ntd@entidi.it>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the 
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */


#ifndef __G_CONTAINERABLE_H__
#define __G_CONTAINERABLE_H__

#include <gcontainer/gchildable.h>


G_BEGIN_DECLS

#define G_TYPE_CONTAINERABLE            (g_containerable_get_type ())
#define G_CONTAINERABLE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_CONTAINERABLE, GContainerable))
#define G_IS_CONTAINERABLE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_CONTAINERABLE))
#define G_CONTAINERABLE_GET_IFACE(obj)  (G_TYPE_INSTANCE_GET_INTERFACE ((obj), G_TYPE_CONTAINERABLE, GContainerableIface))

/* Dummy typedef GContainerable forward declared in gchildable.h */
typedef struct _GContainerableIface  GContainerableIface;

struct _GContainerableIface
{
  GTypeInterface base_iface;


  /* Virtual Table */

  GSList *	(*get_children)			(GContainerable *containerable);
  gboolean	(*add)				(GContainerable	*containerable,
						 GChildable	*childable);
  gboolean	(*remove)			(GContainerable *containerable,
						 GChildable	*childable);
};


GType		g_containerable_get_type	(void) G_GNUC_CONST;
GSList *	g_containerable_get_children	(GContainerable	*containerable);
void		g_containerable_add		(GContainerable	*containerable,
						 GChildable	*childable);
void		g_containerable_remove		(GContainerable	*containerable,
						 GChildable	*childable);

void		g_containerable_foreach		(GContainerable	*containerable,
						 GCallback	 callback,
						 gpointer	 user_data);
void		g_containerable_propagate	(GContainerable *containerable,
						 guint           signal_id,
						 GQuark          detail,
						 ...);
void		g_containerable_propagate_by_name
						(GContainerable *containerable,
						 const gchar    *detailed_signal,
						 ...);
void		g_containerable_propagate_valist(GContainerable *containerable,
						 guint           signal_id,
						 GQuark          detail,
						 va_list         var_args);
void		g_containerable_dispose		(GObject	*object);


G_END_DECLS


#endif /* __G_CONTAINERABLE_H__ */

