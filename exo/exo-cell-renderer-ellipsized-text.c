/* $Id$ */
/*-
 * Copyright (c) 2004-2006 os-cillation e.K.
 *
 * Written by Benedikt Meurer <benny@xfce.org>.
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
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <exo/exo-cell-renderer-ellipsized-text.h>
#include <exo/exo-private.h>
#include <exo/exo-alias.h>



#define EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT, ExoCellRendererEllipsizedTextPrivate))



/* Property identifiers */
enum
{
  PROP_0,
  PROP_FOLLOW_STATE,
};



static void exo_cell_renderer_ellipsized_text_class_init    (ExoCellRendererEllipsizedTextClass *klass);
static void exo_cell_renderer_ellipsized_text_get_property  (GObject                            *object,
                                                             guint                               prop_id,
                                                             GValue                             *value,
                                                             GParamSpec                         *pspec);
static void exo_cell_renderer_ellipsized_text_set_property  (GObject                            *object,
                                                             guint                               prop_id,
                                                             const GValue                       *value,
                                                             GParamSpec                         *pspec);
static void exo_cell_renderer_ellipsized_text_get_size      (GtkCellRenderer                    *renderer,
                                                             GtkWidget                          *widget,
                                                             GdkRectangle                       *cell_area,
                                                             gint                               *x_offset,
                                                             gint                               *y_offset,
                                                             gint                               *width,
                                                             gint                               *height);
static void exo_cell_renderer_ellipsized_text_render        (GtkCellRenderer                    *renderer,
                                                             GdkWindow                          *window,
                                                             GtkWidget                          *widget,
                                                             GdkRectangle                       *background_area,
                                                             GdkRectangle                       *cell_area,
                                                             GdkRectangle                       *expose_area,
                                                             GtkCellRendererState                flags);



struct _ExoCellRendererEllipsizedTextPrivate
{
  gboolean follow_state;
};



static GObjectClass *exo_cell_renderer_ellipsized_text_parent_class;



GType
exo_cell_renderer_ellipsized_text_get_type (void)
{
  static GType type = G_TYPE_INVALID;

  if (G_UNLIKELY (type == G_TYPE_INVALID))
    {
      type = _exo_g_type_register_simple (GTK_TYPE_CELL_RENDERER_TEXT,
                                          "ExoCellRendererEllipsizedText",
                                          sizeof (ExoCellRendererEllipsizedTextClass),
                                          exo_cell_renderer_ellipsized_text_class_init,
                                          sizeof (ExoCellRendererEllipsizedText),
                                          NULL);
    }

  return type;
}



static void
exo_cell_renderer_ellipsized_text_class_init (ExoCellRendererEllipsizedTextClass *klass)
{
  GtkCellRendererClass *gtkcell_renderer_class;
  GObjectClass         *gobject_class;

  /* determine the parent type class */
  exo_cell_renderer_ellipsized_text_parent_class = g_type_class_peek_parent (klass);

  /* add our private data to the type's instances */
  g_type_class_add_private (klass, sizeof (ExoCellRendererEllipsizedTextPrivate));

  gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->get_property = exo_cell_renderer_ellipsized_text_get_property;
  gobject_class->set_property = exo_cell_renderer_ellipsized_text_set_property;

  gtkcell_renderer_class = GTK_CELL_RENDERER_CLASS (klass);
  gtkcell_renderer_class->get_size = exo_cell_renderer_ellipsized_text_get_size;
  gtkcell_renderer_class->render = exo_cell_renderer_ellipsized_text_render;

  /* initialize the library's i18n support */
  _exo_i18n_init ();

  /**
   * ExoCellRendererEllipsizedText:follow-state:
   *
   * Specifies whether the text renderer should render the text based on
   * the selection state of the items. This is necessary for #ExoIconView
   * which doesn't draw any item state indicators itself.
   *
   * Since: 0.3.1.9
   **/
  g_object_class_install_property (gobject_class,
                                   PROP_FOLLOW_STATE,
                                   g_param_spec_boolean ("follow-state",
                                                         _("Follow state"),
                                                         _("Render differently based on the selection state."),
                                                         FALSE,
                                                         EXO_PARAM_READWRITE));
}



static void
exo_cell_renderer_ellipsized_text_get_property (GObject    *object,
                                                guint       prop_id,
                                                GValue     *value,
                                                GParamSpec *pspec)
{
  ExoCellRendererEllipsizedTextPrivate *priv = EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_FOLLOW_STATE:
      g_value_set_boolean (value, priv->follow_state);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_cell_renderer_ellipsized_text_set_property (GObject      *object,
                                                guint         prop_id,
                                                const GValue *value,
                                                GParamSpec   *pspec)
{
  ExoCellRendererEllipsizedTextPrivate *priv = EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_FOLLOW_STATE:
      priv->follow_state = g_value_get_boolean (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}



static void
exo_cell_renderer_ellipsized_text_get_size (GtkCellRenderer *renderer,
                                            GtkWidget       *widget,
                                            GdkRectangle    *cell_area,
                                            gint            *x_offset,
                                            gint            *y_offset,
                                            gint            *width,
                                            gint            *height)
{
  ExoCellRendererEllipsizedTextPrivate *priv = EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE (renderer);
  gint                                  focus_line_width;
  gint                                  focus_padding;
  gint                                  text_height;
  gint                                  text_width;

  /* determine the dimensions of the text from the GtkCellRendererText */
  (*GTK_CELL_RENDERER_CLASS (exo_cell_renderer_ellipsized_text_parent_class)->get_size) (renderer, widget, NULL, NULL, NULL, &text_width, &text_height);

  /* if we have to follow the state manually, we'll need
   * to reserve some space to render the indicator to.
   */
  if (G_LIKELY (priv->follow_state))
    {
      /* determine the focus-padding and focus-line-width style properties from the widget */
      gtk_widget_style_get (widget, "focus-padding", &focus_padding, "focus-line-width", &focus_line_width, NULL);

      /* add the focus widget to the text dimensions */
      text_width += 2 * (focus_line_width + focus_padding);
      text_height += 2 * (focus_line_width + focus_padding);
    }

  /* update width/height */
  if (G_LIKELY (width != NULL))
    *width = text_width;
  if (G_LIKELY (height != NULL))
    *height = text_height;

  /* update the x/y offsets */
  if (G_LIKELY (cell_area != NULL))
    {
      if (G_LIKELY (x_offset != NULL))
        {
          *x_offset = ((gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL) ? (1.0 - renderer->xalign) : renderer->xalign)
                    * (cell_area->width - text_width);
          *x_offset = MAX (*x_offset, 0);
        }

      if (G_LIKELY (y_offset != NULL))
        {
          *y_offset = renderer->yalign * (cell_area->height - text_height);
          *y_offset = MAX (*y_offset, 0);
        }
    }
}



static void
exo_cell_renderer_ellipsized_text_render (GtkCellRenderer     *renderer,
                                          GdkWindow           *window,
                                          GtkWidget           *widget,
                                          GdkRectangle        *background_area,
                                          GdkRectangle        *cell_area,
                                          GdkRectangle        *expose_area,
                                          GtkCellRendererState flags)
{
  ExoCellRendererEllipsizedTextPrivate *priv = EXO_CELL_RENDERER_ELLIPSIZED_TEXT_GET_PRIVATE (renderer);
  GdkRectangle                          text_area;
  GtkStateType                          state;
#if !GTK_CHECK_VERSION(2,8,0)
  GdkPoint                              points[8];
#else
  cairo_t                              *cr;
#endif
  gint                                  focus_line_width;
  gint                                  focus_padding;
  gint                                  text_height;
  gint                                  text_width;
  gint                                  x0, x1;
  gint                                  y0, y1;

  /* determine the text cell areas */
  if (G_UNLIKELY (!priv->follow_state))
    {
      /* no state indicator */
      text_area = *cell_area;
    }
  else
    {
      /* determine the widget state */
      if ((flags & GTK_CELL_RENDERER_SELECTED) == GTK_CELL_RENDERER_SELECTED)
        {
          if (GTK_WIDGET_HAS_FOCUS (widget))
            state = GTK_STATE_SELECTED;
          else
            state = GTK_STATE_ACTIVE;
        }
      else if ((flags & GTK_CELL_RENDERER_PRELIT) == GTK_CELL_RENDERER_PRELIT
            && GTK_WIDGET_STATE (widget) == GTK_STATE_PRELIGHT)
        {
          state = GTK_STATE_PRELIGHT;
        }
      else
        {
          if (GTK_WIDGET_STATE (widget) == GTK_STATE_INSENSITIVE)
            state = GTK_STATE_INSENSITIVE;
          else
            state = GTK_STATE_NORMAL;
        }

      /* determine the focus-padding and focus-line-width style properties from the widget */
      gtk_widget_style_get (widget, "focus-padding", &focus_padding, "focus-line-width", &focus_line_width, NULL);

      /* determine the text cell area */
      text_area.x = cell_area->x + (focus_line_width + focus_padding);
      text_area.y = cell_area->y + (focus_line_width + focus_padding);
      text_area.width = cell_area->width - 2 * (focus_line_width + focus_padding);
      text_area.height = cell_area->height - 2 * (focus_line_width + focus_padding);

      /* check if we need to draw any state indicator */
      if ((flags & (GTK_CELL_RENDERER_FOCUSED | GTK_CELL_RENDERER_SELECTED)) != 0)
        {
          /* determine the real text dimensions */
          (*GTK_CELL_RENDERER_CLASS (exo_cell_renderer_ellipsized_text_parent_class)->get_size) (renderer, widget, &text_area,
                                                                                                 &x0, &y0, &text_width, &text_height);

          /* adjust the offsets appropriately */
          x0 += text_area.x;
          y0 += text_area.y;

          /* render the selected state indicator */
          if ((flags & GTK_CELL_RENDERER_SELECTED) == GTK_CELL_RENDERER_SELECTED)
            {
              /* calculate the text bounding box (including the focus padding/width) */
              x1 = x0 + text_width;
              y1 = y0 + text_height;

#if GTK_CHECK_VERSION(2,8,0)
              /* Cairo produces nicer results than using a polygon
               * and so we use it directly if possible.
               */
              cr = gdk_cairo_create (window);
              cairo_move_to (cr, x0 + 5, y0);
              cairo_line_to (cr, x1 - 5, y0);
              cairo_curve_to (cr, x1 - 5, y0, x1, y0, x1, y0 + 5);
              cairo_line_to (cr, x1, y1 - 5);
              cairo_curve_to (cr, x1, y1 - 5, x1, y1, x1 - 5, y1);
              cairo_line_to (cr, x0 + 5, y1);
              cairo_curve_to (cr, x0 + 5, y1, x0, y1, x0, y1 - 5);
              cairo_line_to (cr, x0, y0 + 5);
              cairo_curve_to (cr, x0, y0 + 5, x0, y0, x0 + 5, y0);
              gdk_cairo_set_source_color (cr, &widget->style->base[state]);
              cairo_fill (cr);
              cairo_destroy (cr);
#else
              /* calculate a (more or less rounded) polygon */
              points[0].x = x0 + 2; points[0].y = y0;
              points[1].x = x1 - 2; points[1].y = y0;
              points[2].x = x1;     points[2].y = y0 + 2;
              points[3].x = x1;     points[3].y = y1 - 2;
              points[4].x = x1 - 2; points[4].y = y1;
              points[5].x = x0 + 2; points[5].y = y1;
              points[6].x = x0;     points[6].y = y1 - 2;
              points[7].x = x0;     points[7].y = y0 + 2;

              /* render the indicator */
              gdk_draw_polygon (window, widget->style->base_gc[state], TRUE, points, G_N_ELEMENTS (points));
#endif
            }

          /* draw the focus indicator */
          if ((flags & GTK_CELL_RENDERER_FOCUSED) != 0)
            {
              gtk_paint_focus (widget->style, window, GTK_WIDGET_STATE (widget), NULL, widget, "icon_view", x0, y0, text_width, text_height);
              flags &= ~GTK_CELL_RENDERER_FOCUSED;
            }
        }
    }

  /* render the text using the GtkCellRendererText */
  (*GTK_CELL_RENDERER_CLASS (exo_cell_renderer_ellipsized_text_parent_class)->render) (renderer, window, widget, background_area,
                                                                                       &text_area, expose_area, flags);
}



/**
 * exo_cell_renderer_ellipsized_text_new:
 *
 * Creates a new #ExoCellRendererEllipsizedText. Adjust rendering parameters using gobject properties,
 * which can be set globally via g_object_set(). Also, with #GtkCellLayout and #GtkTreeViewColumn, you
 * can bind a property to a value in a #GtkTreeModel.
 *
 * Return value: the newly allocated #ExoCellRendererEllipsizedText.
 **/
GtkCellRenderer*
exo_cell_renderer_ellipsized_text_new (void)
{
  return g_object_new (EXO_TYPE_CELL_RENDERER_ELLIPSIZED_TEXT, NULL);
}



#define __EXO_CELL_RENDERER_ELLIPSIZED_TEXT_C__
#include <exo/exo-aliasdef.c>
