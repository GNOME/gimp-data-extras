/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 * Max-RGB plug-in (C) 1997 Shuji Narazaki, 2000 Tim Copperfield
 * e-mail: narazaki@InetQ.or.jp, timecop@japan.co.jp
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/* For localization functions, though for now, there is no gettext catalog
 * attached to this plug-in.
 * When we'll want real localization, use set_i18n().
 */
#define GETTEXT_PACKAGE "max-rgb"
#include <glib/gi18n-lib.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>


#define PLUG_IN_PROC   "plug-in-max-rgb"
#define PLUG_IN_BINARY "max-rgb"
#define PLUG_IN_ROLE   "gimp-max-rgb"

typedef struct _MaxRGB      MaxRGB;
typedef struct _MaxRGBClass MaxRGBClass;

typedef struct
{
  gint     init_value;
  gint     flag;
  gboolean has_alpha;
} MaxRgbParam_t;

typedef struct
{
  gint max_p;
} ValueType;

enum
{
  MIN_CHANNELS = 0,
  MAX_CHANNELS = 1
};

static ValueType pvals =
{
  MAX_CHANNELS
};

struct _MaxRGB
{
  GimpPlugIn parent_instance;
};

struct _MaxRGBClass
{
  GimpPlugInClass parent_class;
};


#define MAXRGB_TYPE  (max_rgb_get_type ())
#define MAXRGB (obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), MAXRGB_TYPE, MaxRGB))

GType                    max_rgb_get_type         (void) G_GNUC_CONST;

static GList           * max_rgb_query_procedures (GimpPlugIn           *plug_in);
static GimpProcedure   * max_rgb_create_procedure (GimpPlugIn           *plug_in,
                                                   const gchar          *name);

static GimpValueArray * max_rgb_run               (GimpProcedure        *procedure,
                                                   GimpRunMode           run_mode,
                                                   GimpImage            *image,
                                                   gint                  n_drawables,
                                                   GimpDrawable        **drawables,
                                                   const GimpValueArray *args,
                                                   gpointer              run_data);

static void              max_rgb_func             (const guchar         *src,
                                                   guchar               *dest,
                                                   gint                  bpp,
                                                   gpointer              data);

static GimpPDBStatusType main_function            (GimpDrawable         *drawable,
                                                   GimpPreview          *preview);

static gint              max_rgb_dialog           (GimpDrawable         *drawable);

G_DEFINE_TYPE (MaxRGB, max_rgb, GIMP_TYPE_PLUG_IN)

GIMP_MAIN (MAXRGB_TYPE)

static void
max_rgb_class_init (MaxRGBClass *klass)
{
  GimpPlugInClass *plug_in_class = GIMP_PLUG_IN_CLASS (klass);

  plug_in_class->query_procedures = max_rgb_query_procedures;
  plug_in_class->create_procedure = max_rgb_create_procedure;
  /* Localization disabled for now. */
  plug_in_class->set_i18n         = NULL;
}

static void
max_rgb_init (MaxRGB *maxrgb)
{
}

static GList *
max_rgb_query_procedures (GimpPlugIn *plug_in)
{
  return g_list_append (NULL, g_strdup (PLUG_IN_PROC));
}

static GimpProcedure *
max_rgb_create_procedure (GimpPlugIn  *plug_in,
                          const gchar *name)
{
  GimpProcedure *procedure = NULL;

  if (! strcmp (name, PLUG_IN_PROC))
    {
      procedure = gimp_image_procedure_new (plug_in, name,
                                            GIMP_PDB_PROC_TYPE_PLUGIN,
                                            max_rgb_run, NULL, NULL);

      gimp_procedure_set_image_types (procedure, "RGB*");
      gimp_procedure_set_sensitivity_mask (procedure,
                                           GIMP_PROCEDURE_SENSITIVE_DRAWABLE  |
                                           GIMP_PROCEDURE_SENSITIVE_DRAWABLES |
                                           GIMP_PROCEDURE_SENSITIVE_NO_DRAWABLES);

      gimp_procedure_set_menu_label (procedure, _("Maxim_um RGB..."));
      gimp_procedure_add_menu_path (procedure, "<Image>/Colors/Map");

      gimp_procedure_set_documentation (procedure,
                                        _("Reduce image to pure red, green, and blue"),
                                        "Reduce image to pure red, green, and blue",
                                        name);
      gimp_procedure_set_attribution (procedure,
                                      "Shuji Narazaki, ",
                                      "Shuji Narazaki (narazaki@InetQ.or.jp)",
                                      "1997");

      GIMP_PROC_ARG_INT (procedure, "max-p",
                         "Min/Max RGB Values",
                         "Minimize (0), Maximize (1)",
                         0, 1, 1,
                         G_PARAM_READWRITE);

      GIMP_PROC_VAL_IMAGE (procedure, "new-image",
                           "New image",
                           "Output image",
                           FALSE,
                           G_PARAM_READWRITE);
    }

  return procedure;
}

static GimpValueArray *
max_rgb_run (GimpProcedure        *procedure,
             GimpRunMode           run_mode,
             GimpImage            *image,
             gint                  n_drawables,
             GimpDrawable        **drawables,
             const GimpValueArray *args,
             gpointer              run_data)
{
  GimpValueArray     *return_vals = NULL;
  GimpPDBStatusType   status      = GIMP_PDB_SUCCESS;

  gegl_init (NULL, NULL);

  switch (run_mode)
    {
    case GIMP_RUN_INTERACTIVE:
      gimp_get_data (PLUG_IN_PROC, &pvals);

      if (! max_rgb_dialog (drawables[0]))
        {
          return gimp_procedure_new_return_values (procedure, GIMP_PDB_CANCEL,
                                                   NULL);
        }
      break;

    case GIMP_RUN_NONINTERACTIVE:
      pvals.max_p = GIMP_VALUES_GET_INT (args, 0);

      break;

    case GIMP_RUN_WITH_LAST_VALS:
      gimp_get_data (PLUG_IN_PROC, &pvals);
      break;

    default:
      break;
    }

  if (status == GIMP_PDB_SUCCESS)
    {
      gimp_progress_init (_("Max RGB"));

      status = main_function (drawables[0], NULL);

      return_vals = gimp_procedure_new_return_values (procedure, status,
                                                      NULL);

      /*  Store data  */
      if (run_mode == GIMP_RUN_INTERACTIVE)
        gimp_set_data (PLUG_IN_PROC, &pvals, sizeof (ValueType));
    }

  if (! return_vals)
    return_vals = gimp_procedure_new_return_values (procedure, status, NULL);

  return return_vals;
}

static void
max_rgb_func (const guchar *src,
              guchar       *dest,
              gint          bpp,
              gpointer      data)
{
  MaxRgbParam_t *param = (MaxRgbParam_t*) data;
  gint   ch, max_ch = 0;
  guchar max, tmp_value;

  max = param->init_value;
  for (ch = 0; ch < 3; ch++)
    if (param->flag * max <= param->flag * (tmp_value = (*src++)))
      {
        if (max == tmp_value)
          {
            max_ch += 1 << ch;
          }
        else
          {
            max_ch = 1 << ch; /* clear memories of old channels */
            max = tmp_value;
          }
      }

  dest[0] = (max_ch & (1 << 0)) ? max : 0;
  dest[1] = (max_ch & (1 << 1)) ? max : 0;
  dest[2] = (max_ch & (1 << 2)) ? max : 0;
  if (param->has_alpha)
    dest[3] = src[3];
}

static GimpPDBStatusType
main_function (GimpDrawable *drawable,
               GimpPreview  *preview)
{
  MaxRgbParam_t param;

  param.init_value = (pvals.max_p > 0) ? 0 : 255;
  param.flag = (0 < pvals.max_p) ? 1 : -1;
  param.has_alpha = gimp_drawable_has_alpha (drawable);

  if (preview)
    {
      gint    i;
      guchar *buffer;
      guchar *src;
      gint    width, height, bpp;

      src = gimp_zoom_preview_get_source (GIMP_ZOOM_PREVIEW (preview),
                                          &width, &height, &bpp);

      buffer = g_new (guchar, width * height * bpp);

      for (i = 0; i < width * height; i++)
        {
          max_rgb_func (src    + i * bpp,
                        buffer + i * bpp,
                        bpp,
                        &param);
        }

      gimp_preview_draw_buffer (preview, buffer, width * bpp);
      g_free (buffer);
      g_free (src);
    }
  else
    {
      GeglBuffer   *src_buffer;
      GeglBuffer   *dest_buffer;
      const Babl   *format;
      guchar       *src;
      guchar       *dest;
      gint          bpp;
      gint          x1, y1, x2, y2;
      gint          total_area;

      gimp_progress_init (_("Max RGB"));

      gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
      format = gimp_drawable_get_format (drawable);
      bpp = babl_format_get_bytes_per_pixel (format);

      src_buffer = gimp_drawable_get_buffer (drawable);
      dest_buffer = gimp_drawable_get_shadow_buffer (drawable);

      total_area  = (x2 - x1) * (y2 - y1);

      src = g_new (guchar, total_area * bpp);
      dest = g_new (guchar, total_area * bpp);

      if (total_area <= 0)
        goto out;

      gegl_buffer_get (src_buffer,
                       GEGL_RECTANGLE (x1, y1, (x2 - x1), (y2 - y1)),
                       1.0, format, src,
                       GEGL_AUTO_ROWSTRIDE, GEGL_ABYSS_NONE);

      for (gint i = 0; i < total_area; i++)
        {
          max_rgb_func (src    + i * bpp,
                        dest + i * bpp,
                        bpp,
                        &param);
        }

      gegl_buffer_set (dest_buffer,
                       GEGL_RECTANGLE (x1, y1, (x2 - x1), (y2 - y1)), 0,
                       format, dest,
                       GEGL_AUTO_ROWSTRIDE);

      out:
      g_free (src);
      g_free (dest);
      g_object_unref (src_buffer);
      g_object_unref (dest_buffer);

      gimp_drawable_merge_shadow (drawable, TRUE);
      gimp_drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
      gimp_displays_flush ();
    }

  return GIMP_PDB_SUCCESS;
}

static gint
max_rgb_dialog (GimpDrawable *drawable)
{
  GtkWidget *dialog;
  GtkWidget *main_vbox;
  GtkWidget *preview;
  GtkWidget *frame;
  GtkWidget *max;
  GtkWidget *min;
  gboolean   run;

  gimp_ui_init (PLUG_IN_BINARY);

  dialog = gimp_dialog_new (_("Maximum RGB Value"), PLUG_IN_ROLE,
                            NULL, 0,
                            gimp_standard_help_func, PLUG_IN_PROC,

                            _("_Cancel"), GTK_RESPONSE_CANCEL,
                            _("_OK"),     GTK_RESPONSE_OK,

                            NULL);

  gimp_dialog_set_alternative_button_order (GTK_DIALOG (dialog),
                                            GTK_RESPONSE_OK,
                                            GTK_RESPONSE_CANCEL,
                                            -1);

  gimp_window_set_transient (GTK_WINDOW (dialog));

  main_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 12);
  gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))),
                      main_vbox, TRUE, TRUE, 0);
  gtk_widget_show (main_vbox);

  preview = gimp_zoom_preview_new_from_drawable (drawable);
  gtk_box_pack_start (GTK_BOX (main_vbox), preview, TRUE, TRUE, 0);
  gtk_widget_show (preview);

  g_signal_connect_swapped (preview, "invalidated",
                            G_CALLBACK (main_function),
                            drawable);

  frame = gimp_int_radio_group_new (FALSE, NULL,
                                    G_CALLBACK (gimp_radio_button_update),
                                    &pvals.max_p, NULL, pvals.max_p,

                                    _("_Hold the maximal channels"),
                                    MAX_CHANNELS, &max,

                                    _("Ho_ld the minimal channels"),
                                    MIN_CHANNELS, &min,

                                    NULL);

  g_signal_connect_swapped (max, "toggled",
                            G_CALLBACK (gimp_preview_invalidate),
                            preview);
  g_signal_connect_swapped (min, "toggled",
                            G_CALLBACK (gimp_preview_invalidate),
                            preview);

  gtk_box_pack_start (GTK_BOX (main_vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run;
}
