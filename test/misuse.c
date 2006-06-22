/* libgcontainer - Test program.
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


#include "demo.h"


int
main (int argc, char *argv[])
{
  GObject *container;
  GObject *bin;
  GObject *child1, *child2;
  
  g_print ("Initializing the type system...\n");
  g_type_init ();

  g_print ("\nCreating the containers...\n");
  container = new_container ("container");
  bin = new_bin ("bin");

  g_print ("\nCreating the children...\n");
  child1 = new_child ("child1");
  child2 = new_child ("child2");

  g_print ("\nAdding 'child1' to 'container'...\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child1));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nTrying to readd 'child1' to 'container'...\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child1));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nTrying to add 'child1' to 'bin'...\n");
  g_containerable_add (G_CONTAINERABLE (bin), G_CHILDABLE (child1));
  show_containerable (G_CONTAINERABLE (bin));

  g_print ("\nTrying to remove 'child2' from 'container'...\n");
  g_containerable_remove (G_CONTAINERABLE (container), G_CHILDABLE (child2));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nTrying to move 'child1' from 'container' to 'container'...\n");
  g_childable_reparent (G_CHILDABLE (child1), G_CONTAINERABLE (container));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nTrying to move 'child2' from unknowned to 'bin'...\n");
  g_childable_reparent (G_CHILDABLE (child2), G_CONTAINERABLE (bin));
  show_containerable (G_CONTAINERABLE (bin));

  g_print ("\nDestroying all...\n");
  g_object_unref (child2);
  g_object_unref (child1);
  g_object_unref (bin);
  g_object_unref (container);

  return 0;
}
