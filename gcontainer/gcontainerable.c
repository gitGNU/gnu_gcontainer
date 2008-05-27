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
 * SECTION:gcontainerable
 * @short_description: A generic interface for #GChildable containers
 * @stability: Unstable
 *
 * The #GContainerable interface should be implemented by all containers of
 * #GChildable objects. It is an interface, so can be implemented by objects
 * at any hierarchy level.
 *
 * If you prefer to derive your container from a base class, take a look to
 * the sample implementations of #GContainerable (#GContainer, #GBin).
 **/

/**
 * GContainerable:
 *
 * Dummy type of the #GContainerableIface interface.
 **/

/**
 * GContainerableIface:
 * @base_iface:		the base interface.
 * @add:		signal handler for #GContainerable::add signals.
 * @remove:		signal handler for #GContainerable::remove signals.
 * @get_children:	returns a newly allocated #GSList containing the
 *                      children list of the container.
 *
 * The virtual methods @add, @remove and @get_children must be defined
 * by all the types which implement this interface.
 **/


#include "gcontainerable.h"
#include "gobjectmissings.h"
#include "gcontainerintl.h"

#define G_CONTAINERABLE_IS_DISPOSING(obj)   ((gboolean) GPOINTER_TO_INT (g_object_get_qdata ((GObject *) (obj), quark_disposing)))
#define G_CONTAINERABLE_SET_DISPOSING(obj)  g_object_set_qdata ((GObject *) (obj), quark_disposing, GINT_TO_POINTER ((gint) TRUE))

enum
{
  ADD,
  REMOVE,
  LAST_SIGNAL
};


static void	g_containerable_iface_init      	(GContainerableIface *iface);
static void     g_containerable_real_add                (GContainerable *containerable,
                                                         GChildable     *childable,
                                                         gpointer        user_data);
static void     g_containerable_real_remove             (GContainerable *containerable,
                                                         GChildable     *childable,
                                                         gpointer        user_data);

static GSList * g_containerable_get_children_unimplemented
                                                        (GContainerable *containerable);
static gboolean g_containerable_add_unimplemented       (GContainerable	*containerable,
                                                         GChildable	*childable);
static gboolean g_containerable_remove_unimplemented    (GContainerable *containerable,
                                                         GChildable	*childable);


static GQuark   quark_disposing = 0;
static guint	signals[LAST_SIGNAL] = { 0 };


GType
g_containerable_get_type (void)
{
  static GType containerable_type = 0;
  
  if G_UNLIKELY (containerable_type == 0)
    {
      static const GTypeInfo containerable_info =
      {
	sizeof (GContainerableIface),
	NULL,                   /* base_init */
	NULL,			/* base_finalize */
	(GClassInitFunc)	g_containerable_iface_init,
	NULL,			/* iface_finalize */
      };

      containerable_type = g_type_register_static (G_TYPE_INTERFACE,
						   "GContainerable",
						   &containerable_info, 0);
      g_type_interface_add_prerequisite (G_TYPE_CONTAINERABLE, G_TYPE_OBJECT);
    }
  
  return containerable_type;
}


static void
g_containerable_iface_init (GContainerableIface *iface)
{
  GParamSpec *param;
  GClosure   *closure;
  GType       param_types[1];

  quark_disposing = g_quark_from_static_string ("gchildable-disposing");

  iface->get_children = g_containerable_get_children_unimplemented;
  iface->add = g_containerable_add_unimplemented;
  iface->remove = g_containerable_remove_unimplemented;

  param = g_param_spec_object ("child",
                               P_("Child"),
                               P_("Can be used to add a new child to the container"),
                               G_TYPE_OBJECT,
                               G_PARAM_WRITABLE);
  g_object_interface_install_property (iface, param);

  /**
   * GContainerable::add:
   * @containerable: a #GContainerable
   * @childable: a #Gobject implementing #GChildable
   *
   * Adds @childable to the children list of @containerable.
   **/
  closure = g_cclosure_new (G_CALLBACK (g_containerable_real_add),
                            (gpointer)0xdeadbeaf, NULL);
  param_types[0] = G_TYPE_OBJECT;
  signals[ADD] = g_signal_newv ("add",
                                G_TYPE_CONTAINERABLE,
                                G_SIGNAL_RUN_FIRST,
                                closure,
                                NULL, NULL,
                                g_cclosure_marshal_VOID__OBJECT,
                                G_TYPE_NONE, 1, param_types);

  /**
   * GContainerable::remove:
   * @containerable: a #GContainerable
   * @childable: a #Gobject implementing #GChildable
   *
   * Removes @childable from the children list of @containerable.
   **/
  closure = g_cclosure_new (G_CALLBACK (g_containerable_real_remove),
                            (gpointer)0xdeadbeaf, NULL);
  param_types[0] = G_TYPE_OBJECT;
  signals[REMOVE] = g_signal_newv ("remove",
                                   G_TYPE_CONTAINERABLE,
                                   G_SIGNAL_RUN_FIRST,
                                   closure,
                                   NULL, NULL,
                                   g_cclosure_marshal_VOID__OBJECT,
                                   G_TYPE_NONE, 1, param_types);
}


static void
g_containerable_real_add (GContainerable *containerable,
                          GChildable     *childable,
                          gpointer        user_data)
{
  GContainerableIface *containerable_iface;
  GContainerable      *old_parent;

  g_assert (user_data == (gpointer) 0xdeadbeaf);

  containerable_iface = G_CONTAINERABLE_GET_IFACE (containerable);
  old_parent = g_childable_get_parent (childable);

  if (old_parent != NULL)
    {
      g_warning ("Attempting to add an object with type %s to a container "
		 "of type %s, but the object is already inside a container "
		 "of type %s.",
                 g_type_name (G_OBJECT_TYPE (childable)),
                 g_type_name (G_OBJECT_TYPE (containerable)),
                 g_type_name (G_OBJECT_TYPE (old_parent)));
      return;
    }


  if (containerable_iface->add (containerable, childable))
    {
      g_childable_set_parent (childable, containerable);
    }
  else
    {
      g_signal_stop_emission (containerable, signals[ADD], 0);
    }
}

static void
g_containerable_real_remove (GContainerable *containerable,
                             GChildable     *childable,
                             gpointer        user_data)
{
  GContainerableIface *containerable_iface;

  g_assert (user_data == (gpointer) 0xdeadbeaf);

  containerable_iface = G_CONTAINERABLE_GET_IFACE (containerable);

  if (containerable_iface->remove (containerable, childable))
    {
      g_childable_unparent (childable);
    }
  else
    {
      g_signal_stop_emission (containerable, signals[REMOVE], 0);
    }
}


static GSList *
g_containerable_get_children_unimplemented (GContainerable *containerable)
{
  g_warning ("GContainerable::get_children not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (containerable)));
  return NULL;
}

static gboolean
g_containerable_add_unimplemented (GContainerable *containerable,
                                   GChildable     *childable)
{
  g_warning ("GContainerable::add not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (containerable)));
  return FALSE;
}

static gboolean
g_containerable_remove_unimplemented (GContainerable *containerable,
                                      GChildable     *childable)
{
  g_warning ("GContainerable::remove not implemented for `%s'",
             g_type_name (G_TYPE_FROM_INSTANCE (containerable)));
  return FALSE;
}


/**
 * g_containerable_add:
 * @containerable: a #GContainerable
 * @childable: a #Gobject implementing #GChildable
 *
 * Emits a #GContainerable::add signal on @containerable
 * passing @childable as argument.
 *
 * A #GChildable implemented object may be added to only one
 * container at a time; you can't place the same child inside
 * two different containers.
 **/
void
g_containerable_add (GContainerable *containerable,
		     GChildable     *childable)
{
  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (G_IS_CHILDABLE (childable));

  g_signal_emit (containerable, signals[ADD], 0, childable);
}

/**
 * g_containerable_remove:
 * @containerable: a #GContainerable
 * @childable: a #Gobject implementing #GChildable
 *
 * Emits a #GContainerable::remove signal on @containerable
 * passing @childable as argument.
 *
 * @childable must be inside @containerable.
 * Note that @containerable will own a reference to @childable
 * and that this may be the last reference held; so removing a
 * child from its container can destroy that child.
 * If you want to use @childable again, you need to add a reference
 * to it, using g_object_ref(), before remove it from @containerable.
 * If you don't want to use @childable again, it's usually more
 * efficient to simply destroy it directly using g_object_unref()
 * since this will remove it from the container and help break any
 * circular reference count cycles.
 **/
void
g_containerable_remove (GContainerable *containerable,
			GChildable     *childable)
{
  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (G_IS_CHILDABLE (childable));

  g_signal_emit (containerable, signals[REMOVE], 0, childable);
}

/**
 * g_containerable_get_children:
 * @containerable: a #GContainerable
 *
 * Gets the children list of @containerable.
 * This list must be manually freed when no longer user.
 *
 * Return value: a newly allocated #GSList or %NULL on error.
 **/
GSList *
g_containerable_get_children (GContainerable *containerable)
{
  GContainerableIface *iface;

  g_return_val_if_fail (G_IS_CONTAINERABLE (containerable), NULL);

  return G_CONTAINERABLE_GET_IFACE (containerable)->get_children (containerable);
}

/**
 * g_containerable_foreach:
 * @containerable: a #GContainerable
 * @callback: a callback
 * @user_data: callback user data
 * 
 * Invokes @callback on each child of @containerable.
 **/
void
g_containerable_foreach (GContainerable *containerable,
			 GCallback       callback,
			 gpointer        user_data)
{
  GSList *children;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (callback != NULL);

  children = g_containerable_get_children (containerable);

  while (children)
    {
      if (children->data)
        ((void (*) (gpointer, gpointer)) callback) (children->data, user_data);

      children = g_slist_delete_link (children, children);
    }
}

/**
 * g_containerable_propagate:
 * @containerable: a #GContainerable
 * @signal_id: the signal id
 * @detail: the detail
 * @...: parameters to be passed to the signal, followed by a location for the
 *       return value. If the return type of the signal is G_TYPE_NONE, the return
 *       value location can be omitted.
 *
 * Emits the specified signal to all the children of @containerable
 * using g_signal_emit_valist() calls.
 **/
void
g_containerable_propagate (GContainerable *containerable,
                           guint           signal_id,
                           GQuark          detail,
                           ...)
{
  va_list var_args;

  va_start (var_args, detail);
  g_containerable_propagate_valist (containerable, signal_id, detail, var_args);
  va_end (var_args);
}

/**
 * g_containerable_propagate_by_name:
 * @containerable: a #GContainerable
 * @detailed_signal: a string of the form "signal-name::detail".
 * @...: a list of parameters to be passed to the signal, followed by a location
 *       for the return value. If the return type of the signal is G_TYPE_NONE,
 *       the return value location can be omitted.
 *
 * Emits the specified signal to all the children of @containerable
 * using g_signal_emit_valist() calls.
 **/
void
g_containerable_propagate_by_name (GContainerable *containerable,
                                   const gchar    *detailed_signal,
                                   ...)
{
  guint   signal_id;
  GQuark  detail = 0;
  va_list var_args;

  if (! g_signal_parse_name (detailed_signal, G_TYPE_FROM_INSTANCE (containerable),
                             &signal_id, &detail, FALSE))
    {
      g_warning ("%s: signal `%s' is invalid for instance `%p'",
                 G_STRLOC, detailed_signal, containerable);
      return;
    }

  va_start (var_args, detailed_signal);
  g_containerable_propagate_valist (containerable, signal_id, detail, var_args);
  va_end (var_args);
}

/**
 * g_containerable_propagate_valist:
 * @containerable: a #GContainerable
 * @signal_id: the signal id
 * @detail: the detail
 * @var_args: a list of parameters to be passed to the signal, followed by a
 *            location for the return value. If the return type of the signal
 *            is G_TYPE_NONE, the return value location can be omitted.
 *
 * Emits the specified signal to all the children of @containerable
 * using g_signal_emit_valist() calls.
 **/
void
g_containerable_propagate_valist (GContainerable *containerable,
                                  guint           signal_id,
                                  GQuark          detail,
                                  va_list         var_args)
{
  GSList *children;
  va_list var_copy;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));

  children = g_containerable_get_children (containerable);

  while (children)
    {
      if (children->data)
	{
	  G_VA_COPY (var_copy, var_args);
	  g_signal_emit_valist (children->data, signal_id, detail, var_copy);
	}

      children = g_slist_delete_link (children, children);
    }
}

/**
 * g_containerable_dispose:
 * @object: a #GObject implementing #GContainerable
 *
 * Convenience function to be used in the class initialization of objects
 * implementing the GContainerable interface.
 * g_containerable_dispose() automatically chain up the dispose method of the
 * parent class of the type in the @object hierarchy which implements
 * #GContainerable.
 **/
void
g_containerable_dispose (GObject *object)
{
  GContainerable      *containerable;
  GSList              *children;
  GChildable          *child;
  GType                instance_type;
  GObjectClass        *parent_class;
 
  if (G_CONTAINERABLE_IS_DISPOSING (object))
    return;

  G_CONTAINERABLE_SET_DISPOSING (object);
      
  containerable = (GContainerable *) object;
  children = G_CONTAINERABLE_GET_IFACE (object)->get_children (containerable);
  instance_type = G_TYPE_INSTANCE_WHICH_IMPLEMENTS (object, G_TYPE_CONTAINERABLE);
  parent_class = g_type_class_peek (g_type_parent (instance_type));

  while (children)
    {
      child = (GChildable *) children->data;

      if (child)
        g_signal_emit (containerable, signals[REMOVE], 0, child);

      children = g_slist_delete_link (children, children);
    }

  parent_class->dispose (object);
}
