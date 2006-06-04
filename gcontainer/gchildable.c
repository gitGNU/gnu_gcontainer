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
 * SECTION:gchildable
 * @short_description: A generic interface for children objects
 * @stability: Stable
 *
 * The #GChildable interface provides a generic interface for objects that
 * can be children of a #GContainerable. It is an interface, so can be
 * implemented by objects at any hierarchy level.
 *
 * If you prefer to derive your child object from a base class, take a look
 * to #GChild, that is the simplest implementation of the
 * #GChildable interface.
 **/

/**
 * GChildable:
 *
 * All the fields in the GChildable structure are private and should never
 * be accessed directly.
 **/

/**
 * GChildableIface:
 * @base_iface:		the base interface.
 * @set_parent:		signal handler for the #GChildable::set_parent
 *			signals. The default method sets the
 *			#GChildable:parent property to the requested parent.
 * @parent_set:		signal handler for the #GChildable::parent_set
 *			signals. There's no default method (it is set to %NULL).
 * @parent_offset:	the offset of the parent pointer in the implemented
 * 			object. This offset can be easely set in the interface
 * 			initialization using the G_STRUCT_OFFSET() macro.
 * @object_parent_class:parent class of the implemented object. It should be
 * 			set in the interface initialization.
 *
 * All the fields in the GChildableIface structure must be set only
 * during the interface initialization. In other cases, these fields must
 * be considered read-only.
 **/


#include "gchildable.h"
#include "gcontainerable.h"
#include "gcontainerintl.h"


/* Property id from 100 to 109 reserved for GChildable
   Property id from 110 to 119 reserved for GContainerable

   There is a more elegant way to manage properties in interfaces? */
enum
{
  PROP_0 = 100,
  PROP_PARENT
};

enum
{
  SET_PARENT,
  PARENT_SET,
  LAST_SIGNAL
};


static void	g_childable_base_init		(gpointer	 iface);
static void	g_childable_iface_init		(GChildableIface*iface);
static void	g_childable_dispose		(GObject	*object);
static void	g_childable_get_property	(GObject	*object,
						 guint		 prop_id,
						 GValue		*value,
						 GParamSpec	*pspec);
static void	g_childable_set_property	(GObject	*object,
						 guint		 prop_id,
						 const GValue	*value,
						 GParamSpec	*pspec);
static void	g_childable_real_set_parent	(GChildable     *childable,
						 GContainerable *new_parent);


static guint	signals[LAST_SIGNAL] = { 0 };


GType
g_childable_get_type (void)
{
  static GType childable_type = 0;
  
  if G_UNLIKELY (childable_type == 0)
    {
      static const GTypeInfo childable_info =
      {
	sizeof (GChildableIface),
	(GBaseInitFunc)		g_childable_base_init,
	NULL,			/* base_finalize */
	(GClassInitFunc)	g_childable_iface_init,
	NULL,			/* iface_finalize */
      };

      childable_type = g_type_register_static (G_TYPE_INTERFACE, "GChildable",
					       &childable_info, 0);
    }
  
  return childable_type;
}

static void
g_childable_base_init (gpointer iface)
{
  static gboolean initialized = FALSE;

  if G_UNLIKELY (! initialized)
    {
      /**
       * GChildable::set-parent:
       * @childable: a #GChildable
       * @new_parent: a #GObject implementing #GContainerable
       *
       * A new parent is set and the "parent" property is changed
       * accordling. When finished, a #GChildable::parent-set signal
       * is emitted.
       **/
      signals[SET_PARENT] =
	g_signal_new ("set-parent",
		      G_TYPE_CHILDABLE,
		      G_SIGNAL_RUN_LAST,
		      G_STRUCT_OFFSET (GChildableIface, set_parent),
		      NULL, NULL,
		      g_cclosure_marshal_VOID__OBJECT,
		      G_TYPE_NONE, 1, G_TYPE_OBJECT);

      /**
       * GChildable::parent-set:
       * @childable: a #GChildable
       * @old_parent: a #GObject implementing #GContainerable
       *
       * A new parent is set.
       **/
      signals[PARENT_SET] =
	g_signal_new ("parent-set",
		      G_TYPE_CHILDABLE,
		      G_SIGNAL_RUN_FIRST,
		      G_STRUCT_OFFSET (GChildableIface, parent_set),
		      NULL, NULL,
		      g_cclosure_marshal_VOID__OBJECT,
		      G_TYPE_NONE, 1, G_TYPE_OBJECT);

      initialized = TRUE;
    }
}

static void
g_childable_iface_init (GChildableIface *iface)
{
  iface->set_parent = g_childable_real_set_parent;
  iface->parent_set = NULL;
}

static void
g_childable_dispose (GObject *object)
{
  GChildableIface *iface;
  
  iface = G_CHILDABLE_GET_IFACE (object);

  if (iface->parent_offset > 0)
    {
      GContainerable **p_parent;
      
      p_parent = G_STRUCT_MEMBER_P (object, iface->parent_offset);

      if (*p_parent != NULL)
	{
	  /* This reference will be removed by g_childable_unparent
	   * called by g_containerable_remove */
	  g_object_ref (object);
	  g_containerable_remove (*p_parent, object);
	}
    }

  if (iface->object_parent_class)
    ((GObjectClass *) iface->object_parent_class)->dispose (object);
}


static void
g_childable_get_property (GObject    *object,
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
g_childable_set_property (GObject      *object,
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

static void
g_childable_real_set_parent (GChildable     *childable,
			     GContainerable *new_parent)
{
  GChildableIface *iface;
  GContainerable **p_parent;
  GContainerable  *old_parent;    
  
  iface = G_CHILDABLE_GET_IFACE (childable);
  g_return_if_fail (iface->parent_offset > 0);
  p_parent = G_STRUCT_MEMBER_P (childable, iface->parent_offset);
  old_parent = *p_parent;

  if (new_parent == old_parent)
    return;

  if (old_parent == NULL)
    g_object_ref_sink (childable);

  *p_parent = new_parent;
  g_signal_emit (childable, signals[PARENT_SET], 0, old_parent);
  g_object_notify ((GObject *) childable, "parent");

  if (new_parent == NULL)
    g_object_unref (childable);
}


/**
 * g_childable_class_init:
 * @g_class: a #GObjectClass
 * @class_data: not used
 *
 * Default class initialization function to be used
 * by the #GTypeInfo structure as class_init function
 * in registration of the type implementing the
 * #GChildable interface. You MUST call in any way this
 * function to let the interface initialize some requested
 * stuff, such as the dispose method and the "parent" property.
 *
 * This method should only be used inside new type implementations.
 **/
void
g_childable_class_init (gpointer g_class,
			gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) g_class;
  GParamSpec   *param;

  gobject_class->dispose      = g_childable_dispose;
  gobject_class->get_property = g_childable_get_property;
  gobject_class->set_property = g_childable_set_property;
 
  param = g_param_spec_object ("parent",
			       P_("Parent"),
			       P_("The parent of this object. The parent must implement the GContainerable interface"),
			       G_TYPE_OBJECT,
			       G_PARAM_READWRITE);
  g_object_class_install_property (gobject_class, PROP_PARENT, param);
}


/**
 * g_childable_get_parent:
 * @childable: a #GChildable
 *
 * Gets the parent of @childable.
 *
 * Return value: the requested parent, or %NULL on errors
 **/
GContainerable *
g_childable_get_parent (GChildable *childable)
{
  GChildableIface *iface;
  GContainerable **p_parent;

  g_return_val_if_fail (G_IS_CHILDABLE (childable), NULL);

  iface = G_CHILDABLE_GET_IFACE (childable);

  if (iface->parent_offset <= 0)
    return NULL;

  p_parent = G_STRUCT_MEMBER_P (childable, iface->parent_offset);

  return *p_parent;
}

/**
 * g_childable_set_parent:
 * @childable: a #GChildable
 * @parent: a #GContainerable
 *
 * Sets @parent as parent of @childable,
 * properly handling the references between them.
 **/
void
g_childable_set_parent (GChildable     *childable,
			GContainerable *parent)
{
  g_return_if_fail (G_IS_CHILDABLE (childable));
  g_return_if_fail (G_IS_CONTAINERABLE (parent));
  g_return_if_fail (childable != (GChildable *) parent);

  g_signal_emit (childable, signals[SET_PARENT], 0, parent);
}

/**
 * g_childable_unparent:
 * @childable: a #GChildable
 *
 * Removes the current parent of @childable,
 * properly handling the references between them.
 **/
void
g_childable_unparent (GChildable *childable)
{
  g_return_if_fail (G_IS_CHILDABLE (childable));

  g_signal_emit (childable, signals[SET_PARENT], 0, NULL);
}

