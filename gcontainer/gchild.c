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
 * SECTION:gchild
 * @short_description: Base implementation of the #GChildable interface
 *
 * This is the simplest implementation of the #GChildable interface.
 * Can be used as base class for #GChildable objects.
 **/

/**
 * GChild:
 *
 * All the fields in the GChild structure are private and should never
 * be accessed directly.
 **/

#include "gchild.h"

#define	PARENT_TYPE	G_TYPE_INITIALLY_UNOWNED


static void	g_child_childable_init		(GChildableIface *iface);


GType
g_child_get_type (void)
{
  static GType child_type = 0;
  
  if G_UNLIKELY (child_type == 0)
    {
      static const GTypeInfo child_info =
	{
	  sizeof (GChildClass),
	  NULL,			/* base_init */
	  NULL,			/* base_finalize */
	  g_childable_class_init,
	  NULL,			/* class_finalize */
	  NULL,			/* class_data */
	  sizeof (GChild),
	  0,			/* n_preallocs */
	  NULL,
	};
      static const GInterfaceInfo childable_info =
	{
	  (GInterfaceInitFunc)	g_child_childable_init,
	  NULL,
	};
      child_type = g_type_register_static (PARENT_TYPE, "GChild",
					   &child_info, 0);
      g_type_add_interface_static (child_type, G_TYPE_CHILDABLE,
				   &childable_info);
    }
  
  return child_type;
}

static void
g_child_childable_init (GChildableIface *iface)
{
  iface->parent_offset = G_STRUCT_OFFSET (GChild, parent);
  iface->object_parent_class = g_type_class_peek (PARENT_TYPE);
}


/**
 * g_child_new:
 *
 * Creates an generic child object with a floating reference.
 *
 * Return value: a new #GChild instance
 **/
GObject *
g_child_new (void)
{
  return g_object_new (G_TYPE_CHILD, NULL);
}

