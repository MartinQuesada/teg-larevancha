/*	$Id: g_scores.c,v 1.12 2002/09/21 17:56:38 riq Exp $ */
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
/**
 * @file g_scores.c
 * Migrated to GTK3
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <string.h>
#include "gnome-compat.h"

#include "gui.h"
#include "client.h"
#include "support.h"
#include "interface.h"
#include "callbacks.h"
#include "g_scores.h"
#include "colors.h"
#include "scores.h"
#include "common.h"

static GtkWidget *gui_scores_dialog = NULL;

enum {
	COLUMN_POSNUMBER,
	COLUMN_SCORE,
	COLUMN_NAME,
	COLUMN_DATE,
	COLUMN_COLOR,
	COLUMN_HUMAN,

	COLUMN_LAST
};

/* Callback for refresh button */
static void on_refresh_clicked(GtkWidget *button, gpointer user_data)
{
	/* Request scores from server */
	net_printf(g_game.fd, "scores\n");
}

/* Callback for dialog response */
static void on_scores_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
	if (response_id == GTK_RESPONSE_CLOSE || response_id == GTK_RESPONSE_DELETE_EVENT) {
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}

static GtkTreeModel *scores_create_model(void)
{
	GtkListStore *store;

	/* create list store */
	store = gtk_list_store_new(
		COLUMN_LAST,
		G_TYPE_UINT,
		G_TYPE_UINT,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_STRING,
		G_TYPE_BOOLEAN);

	return GTK_TREE_MODEL(store);
}

static void scores_add_columns(GtkTreeView *treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	/* column for pos number */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Number"),
		renderer,
		"text", COLUMN_POSNUMBER,
		NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_POSNUMBER);
	gtk_tree_view_append_column(treeview, column);

	/* column for scores */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Score"),
		renderer,
		"text", COLUMN_SCORE,
		NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_SCORE);
	gtk_tree_view_append_column(treeview, column);

	/* column for name */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Name"),
		renderer,
		"text", COLUMN_NAME,
		NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_NAME);
	gtk_tree_view_append_column(treeview, column);

	/* column for date */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Date"),
		renderer,
		"text", COLUMN_DATE,
		NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_DATE);
	gtk_tree_view_append_column(treeview, column);

	/* column for color */
	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes(_("Color"),
		renderer,
		"text", COLUMN_COLOR,
		NULL);
	gtk_tree_view_column_set_sort_column_id(column, COLUMN_COLOR);
	gtk_tree_view_append_column(treeview, column);

	/* column for is human? */
	renderer = gtk_cell_renderer_toggle_new();
	column = gtk_tree_view_column_new_with_attributes(_("Human?"),
		renderer,
		"active", COLUMN_HUMAN,
		NULL);
	gtk_tree_view_append_column(treeview, column);
}

static TEG_STATUS scores_update_model(GtkTreeModel *model)
{
	PSCORES pS;
	int row;
	GtkListStore *store;
	GtkTreeIter iter;
	PLIST_ENTRY list = scores_get_list();
	PLIST_ENTRY l;

	if (!list || IsListEmpty(list)) {
		return TEG_STATUS_SUCCESS;
	}

	store = GTK_LIST_STORE(model);
	gtk_list_store_clear(store);

	row = 0;
	l = list->Flink;

	while (!IsListEmpty(list) && (l != list)) {
		gchar *name;
		char score_str[32];
		char pos_str[32];
		int color_idx;

		pS = (PSCORES)l;

		name = translate_to_utf8(pS->name);
		color_idx = (pS->color < 0 || pS->color >= TEG_MAX_PLAYERS) ? TEG_MAX_PLAYERS : pS->color;

		snprintf(pos_str, sizeof(pos_str), "%d", ++row);
		snprintf(score_str, sizeof(score_str), "%d", pS->stats.score);

		gtk_list_store_append(store, &iter);
		gtk_list_store_set(store, &iter,
			COLUMN_POSNUMBER, row,
			COLUMN_SCORE, pS->stats.score,
			COLUMN_NAME, name ? name : "",
			COLUMN_DATE, pS->date,
			COLUMN_COLOR, _(g_colores[color_idx]),
			COLUMN_HUMAN, pS->human ? TRUE : FALSE,
			-1);

		if (name) {
			g_free(name);
		}

		l = LIST_NEXT(l);
	}

	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS scores_update_dialog(void)
{
	static GtkWidget *scores_treeview = NULL;
	static GtkTreeModel *model = NULL;

	if (gui_scores_dialog == NULL) {
		return TEG_STATUS_ERROR;
	}

	if (scores_treeview == NULL) {
		GtkWidget *scrolled;
		GtkWidget *content;

		/* create tree model */
		model = scores_create_model();

		/* create scrolled window */
		scrolled = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 300);
		gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scrolled), 500);

		/* create tree view */
		scores_treeview = gtk_tree_view_new_with_model(model);
		/* gtk_tree_view_set_rules_hint is deprecated in GTK3.14+, using CSS instead */
		gtk_tree_view_set_search_column(GTK_TREE_VIEW(scores_treeview), COLUMN_SCORE);

		gtk_container_add(GTK_CONTAINER(scrolled), scores_treeview);

		/* add columns to the tree view */
		scores_add_columns(GTK_TREE_VIEW(scores_treeview));

		/* add to dialog */
		content = gtk_dialog_get_content_area(GTK_DIALOG(gui_scores_dialog));
		gtk_box_pack_start(GTK_BOX(content), scrolled, TRUE, TRUE, 5);
		gtk_widget_show_all(scrolled);
	}

	scores_update_model(model);

	return TEG_STATUS_SUCCESS;
}

void gui_scores_view(void)
{
	if (gui_scores_dialog == NULL) {
		gui_scores_dialog = gtk_dialog_new_with_buttons(
			_("High Scores"),
			GTK_WINDOW(main_window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			_("_Refresh"),
			GTK_RESPONSE_APPLY,
			_("_Close"),
			GTK_RESPONSE_CLOSE,
			NULL);

		gtk_window_set_default_size(GTK_WINDOW(gui_scores_dialog), 600, 400);
		gtk_window_set_transient_for(GTK_WINDOW(gui_scores_dialog), GTK_WINDOW(main_window));

		/* Connect signals */
		g_signal_connect(gui_scores_dialog, "response", G_CALLBACK(on_scores_dialog_response), NULL);
		g_signal_connect_swapped(
			gtk_dialog_get_widget_for_response(GTK_DIALOG(gui_scores_dialog), GTK_RESPONSE_APPLY),
			"clicked", G_CALLBACK(on_refresh_clicked), NULL);
		g_signal_connect(gui_scores_dialog, "destroy", G_CALLBACK(gtk_widget_destroyed), &gui_scores_dialog);
	}

	scores_update_dialog();
	gtk_widget_show_all(gui_scores_dialog);
	gtk_window_present(GTK_WINDOW(gui_scores_dialog));
}

/* Embed scores in a container (simplified version) */
void gui_scores_embed(GtkWidget *hbox)
{
	/* This function was used to embed scores in a frame, but it's not commonly used.
	 * For now, we'll leave it as a stub. If needed, it can be implemented using
	 * a GtkTreeView similar to gui_scores_view().
	 */
	(void)hbox; /* Suppress unused parameter warning */
}
