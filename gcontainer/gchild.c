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
 * This is the natural implementation of the #GChildable interface.
 * Can be used as base class for objects that implements #GChildable.
 **/

/**
 * GChild:
 *
 * All the fields in the GChild structure are private and should never
 * be accessed directly.
 **/

#include "gchild.h"


enum
{
  PROP_0,
  PROP_PARENT
};


static void	        g_child_childable_init	(GChildableIface *iface);
static void             g_child_get_property    (GObject         *object,
                                                 guint            prop_id,
                                                 GValue          *value,
                                                 GParamSpec      *pspec);
static void             g_child_set_property    (GObject         *object,
                                                 guint            prop_id,
                                                 const GValue    *value,
                                                 GParamSpec      *pspec);

static GContainerable * g_child_get_parent      (GChildable      *childable);
static void             g_child_set_parent      (GChildable      *childable,
                                                 GContainerable  *new_parent);


G_DEFINE_TYPE_EXTENDED (GChild, g_child, 
                        G_TYPE_INITIALLY_UNOWNED, 0, 
                        G_IMPLEMENT_INTERFACE (G_TYPE_CHILDABLE, 
                                               g_child_childable_init));


static void
g_child_childable_init (GChildableIface *iface)
{
  iface->get_parent = g_child_get_parent;
  iface->set_parent = g_child_set_parent;
}

static void
g_child_class_init (GChildClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  gobject_class->get_property = g_child_get_property;
  gobject_class->set_property = g_child_set_property;
  gobject_class->dispose = g_childable_dispose;

  g_object_class_override_property (gobject_class, PROP_PARENT, "parent");
}

static void
g_child_init (GChild *child)
{
  child->parent = NULL;
}

static void
g_child_get_property (GObject    *object,
                      guint       prop_id,
                      GValue     *value,
                      GParamSpec *pspec)
{
  GChildable *childable = G_CHILDABLE (object);

  switch (prop_id)
    {
    case PROP_PARENT:
      g_value_set_object (value, g_childable_get_parent (childable));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
g_child_set_property (GObject      *object,
                      guint         prop_id,
                      const GValue *value,
                      GParamSpec   *pspec)
{
  GChildable *childable = G_CHILDABLE (object);

  switch (prop_id)
    {
    case PROP_PARENT:
      g_childable_set_parent (childable,
			      (GContainerable *) g_value_get_object (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static GContainerable *
g_child_get_parent (GChildable *childable)
{
  return ((GChild *) childable)->parent;
}

static void
g_child_set_parent (GChildable     *childable,
                    GContainerable *new_parent)
{
  ((GChild *) childable)->parent = new_parent;
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
