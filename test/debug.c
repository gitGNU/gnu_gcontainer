/* libgcontainer - Test program
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


#include "demo.h"


/* Debugging macros (required for lazyness) */

#define	NAME(object) \
  g_object_get_data ((GObject *) object, "name")

#define DEBUG(type,name,action...) \
  G_STMT_START { \
      g_printf ("    -> %s '%s' ", type, name); \
      g_printf (action); \
      g_printf ("\n"); \
  } G_STMT_END

#define DEBUG_OBJECT(object,action...) \
  DEBUG(g_type_name (G_TYPE_FROM_INSTANCE (object)), NAME (object), action)


/* Callbacks to show what the objects are doing */

static void     containerable_add       (GContainerable *containerable,
                                         GChildable     *childable);
static void     containerable_remove    (GContainerable *containerable,
                                         GChildable     *childable);
static void     containerable_destroy   (gchar          *name);
static void     childable_parent_set    (GChildable     *childable,
                                         GContainerable *old_parent);
static void     childable_destroy       (gchar          *name);
static void     dump_containerable      (GContainerable *containerable);
static void     dump_childable          (GChildable     *childable);


static void
containerable_add (GContainerable *containerable,
                   GChildable     *childable)
{
  DEBUG_OBJECT (childable, "added to '%s'", NAME (containerable));
}

static void
containerable_remove (GContainerable *containerable,
                      GChildable     *childable)
{
  DEBUG_OBJECT (childable, "removed from '%s'", NAME (containerable));
}

static void
containerable_destroy (gchar *name)
{
  DEBUG ("GContainerable", name, "destroyed");
}

static void
childable_parent_set (GChildable     *childable,
                      GContainerable *old_parent)
{
  GContainerable *new_parent = g_childable_get_parent (childable);
  const gchar *old_parent_name;
  const gchar *new_parent_name;

  new_parent = g_childable_get_parent (childable);

  if (old_parent)
    old_parent_name = NAME (old_parent);
  else
    old_parent_name = "";

  if (new_parent)
    new_parent_name = NAME (new_parent);
  else
    new_parent_name = "";

  DEBUG_OBJECT (childable, "changed parent from '%s' to '%s'", old_parent_name, new_parent_name);
}

static void
childable_destroy (gchar *name)
{
  DEBUG ("GChildable", name, "destroyed");
}

static void
dump_containerable (GContainerable *containerable)
{
  g_return_if_fail (G_IS_CONTAINERABLE (containerable));

  g_print ("'%s' containing ( ", NAME (containerable));
  g_containerable_foreach (containerable, G_CALLBACK (dump_childable), NULL);
  g_print (") ");
}

static void
dump_childable (GChildable *childable)
{
  g_return_if_fail (G_IS_CHILDABLE (childable));

  /* A GChildable can also implements GContainerable */
  if (G_IS_CONTAINERABLE (childable))
    {
      dump_containerable (G_CONTAINERABLE (childable));
      return;
    }

  g_print ("'%s' ", NAME (childable));
}


/* Public functions definitions */

GObject *
new_container (const gchar *name)
{
  GObject *container = g_container_new ();
  g_object_set_data (container, "name", (gpointer) name);

  g_signal_connect (container, "add", G_CALLBACK (containerable_add), NULL);
  g_signal_connect (container, "remove", G_CALLBACK (containerable_remove), NULL);
  g_signal_connect (container, "parent-set", G_CALLBACK (childable_parent_set), NULL);

  /* There's no "destroy" or "dispose" signals, so I add a weak
   * reference to catch the object destruction.
   * In weak reference callback, the object is yet died so I can't
   * get his name in the usual way (g_object_get_data). Instead,
   * I'll pass the 'name' argument to the callback (don't do this
   * in real life: 'name' could be a dinamic string!).
   */
  g_object_weak_ref (container, (GWeakNotify) containerable_destroy, (gpointer) name);

  DEBUG_OBJECT (container, "created");

  return container;
}

GObject *
new_bin (const gchar *name)
{
  GObject *bin = g_bin_new ();
  g_object_set_data (bin, "name", (gpointer) name);

  g_signal_connect (bin, "add", G_CALLBACK (containerable_add), NULL);
  g_signal_connect (bin, "remove", G_CALLBACK (containerable_remove), NULL);
  g_signal_connect (bin, "parent-set", G_CALLBACK (childable_parent_set), NULL);

  g_object_weak_ref (bin, (GWeakNotify) containerable_destroy, (gpointer) name);

  DEBUG_OBJECT (bin, "created");

  return bin;
}

GObject *
new_child (const gchar *name)
{
  GObject *child = g_child_new ();
  g_object_set_data (child, "name", (gpointer) name);

  g_signal_connect (child, "parent-set", G_CALLBACK (childable_parent_set), NULL);

  /* As for the container, I catch the destruction trought a weak reference */
  g_object_weak_ref (child, (GWeakNotify) childable_destroy, (gpointer) name);

  DEBUG_OBJECT (child, "created");

  return child;
}

void
show_containerable (GContainerable *containerable)
{
  g_print ("...showing ");
  dump_containerable (containerable);
  g_print ("\n");
}
