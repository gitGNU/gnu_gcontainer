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
 * SECTION:gbin
 * @short_description: One-child object container
 *
 * This is the simplest implementation of the #GContainerable interface.
 * A #GBin container can only own one #GObject that implements the #GChildable
 * interface, so internally there is only a private pointer (@content) that
 * point to the child instance.
 **/

/**
 * GBin:
 *
 * All the fields in the GBin structure are private and should never
 * be accessed directly.
 **/

#include "gbin.h"


enum
{
  PROP_0,
  PROP_CHILD
};


static void	containerable_init	(GContainerableIface *iface);
static void	set_property		(GObject	*object,
					 guint		 prop_id,
					 const GValue	*value,
					 GParamSpec	*pspec);
static GSList *	get_children		(GContainerable	*containerable);
static gboolean	add			(GContainerable	*containerable,
					 GChildable	*childable);
static gboolean	remove			(GContainerable	*containerable,
					 GChildable	*childable);


G_DEFINE_TYPE_EXTENDED (GBin, g_bin, G_TYPE_CHILD, 0, 
                        G_IMPLEMENT_INTERFACE (G_TYPE_CONTAINERABLE, 
                                               containerable_init));


static void
containerable_init (GContainerableIface *iface)
{
  iface->get_children = get_children;
  iface->add = add;
  iface->remove = remove;
}

static void
g_bin_class_init (GBinClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  gobject_class->set_property = set_property;
  gobject_class->dispose = g_containerable_dispose;

  g_object_class_override_property (gobject_class, PROP_CHILD, "child");
}

static void
g_bin_init (GBin *bin)
{
  bin->content = NULL;
}

static void
set_property (GObject      *object,
	      guint         prop_id,
	      const GValue *value,
	      GParamSpec   *pspec)
{
  GContainerable *containerable = (GContainerable *) object;

  switch (prop_id)
    {
    case PROP_CHILD:
      g_containerable_add (containerable, g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}


static GSList *
get_children (GContainerable *containerable)
{
  GBin *bin = (GBin *) containerable;

  if (bin->content == NULL)
    return NULL;

  return g_slist_append (NULL, bin->content);
}

static gboolean
add (GContainerable *containerable,
     GChildable     *childable)
{
  GBin *bin = (GBin *) containerable;

  if (bin->content != NULL)
    {
      g_warning ("Attempting to add an object with type %s to a %s, "
                 "but as a GBin can only contain one object at a time; "
                 "it already contains a widget of type %s",
                 g_type_name (G_OBJECT_TYPE (childable)),
                 g_type_name (G_OBJECT_TYPE (bin)),
                 g_type_name (G_OBJECT_TYPE (bin->content)));
      return FALSE;
    }

  bin->content = childable;
  return TRUE;
}

static gboolean
remove (GContainerable *containerable,
	GChildable     *childable)
{
  GBin *bin = (GBin *) containerable;
  
  bin->content = NULL;
  return TRUE;
}


/**
 * g_bin_new:
 *
 * Creates a new one-child container.
 *
 * Return value: a #GBin instance
 **/
GObject *
g_bin_new (void)
{
  return g_object_new (G_TYPE_BIN, NULL);
}
