#include <gtk/gtk.h>
#define MIDDAY 12

// To access the surface withing different callbacks
static cairo_surface_t *surface = NULL;

static void
clear_surface (double *value)
{
  double range;
  cairo_t *cr;

  cr = cairo_create (surface);


  if (!value) {
    cairo_set_source_rgb (cr, 0, 0, 0);
  } else if (*value >= MIDDAY) {
    range = ((24 - *value) / (MIDDAY));
    cairo_set_source_rgb (cr, range, range, range);
  } else {
    cairo_set_source_rgb (cr, (*value) / (MIDDAY * 2), (*value) / (MIDDAY * 2), (*value) / (MIDDAY * 2));
  }
  cairo_paint (cr);

  cairo_destroy (cr);
}

static gboolean
configure_event_cb (GtkWidget         *widget,
                    GdkEventConfigure *event,
                    gpointer           data)
{
  if (surface) {
    cairo_surface_destroy (surface);
  }

  surface = gdk_window_create_similar_surface (gtk_widget_get_window (widget),
                                               CAIRO_CONTENT_COLOR,
                                               gtk_widget_get_allocated_width (widget),
                                               gtk_widget_get_allocated_height (widget));
  clear_surface (NULL);
  return TRUE;
}


static gboolean
draw_cb (GtkWidget *widget,
         cairo_t   *cr,
         gpointer   data)
{
  cairo_set_source_surface (cr, surface, 0, 0);
  cairo_paint (cr);

  return FALSE;
}

gboolean slider_value_changed_cb(GtkRange *range, GtkWidget *widget, gdouble value,
                                 gpointer darea)
{
  cairo_t *cr;
  cr = cairo_create (surface);
  
  double width, height;
  
  // clear the previos surface
  clear_surface (&value);
  
  width = gtk_widget_get_allocated_width (GTK_WIDGET(darea));
  height = gtk_widget_get_allocated_height (GTK_WIDGET(darea));

  /* a circle with the slider value as the angel1. 4 in radius is taken
     to have a small circle compared to the whole surface */

  if (value < MIDDAY) {
    cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
    cairo_arc (cr, width / 2, height / 2, MIN(width, height) / 8, 0, value * G_PI / MIDDAY);
  } else {
    cairo_set_source_rgb (cr, 1.0, 1.0, 0.0);
    cairo_arc (cr, width / 2, height / 2, MIN(width, height) / 4, 0, value * G_PI / MIDDAY);
  }

  // need to draw and destroy the cairo_t and then build it again in the next run
  cairo_fill (cr);
  cairo_destroy (cr);
  
  // refresh the surface
  gtk_widget_queue_draw(GTK_WIDGET(darea));
  
  // has to be false so the slider can change
  return FALSE;

}

gboolean button_clicked_cb(GtkWidget *widget,
                           gpointer darea)
{  
  double width, height;
  
  // clear the previos surface
  clear_surface (NULL);
  
  width = gtk_widget_get_allocated_width (GTK_WIDGET(darea));
  height = gtk_widget_get_allocated_height (GTK_WIDGET(darea));
  /* a circle with the slider value as the angel1. 4 in radius is taken
     to have a small circle compared to the whole surface */
  cairo_t *cr;
  for (double i = 0; i < 24; i += 0.002) {
    cr = cairo_create (surface);

    clear_surface (&i);

    if (i < MIDDAY) {
      cairo_set_source_rgb (cr, 0.9531, 0.9609, 0.9375);
      cairo_arc (cr, width / 2, height / 2, MIN(width, height) / 8, 0, i * G_PI / MIDDAY);
    } else {
      cairo_set_source_rgb (cr, 1.0, 1.0, 0.0);
      cairo_arc (cr, width / 2, height / 2, MIN(width, height) / 4, 0, i * G_PI / MIDDAY);
    }
    // need to draw and destroy the cairo_t and then build it again in the next run
    cairo_fill (cr);
    cairo_destroy (cr);

    // refresh the surface
    gtk_widget_queue_draw(GTK_WIDGET(darea));

    /* It's required to return back to the main loop for just an instruction
       and to show the updated surface*/
    while(gtk_events_pending()) gtk_main_iteration();
  }

  return FALSE;
}

int main(int argc, char **argv) {
    GtkWidget *win, *slider, *box, *btn, *drawing_area; 
    gtk_init (&argc, &argv);

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    slider = gtk_scale_new_with_range (GTK_ORIENTATION_HORIZONTAL, 0, 24, 0.5);
    btn = gtk_button_new_with_label("Start!");
    drawing_area = gtk_drawing_area_new();
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);

    gtk_widget_set_size_request (drawing_area, 400, 400);
    gtk_window_set_default_size (GTK_WINDOW(win), 400, 400);
    gtk_window_set_title (GTK_WINDOW(win), "Changing color acording to a slider value!");

    // callbacks
    g_signal_connect (win, "delete_event", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT(btn), "clicked", G_CALLBACK (button_clicked_cb), drawing_area);
    g_signal_connect (G_OBJECT (drawing_area), "draw", G_CALLBACK (draw_cb), NULL);
    g_signal_connect (drawing_area,"configure-event", G_CALLBACK (configure_event_cb), NULL);
    g_signal_connect (G_OBJECT(slider), "change-value", G_CALLBACK (slider_value_changed_cb), drawing_area);

    // put widgets into the box and box into the window
    gtk_box_pack_start (GTK_BOX (box), btn, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), slider, FALSE, FALSE, 20);
    gtk_box_pack_start (GTK_BOX (box), drawing_area, TRUE, TRUE, 20);
    gtk_container_add (GTK_CONTAINER (win), box);

    gtk_widget_show_all (win);
    gtk_main();
}