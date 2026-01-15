/*	$Id: status.c,v 1.50 2004/02/08 20:49:05 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2000 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <rquesada@core-sdi.com>
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
/*
 * functions that manages the status dialog
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include "gnome-compat.h"

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "status.h"
#include "colors.h"
#include "priv.h"
#include "fonts.h"

extern TTheme gui_theme;

/* some defines for the position */
#define RECT_SIZE (8)
#define PLAYERS_COLORS_OFFSET (246)
#define ROUND_OFFSET (410)
#define OFFSET_LEFT (6)
#define OFFSET_RIGHT (-8)

GtkWidget *status_dialog=NULL;
static GtkWidget *status_treeview=NULL;

static GtkWidget *ministatus = NULL;

static GtkWidget* mainstatus_canvas = NULL;

// GTK4: Cambiamos GnomeCanvasItem por GtkWidget
static GtkWidget *players_color[TEG_MAX_PLAYERS];
static GtkWidget *color_started_item;
static GtkWidget *round_number_item;
static GtkWidget *players_color_over;

static gint status_boton_clicked_cb(GtkWidget *area, GdkEvent *event, gpointer user_data)
{
	out_status();
	return FALSE;
}

static TEG_STATUS status_paint_color( int color, GdkPixmap **pixmap )
{
	// GTK4: Esta función ya no es necesaria ya que usamos GtkImage
	*pixmap = NULL;
	return TEG_STATUS_SUCCESS;
}

// GTK4: Función callback para manejar respuestas del diálogo
static void dialog_response(GtkDialog *dialog, int response, gpointer user_data)
{
	if (response == GTK_RESPONSE_ACCEPT) {
		// Refresh button clicked
		status_update_dialog();
	} else {
		// Close button clicked or dialog closed
		gtk_widget_set_visible(GTK_WIDGET(dialog), FALSE);
	}
}

static GtkListStore *
status_create_model (void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new (
			STATUS_COLUMN_LAST,
			G_TYPE_STRING,	/* color */
			G_TYPE_UINT,	/* number */
			G_TYPE_STRING,	/* name */
			G_TYPE_UINT,	/* score */
			G_TYPE_STRING,	/* address */
			G_TYPE_BOOLEAN,	/* human? */
			G_TYPE_INT,	/* countries */
			G_TYPE_INT,	/* armies */
			G_TYPE_UINT,	/* cards */
			G_TYPE_STRING,	/* status */
			G_TYPE_BOOLEAN	/* started the turn */
			);

	return store;
}

static void status_add_columns (GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* color column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Color"),
							 renderer,
							 "text", STATUS_COLUMN_COLOR,
							 NULL);
	gtk_tree_view_append_column (treeview, column);

	/* number column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Number"),
							 renderer,
							 "text", STATUS_COLUMN_NUMBER,
							 NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_NUMBER);
	gtk_tree_view_append_column (treeview, column);

	/* name column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Name"),
							 renderer,
							 "text", STATUS_COLUMN_NAME,
							 NULL);
	gtk_tree_view_append_column (treeview, column);

	/* score column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Score"),
							 renderer,
							 "text", STATUS_COLUMN_SCORE,
							 NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_SCORE);
	gtk_tree_view_append_column (treeview, column);

	/* address column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Address"),
							 renderer,
							 "text", STATUS_COLUMN_ADDR,
							 NULL);
	gtk_tree_view_append_column (treeview, column);

	/* human column */
	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Human?"),
							 renderer,
							 "active", STATUS_COLUMN_HUMAN,
							 NULL);
	gtk_tree_view_append_column (treeview, column);

	/* countries column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Countries"),
							 renderer,
							 "text", STATUS_COLUMN_COUNTRIES,
							 NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_COUNTRIES);
	gtk_tree_view_append_column (treeview, column);

	/* armies column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Armies"),
							 renderer,
							 "text", STATUS_COLUMN_ARMIES,
							 NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_ARMIES);
	gtk_tree_view_append_column (treeview, column);

	/* cards column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Cards"),
							 renderer,
							 "text", STATUS_COLUMN_CARDS,
							 NULL);
	gtk_tree_view_column_set_sort_column_id (column, STATUS_COLUMN_CARDS);
	gtk_tree_view_append_column (treeview, column);

	/* status column */
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Status"),
							 renderer,
							 "text", STATUS_COLUMN_STATUS,
							 NULL);
	gtk_tree_view_append_column (treeview, column);

	/* started turn column */
	renderer = gtk_cell_renderer_toggle_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Started Turn?"),
							 renderer,
							 "active", STATUS_COLUMN_WHO,
							 NULL);
	gtk_tree_view_append_column (treeview, column);
}

TEG_STATUS status_turn_color(PCPLAYER pJ, GdkPixmap **pixmap)
{
	// GTK4: Esta función ya no es necesaria ya que usamos GtkImage
	*pixmap = NULL;
	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS status_update_model( GtkListStore *store)
{
	GtkTreeIter iter;
	PCPLAYER pJ;
	PLIST_ENTRY l = g_list_player.Flink;

	gtk_list_store_clear( store );

	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) )
	{
		gchar *name;
		pJ = (PCPLAYER) l;
		
		name = translate_to_utf8( pJ->name );

		gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
				STATUS_COLUMN_COLOR, _(g_colores[pJ->color]),
				STATUS_COLUMN_NUMBER, pJ->numjug,
				STATUS_COLUMN_NAME, name,
				STATUS_COLUMN_SCORE, pJ->score,
				STATUS_COLUMN_ADDR, pJ->addr,
				STATUS_COLUMN_HUMAN, pJ->human,
				STATUS_COLUMN_COUNTRIES, pJ->tot_countries,
				STATUS_COLUMN_ARMIES, pJ->tot_armies,
				STATUS_COLUMN_CARDS, pJ->tot_cards,
				STATUS_COLUMN_STATUS, _(g_estados[pJ->estado]),
				STATUS_COLUMN_WHO, pJ->empezo_turno,
				-1);
		free( name );

		l = LIST_NEXT(l);
	}
	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_visibility_of_columns( void )
{
	GtkTreeViewColumn *column;
	int i;

	if( status_treeview == NULL )
		return TEG_STATUS_ERROR;

	for( i=0; i < STATUS_COLUMN_LAST; i++ ) {
		column = gtk_tree_view_get_column( GTK_TREE_VIEW(status_treeview), i );
		gtk_tree_view_column_set_visible( column, (gui_private.status_show & (1<<i)) != 0 );
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS status_update_dialog()
{
	GtkListStore *store = NULL;

	if( status_dialog == NULL )
		return TEG_STATUS_ERROR;

	if( status_treeview == NULL ) {

		/* create tree model */
		store = status_create_model ();

		/* create tree view */
		status_treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL(store));
		gtk_tree_view_set_search_column (GTK_TREE_VIEW (status_treeview),
				STATUS_COLUMN_SCORE);

		g_object_unref (G_OBJECT (store));
		gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(status_dialog))), GTK_WIDGET(status_treeview), TRUE, TRUE, 0);

		/* add columns to the tree view */
		status_add_columns (GTK_TREE_VIEW (status_treeview));
	}

	/* Obtener el store del treeview existente */
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(status_treeview)));
	if (store) {
		status_update_model( store );
	}

	gtk_widget_set_visible(status_treeview, TRUE);
	return TEG_STATUS_SUCCESS;
}

/* view the status of players */
void status_view()
{
	if( status_dialog == NULL) {
		// GTK4: Usamos GtkDialog en lugar de GnomeDialog
		status_dialog = gtk_dialog_new();
		gtk_window_set_title(GTK_WINDOW(status_dialog), _("Players Status"));
		gtk_window_set_transient_for(GTK_WINDOW(status_dialog), GTK_WINDOW(main_window));
		gtk_window_set_modal(GTK_WINDOW(status_dialog), TRUE);
		gtk_window_set_resizable(GTK_WINDOW(status_dialog), TRUE);
		gtk_window_set_default_size(GTK_WINDOW(status_dialog), 600, 400);

		// GTK4: Agregamos botones al diálogo
		gtk_dialog_add_button(GTK_DIALOG(status_dialog), _("Refresh"), GTK_RESPONSE_ACCEPT);
		gtk_dialog_add_button(GTK_DIALOG(status_dialog), _("Close"), GTK_RESPONSE_CLOSE);

		g_signal_connect(status_dialog, "response", G_CALLBACK(dialog_response), status_dialog);
	}

	status_update_dialog();
	gtk_window_present(GTK_WINDOW(status_dialog));
}

TEG_STATUS ministatus_update()
{
	if( ministatus == NULL )
		return TEG_STATUS_ERROR;

	gtk_widget_queue_draw(ministatus);

	return TEG_STATUS_SUCCESS;
}

static gint ministatus_expose_cb(GtkWidget *area, GdkEvent *event, gpointer user_data)
{
	// GTK4: Esta función ya no es necesaria ya que usamos GtkDrawingArea con Cairo
	return FALSE;
}

GtkWidget *ministatus_build()
{
	if( ministatus == NULL ) {
		// GTK4: Usamos GtkDrawingArea en lugar de GtkDrawingArea con eventos
		ministatus = gtk_drawing_area_new();

		g_signal_connect(G_OBJECT(ministatus), "draw",
			   G_CALLBACK(ministatus_expose_cb), NULL);
	}
	gtk_widget_set_size_request(ministatus, 15, -1);
	gtk_widget_set_visible(ministatus, TRUE);

	return ministatus;
}

/*
 * Main Status
 */

#define MAINSTATUS_X (642)
#define MAINSTATUS_Y (30)
TEG_STATUS mainstatus_create( GtkWidget **window )
{
	int i;
	int failed=1;

	if( mainstatus_canvas )
		goto error;

	// GTK4: Usamos GtkBox en lugar de GNOME Canvas
	mainstatus_canvas = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	if( ! mainstatus_canvas )
		goto error;

	/* load colors for started_item, and player_colors */
	colors_load_images();

	gtk_widget_set_size_request(mainstatus_canvas, MAINSTATUS_X, MAINSTATUS_Y + 1);

	/* background */
	if( gui_theme.toolbar_custom && gui_theme.toolbar_name )
	{
		GdkPixbuf *im;
		char *filename = theme_load_file( gui_theme.toolbar_name );
		im = gdk_pixbuf_new_from_file(filename, NULL);

		if( im ) {
			GtkWidget *image = gtk_image_new_from_pixbuf(im);
			gtk_box_pack_start(GTK_BOX(mainstatus_canvas), image, FALSE, FALSE, 0);
			gtk_widget_set_visible(image, TRUE);

			failed = 0;

			g_object_unref( im );
		}
	}

	/* load default background */
	if( failed ) {
		GtkWidget *label = gtk_label_new(_("Status Bar"));
		gtk_box_pack_start(GTK_BOX(mainstatus_canvas), label, FALSE, FALSE, 0);
		gtk_widget_set_visible(label, TRUE);
	}

	/* round started by */
	GtkWidget *round_label = gtk_label_new(_("Round started by:"));
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), round_label, FALSE, FALSE, 0);
	gtk_widget_set_visible(round_label, TRUE);

	color_started_item = gtk_image_new_from_pixbuf(g_color_circles[TEG_MAX_PLAYERS]);
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), color_started_item, FALSE, FALSE, 0);
	gtk_widget_set_visible(color_started_item, FALSE);

	/* round number */
	GtkWidget *round_num_label = gtk_label_new(_("Round number:"));
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), round_num_label, FALSE, FALSE, 0);
	gtk_widget_set_visible(round_num_label, TRUE);

	round_number_item = gtk_label_new(_("?"));
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), round_number_item, FALSE, FALSE, 0);
	gtk_widget_set_visible(round_number_item, TRUE);

	/* players turn */
	GtkWidget *players_label = gtk_label_new(_("Players turn:"));
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), players_label, FALSE, FALSE, 0);
	gtk_widget_set_visible(players_label, TRUE);

	/* create canvas for the circles & and load the circles */
	for(i=0;i<TEG_MAX_PLAYERS;i++)
	{
		players_color[i] = gtk_image_new_from_pixbuf(g_color_circles[TEG_MAX_PLAYERS]);
		gtk_box_pack_start(GTK_BOX(mainstatus_canvas), players_color[i], FALSE, FALSE, 0);
		gtk_widget_set_visible(players_color[i], FALSE);
	}

	players_color_over = gtk_image_new_from_pixbuf(g_color_circle_over);
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), players_color_over, FALSE, FALSE, 0);
	gtk_widget_set_visible(players_color_over, FALSE);

	mainstatus_update();

	*window = mainstatus_canvas;
	gtk_widget_set_visible(mainstatus_canvas, TRUE);
	return TEG_STATUS_SUCCESS;

error:
	return TEG_STATUS_ERROR;
}

TEG_STATUS mainstatus_update_colors()
{
	int i;
	PLIST_ENTRY l;
	PCPLAYER pJ;

	if( ! mainstatus_canvas )
		return TEG_STATUS_ERROR;

	l = g_list_player.Flink;

	i=0;

	gtk_widget_set_visible(players_color_over, FALSE);
	while( !IsListEmpty( &g_list_player ) && (l != &g_list_player) )
	{
		pJ = (PCPLAYER) l;

		if( pJ->color >= 0 && pJ->numjug >= 0 ) {
			gtk_widget_set_visible(players_color[i], TRUE);

			if( g_game.whos_turn == pJ->numjug )
			{
				gtk_widget_set_visible(players_color_over, TRUE);
			}

			gtk_image_set_from_pixbuf(GTK_IMAGE(players_color[i]), g_color_circles[pJ->color]);

			i++;
		}

		l = LIST_NEXT(l);

		if( i >= TEG_MAX_PLAYERS )
			break;
	}

	{
		PCPLAYER pJ;
		gtk_widget_set_visible(color_started_item, FALSE);
		if( g_game.who_started_round >= 0 && g_game.who_started_round < TEG_MAX_PLAYERS ) {

			if( player_whois( g_game.who_started_round, &pJ ) == TEG_STATUS_SUCCESS )
			{
				gtk_image_set_from_pixbuf(GTK_IMAGE(color_started_item), g_color_circles[pJ->color]);
				gtk_widget_set_visible(color_started_item, TRUE);
			}
		}
	}

	for( ; i < TEG_MAX_PLAYERS ; i++ )
		gtk_widget_set_visible(players_color[i], FALSE);

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS mainstatus_update()
{
	char buffer[256];
	static int offset_right=-1, offset_left=-1;
	static GtkWidget *gamestatus_item = NULL;
	static GtkWidget *country_item = NULL;
	static GtkWidget *cont_item = NULL;

	if( ! mainstatus_canvas )
		return TEG_STATUS_ERROR;

	if( offset_left == -1 && offset_right == -1)
	{
		offset_left = gui_theme.toolbar_offset_left != -1 ? gui_theme.toolbar_offset_left : OFFSET_LEFT;
		offset_right = gui_theme.toolbar_offset_right != -1 ? gui_theme.toolbar_offset_right : OFFSET_RIGHT;
	}

	/* game status */
	if( gamestatus_item  )
		gtk_widget_destroy(gamestatus_item);

	gamestatus_item = gtk_label_new(_(g_estados[g_game.estado]));
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), gamestatus_item, FALSE, FALSE, 0);
	gtk_widget_set_visible(gamestatus_item, TRUE);

	/* country */
	if( country_item  )
		gtk_widget_destroy(country_item);

	if( gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT )
		snprintf(buffer,sizeof(buffer)-1," ");
	else
		snprintf(buffer,sizeof(buffer)-1,"%s", countries_get_name(gui_private.country_i_am) );
		
	country_item = gtk_label_new(buffer);
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), country_item, FALSE, FALSE, 0);
	gtk_widget_set_visible(country_item, TRUE);

	/* continent */
	if( cont_item  )
		gtk_widget_destroy(cont_item);

	if( gui_private.country_i_am < 0 || gui_private.country_i_am >= COUNTRIES_CANT )
		snprintf(buffer,sizeof(buffer)-1," ");
	else
		snprintf(buffer,sizeof(buffer)-1,"%s", cont_get_name( g_countries[gui_private.country_i_am].continente )  );
		
	cont_item = gtk_label_new(buffer);
	gtk_box_pack_start(GTK_BOX(mainstatus_canvas), cont_item, FALSE, FALSE, 0);
	gtk_widget_set_visible(cont_item, TRUE);

	/* round number */
	{
		char buffer[256];

		gtk_widget_set_visible(round_number_item, FALSE);
		if( g_game.round_number >= 0 ) {

			sprintf(buffer,"%d",g_game.round_number );

			gtk_label_set_text(GTK_LABEL(round_number_item), buffer);
			gtk_widget_set_visible(round_number_item, TRUE);
		}
	}

	mainstatus_update_colors();

	return TEG_STATUS_SUCCESS;
}
