/* libgcontainer - Test program
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


#include <gcontainer/gcontainer.h>


/*
 * Debugging
 *
 * This functions show what the application is doing.
 */

#define	NAME(object) \
  g_object_get_data ((GObject *) object, "name")

#define DEBUG_ACTION(object,action...) \
  DEBUG(g_type_name (G_TYPE_FROM_INSTANCE (object)), NAME (object), action)

#define DEBUG(type,name,action...) \
  G_STMT_START { \
      g_printf ("    -> %s '%s' ", type, name); \
      g_printf (action); \
      g_printf ("\n"); \
  } G_STMT_END

#define MESSAGE(object,action) \
  g_strdup_printf ("    -> %s '%s' %s\n", \
		   g_type_name (G_TYPE_FROM_INSTANCE (object)), \
		   g_object_get_data ((GObject *) object, "name"), \
		   action)



void
container_add (GContainer *containerable,
	       GChildable *childable)
{
  DEBUG_ACTION (childable, "added to '%s'", NAME (containerable));
}

void
container_remove (GContainer *containerable,
		  GChildable *childable)
{
  DEBUG_ACTION (childable, "removed from '%s'", NAME (containerable));
}

void
container_destroy (gchar *name)
{
  DEBUG ("GContainer", name, "destroyed");
}

void
child_destroy (gchar *name)
{
  DEBUG ("GChild", name, "destroyed");
}


void
dump_child (GObject *child)
{
  g_return_if_fail (G_IS_CHILD (child));

  g_print ("[%s] ", NAME (child));
}

void
dump_container (GObject *container)
{
  g_print ("...'%s' contains ' ", NAME (container));
  
  g_containerable_foreach (G_CONTAINERABLE (container),
			   G_CALLBACK (dump_child), NULL);

  g_print ("'\n\n");
}



/*
 * Object creation
 *
 * This functions create a GContainer and a GChild with
 * attached debugging stuff.
 */

GObject *
new_container (const gchar *name)
{
  GObject *container = g_container_new ();
  g_object_set_data (container, "name", (gpointer) name);

  g_signal_connect (container, "add", G_CALLBACK (container_add), NULL);
  g_signal_connect (container, "remove", G_CALLBACK (container_remove), NULL);

  /* There's no "destroy" or "dispose" signals, so I add a weak
   * reference to catch the object destruction.
   * In weak reference callback, the object is yet died so I can't
   * get his name in the usual way (g_object_get_data). Instead,
   * I'll pass the 'name' argument to the callback (don't do this
   * in real life: 'name' could be a dinamic string!).
   */
  g_object_weak_ref (container, (GWeakNotify) container_destroy, (gpointer) name);

  DEBUG_ACTION (container, "created");

  return container;
}

GObject *
new_child (const gchar *name)
{
  GObject *child = g_child_new ();
  g_object_set_data (child, "name", (gpointer) name);

  /* As for the container, I catch the destruction trought a weak reference */
  g_object_weak_ref (child, (GWeakNotify) child_destroy, (gpointer) name);

  DEBUG_ACTION (child, "created");

  return child;
}



/*
 * Main application
 */

int
main (int argc, char *argv[])
{
  GObject *container;
  GObject *child1, *child2, *child3, *child4;
  
  g_print ("Initializing the type system...\n");
  g_type_init ();

  g_print ("Creating the container...\n");
  container = new_container ("container");
  g_object_ref_sink (container);
  dump_container (container);

  g_print ("Creating the children...\n");
  child1 = new_child ("first");
  child2 = new_child ("second");
  child3 = new_child ("third");
  child4 = new_child ("forth");
  dump_container (container);

  g_print ("Adding three children...\n");
  g_containerable_add (G_CONTAINERABLE (container), child1);
  g_containerable_add (G_CONTAINERABLE (container), child2);
  g_containerable_add (G_CONTAINERABLE (container), child3);
  dump_container (container);

  g_print ("Double referencing the 'second' child...\n");
  g_object_ref (child2);
  dump_container (container);

  g_print ("Removing 'third' and 'second' children...\n");
  g_containerable_remove (G_CONTAINERABLE (container), child3);
  g_containerable_remove (G_CONTAINERABLE (container), child2);
  dump_container (container);

  g_print ("Unreferencing the 'first' child...\n");
  g_object_unref (child1);
  dump_container (container);

  g_print ("Adding the 'forth' child...\n");
  g_containerable_add (G_CONTAINERABLE (container), child4);
  dump_container (container);

  g_print ("Readding the 'second' child...\n");
  g_containerable_add (G_CONTAINERABLE (container), child2);
  dump_container (container);

  g_print ("Unreferencing the second child...\n");
  g_object_unref (child2);
  dump_container (container);

  g_print ("Destroying the container...\n");
  g_object_unref (container);

  return 0;
}

