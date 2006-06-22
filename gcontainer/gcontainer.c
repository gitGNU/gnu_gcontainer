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
 * @short_description: A generic object container
 *
 * An implementation of #GContainerable to manage a generic list of #GObject
 * that implements #GChildable. The children are internally managed trought
 * a #GSList.
 **/

/**
 * GContainer:
 *
 * All the fields in the GContainer structure are private and should never
 * be accessed directly.
 **/

#include "gcontainer.h"


enum
{
  PROP_0,
  PROP_CHILD
};


static void	g_container_containerable_init
                                        (GContainerableIface *iface);
static void     g_container_set_property(GObject             *object,
                                         guint                prop_id,
                                         const GValue        *value,
                                         GParamSpec          *pspec);

static GSList * g_container_get_children(GContainerable      *containerable);
static gboolean g_container_add         (GContainerable      *containerable,
                                         GChildable          *childable);
static gboolean g_container_remove      (GContainerable      *containerable,
                                         GChildable          *childable);


G_DEFINE_TYPE_EXTENDED (GContainer, g_container, 
                        G_TYPE_CHILD, 0, 
                        G_IMPLEMENT_INTERFACE (G_TYPE_CONTAINERABLE, 
                                               g_container_containerable_init));


static void
g_container_containerable_init (GContainerableIface *iface)
{
  iface->get_children = g_container_get_children;
  iface->add = g_container_add;
  iface->remove = g_container_remove;
}

static void
g_container_class_init (GContainerClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  gobject_class->set_property = g_container_set_property;
  gobject_class->dispose = g_containerable_dispose;

  g_object_class_override_property (gobject_class, PROP_CHILD, "child");
}

static void
g_container_init (GContainer *container)
{
  container->children = NULL;
}

static void
g_container_set_property (GObject      *object,
                          guint         prop_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  GContainerable *containerable = G_CONTAINERABLE (object);

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
g_container_get_children (GContainerable *containerable)
{
  GContainer *container = G_CONTAINER (containerable);

  return g_slist_copy (container->children);
}

static gboolean
g_container_add (GContainerable *containerable,
                 GChildable     *childable)
{
  GContainer *container = G_CONTAINER (containerable);

  container->children = g_slist_append (container->children, childable);
  return TRUE;
}

static gboolean
g_container_remove (GContainerable *containerable,
                    GChildable     *childable)
{
  GContainer *container;
  GSList     *node;

  container = G_CONTAINER (containerable);
  node = g_slist_find (container->children, childable);

  if (! node)
    return FALSE;

  container->children = g_slist_delete_link (container->children, node);
  return TRUE;
}


/**
 * g_container_new:
 *
 * Creates a new generic container.
 *
 * Return value: a #GContainer instance
 **/
GObject *
g_container_new (void)
{
  return g_object_new (G_TYPE_CONTAINER, NULL);
}
