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


#include "demo.h"


int
main (int argc, char *argv[])
{
  GObject *container;
  GObject *bin;
  GObject *self_container;
  GObject *child1, *child2, *child3, *child4;
  
  g_print ("Initializing the type system...\n");
  g_type_init ();

  g_print ("\nCreating the containers...\n");
  container = new_container ("container");
  bin = new_bin ("bin");
  self_container = new_container ("self_container");

  g_print ("\nCreating the children...\n");
  child1 = new_child ("child1");
  child2 = new_child ("child2");
  child3 = new_child ("child3");
  child4 = new_child ("child4");

  g_print ("\nAdding 'child1', 'child2' and 'child3' to 'container'...\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child1));
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child2));
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child3));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nDouble referencing 'child2'...\n");
  g_object_ref (child2);

  g_print ("\nRemoving 'child3' and 'child2' from 'container'...\n");
  g_containerable_remove (G_CONTAINERABLE (container), G_CHILDABLE (child3));
  g_containerable_remove (G_CONTAINERABLE (container), G_CHILDABLE (child2));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nUnreferencing 'child1'...\n");
  g_object_unref (child1);
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nAdding 'child4' to 'container'...\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child4));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nReadding 'child2' to 'container'...\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (child2));
  show_containerable (G_CONTAINERABLE (container));

  g_print ("\nMoving 'child2' to 'bin' and removing its double reference...\n");
  g_childable_reparent (G_CHILDABLE (child2), G_CONTAINERABLE (bin));
  g_object_unref (child2);
  show_containerable (G_CONTAINERABLE (container));
  show_containerable (G_CONTAINERABLE (bin));

  g_print ("\nMoving 'child2' to 'container'...\n");
  g_childable_reparent (G_CHILDABLE (child2), G_CONTAINERABLE (container));
  show_containerable (G_CONTAINERABLE (container));
  show_containerable (G_CONTAINERABLE (bin));

  g_print ("\nAdding 'container' inside 'bin'...\n"
           "The GContainer itsself implements GChildable.\n");
  g_containerable_add (G_CONTAINERABLE (bin), G_CHILDABLE (container));
  show_containerable (G_CONTAINERABLE (bin));

  g_print ("\nAdding 'bin' to 'container'...\n"
           "This can be done and, yes, there will be a mutual recursion between\n"
           "'container' and 'bin', so I can't show any container contents.\n");
  g_containerable_add (G_CONTAINERABLE (container), G_CHILDABLE (bin));

  g_print ("\nAdding 'self_container' to 'self_container'...\n");
  g_print ("Also this can be done: there are no reasons to avoid this operation.\n");
  g_containerable_add (G_CONTAINERABLE (self_container), G_CHILDABLE (self_container));

  g_print ("\nMoving 'bin' to 'self_container'...\n");
  g_childable_reparent (G_CHILDABLE (bin), G_CONTAINERABLE (self_container));

  g_print ("\nDestroying 'self_container' (this will destroy all)...\n");
  g_object_unref (self_container);

  return 0;
}
