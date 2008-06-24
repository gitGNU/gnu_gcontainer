/* gcontainer - A generic container for the glib-2.0 library
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


/**
 * SECTION:gchildable
 * @short_description: A generic interface for children objects
 * @stability: Unstable
 *
 * The #GChildable interface provides a generic interface for objects that
 * can be children of a #GContainerable. It is an interface, so can be
 * implemented by objects at any hierarchy level.
 *
 * If you prefer to derive your child object from a base class, take a look
 * to #GChild, that is the natural implementation of the #GChildable interface.
 **/

/**
 * GChildable:
 *
 * Dummy type of the #GChildableIface interface.
 **/

/**
 * GChildableIface:
 * @base_iface:		the base interface.
 * @get_parent:         returns the current parent.
 * @set_parent:		set the new parent.
 * @parent_set:		signal handler for the #GChildable::parent_set
 *			signals.
 *
 * The virtual methods @get_parent and @set_parent must be defined
 * by all the types which implement this interface.
 **/


#include "gchildable.h"
#include "gcontainerable.h"
#include "gobjectmissings.h"
#include "gcontainerintl.h"

#define G_CHILDABLE_IS_DISPOSING(obj)   ((gboolean) GPOINTER_TO_INT (g_object_get_qdata ((GObject *) (obj), quark_disposing)))
#define G_CHILDABLE_SET_DISPOSING(obj)  g_object_set_qdata ((GObject *) (obj), quark_disposing, GINT_TO_POINTER ((gint) TRUE))


enum
{
  PARENT_SET,
  LAST_SIGNAL
};


static void		iface_base	(GChildableIface *iface);
static void		iface_init	(GChildableIface *iface);
static GContainerable *	get_parent	(GChildable	*childable);
static void		set_parent	(GChildable	*childable,
					 GContainerable	*parent);

static GQuark		quark_disposing = 0;
static guint		signals[LAST_SIGNAL] = { 0 };


GType
g_childable_get_type (void)
{
  static GType childable_type = 0;
  
  if G_UNLIKELY (childable_type == 0)
    {
      static const GTypeInfo childable_info =
      {
	sizeof (GChildableIface),
	(GBaseInitFunc)		iface_base,
	NULL,			/* base_finalize */
	(GClassInitFunc)	iface_init,
	NULL,			/* iface_finalize */
      };

      childable_type = g_type_register_static (G_TYPE_INTERFACE, "GChildable",
					       &childable_info, 0);
      g_type_interface_add_prerequisite (G_TYPE_CHILDABLE, G_TYPE_OBJECT);
    }
  
  return childable_type;
}

static void
iface_base (GChildableIface *iface)
{
  static gboolean initialized = FALSE;
  GParamSpec     *param;

  if (initialized)
    return;

  initialized = TRUE;
  quark_disposing = g_quark_from_static_string ("gchildable-disposing");

  param = g_param_spec_object ("parent",
                               P_("Parent"),
                               P_("The parent of this object. The parent must implement the GContainerable interface"),
                               G_TYPE_OBJECT,
                               G_PARAM_READWRITE);
  g_object_interface_install_property (iface, param);

  /**
   * GChildable::parent-set:
   * @childable: a #GChildable
   * @old_parent: the old parent
   *
   * A new parent is set and the "parent" property was changed
   * accordling.
   **/
  signals[PARENT_SET] = g_signal_new ("parent-set",
                                      G_TYPE_CHILDABLE,
                                      G_SIGNAL_RUN_FIRST,
                                      G_STRUCT_OFFSET (GChildableIface, parent_set),
                                      NULL, NULL,
                                      g_cclosure_marshal_VOID__OBJECT,
                                      G_TYPE_NONE, 1, G_TYPE_OBJECT);
}

static void
iface_init (GChildableIface *iface)
{
  iface->get_parent = get_parent;
  iface->set_parent = set_parent;
}


static GContainerable *
get_parent (GChildable *childable)
{
  g_warning ("GChildable::get_parent not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (childable)));
  return NULL;
}

static void
set_parent (GChildable     *childable,
	    GContainerable *parent)
{
  g_warning ("GChildable::set_parent not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (childable)));
}


/**
 * g_childable_get_parent:
 * @childable: a #GChildable
 *
 * Gets the parent of @childable.
 *
 * Returns: the requested parent, or %NULL on errors
 **/
GContainerable *
g_childable_get_parent (GChildable *childable)
{
  g_return_val_if_fail (G_IS_CHILDABLE (childable), NULL);

  return G_CHILDABLE_GET_IFACE (childable)->get_parent (childable);
}

/**
 * g_childable_set_parent:
 * @childable: a #GChildable
 * @parent: a #GContainerable
 *
 * Sets @parent as parent of @childable,
 * properly handling the references between them.
 *
 * If @childable has yet a parent, this function returns with a warning.
 **/
void
g_childable_set_parent (GChildable     *childable,
			GContainerable *parent)
{
  GChildableIface *childable_iface;
  GContainerable  *old_parent;

  g_return_if_fail (G_IS_CHILDABLE (childable));
  g_return_if_fail (G_IS_CONTAINERABLE (parent));

  childable_iface = G_CHILDABLE_GET_IFACE (childable);
  old_parent = childable_iface->get_parent (childable);

  if (old_parent != NULL)
    {
      g_warning ("Can't set a parent on GChildable which has a parent\n");
      return;
    }

  g_object_ref_sink (childable);
  childable_iface->set_parent (childable, parent);
  g_signal_emit (childable, signals[PARENT_SET], 0, old_parent);
}

/**
 * g_childable_unparent:
 * @childable: a #GChildable
 *
 * Removes the current parent of @childable,
 * properly handling the references between them.
 *
 * If @childable has no parent, this function simply returns.
 **/
void
g_childable_unparent (GChildable *childable)
{
  GChildableIface *childable_iface;
  GContainerable  *old_parent;

  g_return_if_fail (G_IS_CHILDABLE (childable));

  childable_iface = G_CHILDABLE_GET_IFACE (childable);
  old_parent = childable_iface->get_parent (childable);

  if (old_parent == NULL)
    return;

  childable_iface->set_parent (childable, NULL);
  g_signal_emit (childable, signals[PARENT_SET], 0, old_parent);

  if (!G_CHILDABLE_IS_DISPOSING (childable))
    g_object_unref (childable);
}

/**
 * g_childable_reparent:
 * @childable: a #GChildable
 * @parent: an object that implements #GContainerable
 *
 * Moves @childable from the old parent to @parent, handling reference
 * count issues to avoid destroying the object.
 **/
void
g_childable_reparent (GChildable     *childable,
                      GContainerable *parent)
{
  GChildableIface *childable_iface;
  GContainerable  *old_parent;

  g_return_if_fail (G_IS_CHILDABLE (childable));
  g_return_if_fail (G_IS_CONTAINERABLE (parent));

  childable_iface = G_CHILDABLE_GET_IFACE (childable);
  old_parent = childable_iface->get_parent (childable);

  g_return_if_fail (old_parent != NULL);

  if (old_parent == parent)
    return;

  g_object_ref (childable);
  g_containerable_remove (old_parent, childable);
  g_containerable_add (parent, childable);
  g_object_unref (childable);
}

/**
 * g_childable_dispose:
 * @object: a #GObject implementing #GChildable
 *
 * Convenience function to be used in the class initialization of objects
 * implementing the GChildable interface.
 * g_childable_dispose() automatically chain up the dispose method of the
 * parent class of the type in the @object hierarchy which implements
 * #GChildable.
 **/
void
g_childable_dispose (GObject *object)
{
  GChildable      *childable;
  GChildableIface *childable_iface;
  GContainerable  *old_parent;
  GType            instance_type;
  GObjectClass    *parent_class;

  if (G_CHILDABLE_IS_DISPOSING (object))
    return;
      
  G_CHILDABLE_SET_DISPOSING (object);

  childable = (GChildable *) object;
  childable_iface = G_CHILDABLE_GET_IFACE (childable);
  old_parent = childable_iface->get_parent (childable);
  instance_type = G_TYPE_INSTANCE_WHICH_IMPLEMENTS (object, G_TYPE_CHILDABLE);
  parent_class = g_type_class_peek (g_type_parent (instance_type));


  if (old_parent)
    g_containerable_remove (old_parent, childable);

  parent_class->dispose (object);
}
