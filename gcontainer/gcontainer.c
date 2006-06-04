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


/**
 * SECTION:gcontainer
 * @short_description: A sample implementation of the #GContainer interface
 *
 * This is the most simple implementation of the #GContainerable interface.
 * Can be used stand-alone or as base class for container objects.
 **/

/**
 * GContainer:
 *
 * All the fields in the GContainer structure are private and should never
 * be accessed directly.
 **/

#include "gcontainer.h"

#define	PARENT_TYPE	G_TYPE_CHILD


static void	g_container_containerable_init	(GContainerableIface *iface);


GType
g_container_get_type (void)
{
  static GType container_type = 0;
  
  if G_UNLIKELY (container_type == 0)
    {
      static const GTypeInfo container_info =
	{
	  sizeof (GContainerClass),
	  NULL,			/* base_init */
	  NULL,			/* base_finalize */
	  g_containerable_class_init,
	  NULL,			/* class_finalize */
	  NULL,			/* class_data */
	  sizeof (GContainer),
	  0,			/* n_preallocs */
	  NULL,
	};
      static const GInterfaceInfo containerable_info =
	{
	  (GInterfaceInitFunc)	g_container_containerable_init,
	  NULL,
	};
      container_type = g_type_register_static (PARENT_TYPE, "GContainer",
					       &container_info, 0);
      g_type_add_interface_static (container_type, G_TYPE_CONTAINERABLE,
				   &containerable_info);
    }
  
  return container_type;
}

static void
g_container_containerable_init (GContainerableIface *iface)
{
  iface->children_offset = G_STRUCT_OFFSET (GContainer, children);
  iface->object_parent_class = g_type_class_peek (PARENT_TYPE);
}


/**
 * g_container_new:
 *
 * Creates an empty container with a floating reference.
 *
 * Return value: a new #GContainer instance
 **/
GObject *
g_container_new (void)
{
  return g_object_new (G_TYPE_CONTAINER, NULL);
}

