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
 * @stability: Stable
 *
 * The #GContainerable interface should be implemented by all containers
 * of #GChildable objects. It is an interface, so can be implemented by
 * objects at any hierarchy level.
 *
 * If you prefer to derive your container from a base class, take a look
 * to #GContainer, that is the simplest implementation of the
 * #GContainerable interface.
 **/

/**
 * GContainerable:
 *
 * All the fields in the GContainerable structure are private and should
 * never be accessed directly.
 **/

/**
 * GContainerableIface:
 * @base_iface:		the base interface.
 * @add:		signal handler for #GContainerable::add signals.
 *			The default method adds a node to the children list,
 *			g_object_ref_sink() the child and g_object_ref() the
 *			parent.
 * @remove:		signal handler for #GContainerable::remove signals.
 *			The default method reverses the operations made by the
 *			add signal.
 * @foreach:		overridable method. The default method invokes a
 *			callback for every children.
 * @propagate_valist:   overridable method. The default method calls
 *                      g_signal_emit_valist() with the passed-in parameters
 *                      for every child.
 * @children_offset:	the offset of the children pointer in the implemented
 * 			object: it must reference an empty #GList.
 * 			This offset can be easely set using the in the
 * 			interface initialization using the G_STRUCT_OFFSET()
 * 			macro.
 * @object_parent_class:parent class of the implemented object. It should be
 * 			set in the interface initialization.
 *
 * All the fields in the GContainerableIface structure must be set only
 * during the interface initialization. In other cases, these fields must
 * be considered read-only.
 **/


#include "gcontainerable.h"
#include "gcontainerintl.h"


/* Property id from 100 to 109 reserved for GChildable
   Property id from 110 to 119 reserved for GContainerable

   There is a more elegant way to manage properties in interfaces? */
enum
{
  PROP_0 = 110,
  PROP_CHILD
};

enum
{
  ADD,
  REMOVE,
  LAST_SIGNAL
};


static void	g_containerable_base_init	(gpointer	 iface);
static void	g_containerable_iface_init	(GContainerableIface *iface);
static void	g_containerable_dispose		(GObject	*object);
static void	g_containerable_set_property	(GObject	*object,
						 guint		 prop_id,
						 const GValue	*value,
						 GParamSpec	*pspec);

static void	g_containerable_real_add	(GContainerable	*containerable,
						 GObject	*childable);
static void	g_containerable_real_remove	(GContainerable *containerable,
						 GObject	*childable);
static void	g_containerable_real_foreach	(GContainerable	*containerable,
						 GCallback	 callback,
						 gpointer	 user_data);
static void     g_containerable_real_propagate_valist
                                                (GContainerable *containerable,
                                                 guint           signal_id,
                                                 GQuark          detail,
                                                 va_list         var_args);


static guint	containerable_signals[LAST_SIGNAL] = { 0 };


GType
g_containerable_get_type (void)
{
  static GType containerable_type = 0;
  
  if G_UNLIKELY (containerable_type == 0)
    {
      static const GTypeInfo containerable_info =
      {
	sizeof (GContainerableIface),
	(GBaseInitFunc)		g_containerable_base_init,
	NULL,			/* base_finalize */
	(GClassInitFunc)	g_containerable_iface_init,
	NULL,			/* iface_finalize */
      };

      containerable_type = g_type_register_static (G_TYPE_INTERFACE,
						   "GContainerable",
						   &containerable_info, 0);
    }
  
  return containerable_type;
}

static void
g_containerable_base_init (gpointer iface)
{
  static gboolean initialized = FALSE;

  if G_UNLIKELY (! initialized)
    {
      /**
       * GContainerable::add:
       * @containerable: a #GContainerable
       * @childable: a #Gobject implementing #GChildable
       *
       * Adds @childable to the children list of @containerable.
       **/
      containerable_signals[ADD] =
	g_signal_new ("add",
		      G_TYPE_CONTAINERABLE,
		      G_SIGNAL_RUN_LAST,
		      G_STRUCT_OFFSET (GContainerableIface, add),
		      NULL, NULL,
		      g_cclosure_marshal_VOID__OBJECT,
		      G_TYPE_NONE, 1, G_TYPE_OBJECT);

      /**
       * GContainerable::remove:
       * @containerable: a #GContainerable
       * @childable: a #Gobject implementing #GChildable
       *
       * Removes @childable from the children list of @containerable.
       **/
      containerable_signals[REMOVE] =
	g_signal_new ("remove",
		      G_TYPE_CONTAINERABLE,
		      G_SIGNAL_RUN_LAST,
		      G_STRUCT_OFFSET (GContainerableIface, remove),
		      NULL, NULL,
		      g_cclosure_marshal_VOID__OBJECT,
		      G_TYPE_NONE, 1, G_TYPE_OBJECT);

      initialized = TRUE;
    }
}


static void
g_containerable_iface_init (GContainerableIface *iface)
{
  iface->add = g_containerable_real_add;
  iface->remove = g_containerable_real_remove;
  iface->foreach = g_containerable_real_foreach;
  iface->propagate_valist = g_containerable_real_propagate_valist;
}

static void
g_containerable_dispose (GObject *object)
{
  GContainerableIface *iface;
  GList              **p_children;

  g_return_if_fail (G_IS_CONTAINERABLE (object));
  iface = G_CONTAINERABLE_GET_IFACE (object);
  g_return_if_fail (iface->children_offset > 0);
  p_children = G_STRUCT_MEMBER_P (object, iface->children_offset);

  while (*p_children)
    g_signal_emit (object, containerable_signals[REMOVE], 0, (*p_children)->data);
}


static void
g_containerable_set_property (GObject      *object,
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
      break;
    }
}


static void
g_containerable_real_add (GContainerable *containerable,
			  GObject        *childable)
{
  GContainerableIface *iface;

  iface = G_CONTAINERABLE_GET_IFACE (containerable);

  if (iface->children_offset <= 0)
    {
      g_signal_stop_emission (containerable, containerable_signals[ADD], 0);
      g_return_if_reached ();
    }

  g_childable_set_parent ((GChildable *) childable, containerable);

  if (g_childable_get_parent ((GChildable *) childable) == containerable)
    {
      GList **p_children;
      p_children = G_STRUCT_MEMBER_P (containerable, iface->children_offset);
      *p_children = g_list_append (*p_children, childable);
    }
}

static void
g_containerable_real_remove (GContainerable *containerable,
			     GObject        *childable)
{
  GContainerableIface *iface;
  GList          **p_children;

  iface = G_CONTAINERABLE_GET_IFACE (containerable);

  if (iface->children_offset <= 0)
    {
      g_signal_stop_emission (containerable, containerable_signals[REMOVE], 0);
      g_return_if_reached ();
    }

  g_childable_unparent ((GChildable *) childable);

  p_children = G_STRUCT_MEMBER_P (containerable, iface->children_offset);
  *p_children = g_list_remove (*p_children, childable);
}

static void
g_containerable_real_propagate_valist (GContainerable *containerable,
                                       guint           signal_id,
                                       GQuark          detail,
                                       va_list         var_args)
{
  GContainerableIface *iface;
  GList              **p_children;
  GList               *child;

  iface = G_CONTAINERABLE_GET_IFACE (containerable);
  g_return_if_fail (iface->children_offset > 0);
  p_children = G_STRUCT_MEMBER_P (containerable, iface->children_offset);

  for (child = *p_children; child; child = child->next)
    g_signal_emit_valist (child->data, signal_id, detail, var_args);
}


static void
g_containerable_real_foreach (GContainerable *containerable,
			      GCallback       callback,
			      gpointer        user_data)
{
  GContainerableIface *iface;
  GList              **p_children;
  GList               *child;

  iface = G_CONTAINERABLE_GET_IFACE (containerable);
  g_return_if_fail (iface->children_offset > 0);
  p_children = G_STRUCT_MEMBER_P (containerable, iface->children_offset);

  for (child = *p_children; child; child = child->next)
    ((void (*) (gpointer, gpointer)) callback) (child->data, user_data);
}


/**
 * g_containerable_class_init:
 * @g_class: a #GObjectClass
 * @class_data: not used
 *
 * Default class initialization function to be used
 * by the #GTypeInfo structure as class_init function
 * in registration of the type implementing the
 * #GContainerable interface. You MUST call in any way this
 * function to let the interface initialize some requested
 * stuff, such as the dispose method and the "child" property.
 *
 * This method should only be used inside new type implementations.
 **/
void
g_containerable_class_init (gpointer g_class,
			    gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) g_class;
  GParamSpec   *param;

  gobject_class->dispose      = g_containerable_dispose;
  gobject_class->set_property = g_containerable_set_property;
 
  param = g_param_spec_object ("child",
			       P_("Child"),
			       P_("Can be used to add a new child to the container"),
			       G_TYPE_OBJECT,
			       G_PARAM_WRITABLE);
  g_object_class_install_property (gobject_class, PROP_CHILD, param);
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
		     GObject        *childable)
{
  GContainerable *old_parent;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (G_IS_CHILDABLE (childable));

  old_parent = g_childable_get_parent ((GChildable *) childable);

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

  g_signal_emit (containerable, containerable_signals[ADD], 0, childable);
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
			GObject        *childable)
{
  GContainerable *parent;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (G_IS_CHILDABLE (childable));

  parent = g_childable_get_parent ((GChildable *) childable);

  g_return_if_fail (parent == containerable);

  g_signal_emit (containerable, containerable_signals[REMOVE], 0, childable);
}

/**
 * g_containerable_get_children:
 * @containerable: a #GContainerable
 *
 * Gets the children list pointer of @containerable.
 *
 * Return value: the requested pointer or %NULL on error
 **/
GList *
g_containerable_get_children (GContainerable *containerable)
{
  GContainerableIface *iface;
  GList              **p_children;

  g_return_val_if_fail (G_IS_CONTAINERABLE (containerable), NULL);

  iface = G_CONTAINERABLE_GET_IFACE (containerable);

  if (iface->children_offset <= 0)
    return NULL;

  p_children = G_STRUCT_MEMBER_P (containerable, iface->children_offset);

  return *p_children;
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
  GContainerableIface *iface;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));
  g_return_if_fail (callback != NULL);

  iface = G_CONTAINERABLE_GET_IFACE (containerable);

  iface->foreach (containerable, callback, user_data);
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

  va_start (var_args, detail);
  g_containerable_propagate_valist (containerable, signal_id, detail, var_args);
  va_end (var_args);
}

/**
 * g_containerable_propagate_valist:
 * @containerable: a #GContainerable
 * @signal_id: the signal id
 * @detail: the detail
 * @...: a list of parameters to be passed to the signal, followed by a location
 *       for the return value. If the return type of the signal is G_TYPE_NONE,
 *       the return value location can be omitted.
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
  GContainerableIface *iface;

  g_return_if_fail (G_IS_CONTAINERABLE (containerable));

  iface = G_CONTAINERABLE_GET_IFACE (containerable);

  iface->propagate_valist (containerable, signal_id, detail, var_args);
}

