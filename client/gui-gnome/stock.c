/* Eye of Gnome image viewer - stock icons
 *
 * Copyright (C) 1999 The Free Software Foundation
 *
 * Author: Federico Mena-Quintero <federico@gimp.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; only version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include "gnome-compat.h"
#include "stock.h"


#include "stock/stock-getcard.xpm"
#include "stock/stock-sendarmies.xpm"
#include "stock/stock-endturn.xpm"
#include "stock/stock-viewplayers.xpm"
#include "stock/stock-zoom-1.xpm"
#include "stock/stock-zoom-fit.xpm"
#include "stock/stock-zoom-in.xpm"
#include "stock/stock-zoom-out.xpm"

/* Hash table to store stock pixbufs */
static GHashTable *stock_pixbufs = NULL;

static void add_stock_icon(const char **xpm_data, const gchar *stock_id)
{
	GdkPixbuf *pixbuf;
	
	pixbuf = gdk_pixbuf_new_from_xpm_data(xpm_data);
	if (pixbuf) {
		g_hash_table_insert(stock_pixbufs, g_strdup(stock_id), pixbuf);
	}
}

void stock_init (void)
{
	static gboolean initialized = FALSE;

	if (initialized)
		return;
	else
		initialized = TRUE;

	/* Create hash table to store pixbufs */
	stock_pixbufs = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);

	/* Add all stock icons */
	add_stock_icon(stock_getcard_xpm, STOCK_GETCARD);
	add_stock_icon(stock_sendarmies_xpm, STOCK_SENDARMIES);
	add_stock_icon(stock_endturn_xpm, STOCK_ENDTURN);
	add_stock_icon(stock_viewplayers_xpm, STOCK_VIEWPLAYERS);
	add_stock_icon(stock_zoom_1_xpm, STOCK_ZOOM_1);
	add_stock_icon(stock_zoom_fit_xpm, STOCK_ZOOM_FIT);
	add_stock_icon(stock_zoom_in_xpm, STOCK_ZOOM_IN);
	add_stock_icon(stock_zoom_out_xpm, STOCK_ZOOM_OUT);
}

GdkPixbuf* stock_get_pixbuf(const gchar *stock_id)
{
	if (!stock_pixbufs) {
		stock_init();
	}
	return (GdkPixbuf*)g_hash_table_lookup(stock_pixbufs, stock_id);
}

GtkWidget* stock_create_image(const gchar *stock_id)
{
	GdkPixbuf *pixbuf = stock_get_pixbuf(stock_id);
	if (pixbuf) {
		return gtk_image_new_from_pixbuf(pixbuf);
	}
	return NULL;
}
