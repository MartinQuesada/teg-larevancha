/*	$Id: preferences.c,v 1.24 2002/10/08 04:35:57 riq Exp $	*/
/* Tenes Empanadas Graciela
 *
 * Copyright (C) 2001 Ricardo Quesada
 *
 * Author: Ricardo Calixto Quesada <riq@core-sdi.com>
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
 * @file preferences.c
 * Migrated to GTK3
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include "gnome-compat.h"
#include "client.h"
#include "gui.h"
#include "priv.h"
#include "interface.h"
#include "support.h"
#include "preferences.h"
#include "themes.h"

static GtkWidget *pref_dialog = NULL;
static GtkWidget *theme_combo = NULL;
static GtkWidget *theme_preview_image = NULL;

/* Update theme preview image */
static void update_theme_preview(const char *theme_name)
{
	if (!theme_preview_image || !theme_name)
		return;
	
	char *preview_path = theme_load_fake_file("mini_shot.png", (char*)theme_name);
	if (preview_path) {
		GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(preview_path, NULL);
		if (pixbuf) {
			/* Scale to reasonable size if needed */
			int width = gdk_pixbuf_get_width(pixbuf);
			int height = gdk_pixbuf_get_height(pixbuf);
			int max_width = 300;
			int max_height = 200;
			
			if (width > max_width || height > max_height) {
				double scale = MIN((double)max_width / width, (double)max_height / height);
				int new_width = (int)(width * scale);
				int new_height = (int)(height * scale);
				GdkPixbuf *scaled = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
				g_object_unref(pixbuf);
				pixbuf = scaled;
			}
			
			gtk_image_set_from_pixbuf(GTK_IMAGE(theme_preview_image), pixbuf);
			g_object_unref(pixbuf);
		} else {
			gtk_image_clear(GTK_IMAGE(theme_preview_image));
		}
	} else {
		gtk_image_clear(GTK_IMAGE(theme_preview_image));
	}
}

/* Callback when message checkboxes change */
static void on_msg_err_toggled(GtkToggleButton *button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
		g_game.msg_show |= M_ERR;
	else
		g_game.msg_show &= ~M_ERR;
}

static void on_msg_msg_toggled(GtkToggleButton *button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
		g_game.msg_show |= M_MSG;
	else
		g_game.msg_show &= ~M_MSG;
}

static void on_msg_inf_toggled(GtkToggleButton *button, gpointer data)
{
	if (gtk_toggle_button_get_active(button))
		g_game.msg_show |= M_INF;
	else
		g_game.msg_show &= ~M_INF;
}

/* Callback when theme selection changes */
static void on_theme_changed(GtkComboBox *combo, gpointer data)
{
	gchar *selected = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));
	if (selected) {
		/* Prevent selection of "modern" */
		if (strcmp(selected, "modern") == 0) {
			/* Revert to sentimental */
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
			g_free(selected);
			return;
		}
		
		strncpy(g_game.theme, selected, THEME_MAX_NAME - 1);
		g_game.theme[THEME_MAX_NAME - 1] = '\0';
		update_theme_preview(selected);
		g_free(selected);
	}
}

/* GTK3: Preferences dialog with functional theme selector */
void preferences_activate(void)
{
	if (pref_dialog == NULL) {
		pref_dialog = gtk_dialog_new_with_buttons(
			_("Preferences"),
			GTK_WINDOW(main_window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			_("_Close"),
			GTK_RESPONSE_CLOSE,
			NULL);

		gtk_window_set_default_size(GTK_WINDOW(pref_dialog), 400, 350);

		GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(pref_dialog));
		gtk_container_set_border_width(GTK_CONTAINER(content), 10);
		
		/* Messages frame */
		GtkWidget *frame_msg = gtk_frame_new(_("Messages"));
		gtk_box_pack_start(GTK_BOX(content), frame_msg, FALSE, FALSE, 5);
		
		GtkWidget *vbox_msg = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
		gtk_container_set_border_width(GTK_CONTAINER(vbox_msg), 5);
		gtk_container_add(GTK_CONTAINER(frame_msg), vbox_msg);
		
		GtkWidget *cb_showerr = gtk_check_button_new_with_label(_("Show error messages"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_showerr), 
			(g_game.msg_show & M_ERR) ? TRUE : FALSE);
		g_signal_connect(cb_showerr, "toggled", G_CALLBACK(on_msg_err_toggled), NULL);
		gtk_box_pack_start(GTK_BOX(vbox_msg), cb_showerr, FALSE, FALSE, 0);
		
		GtkWidget *cb_showmsg = gtk_check_button_new_with_label(_("Show game messages"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_showmsg),
			(g_game.msg_show & M_MSG) ? TRUE : FALSE);
		g_signal_connect(cb_showmsg, "toggled", G_CALLBACK(on_msg_msg_toggled), NULL);
		gtk_box_pack_start(GTK_BOX(vbox_msg), cb_showmsg, FALSE, FALSE, 0);
		
		GtkWidget *cb_showinf = gtk_check_button_new_with_label(_("Show info messages"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(cb_showinf),
			(g_game.msg_show & M_INF) ? TRUE : FALSE);
		g_signal_connect(cb_showinf, "toggled", G_CALLBACK(on_msg_inf_toggled), NULL);
		gtk_box_pack_start(GTK_BOX(vbox_msg), cb_showinf, FALSE, FALSE, 0);

		/* Theme frame */
		GtkWidget *frame_theme = gtk_frame_new(_("Theme"));
		gtk_box_pack_start(GTK_BOX(content), frame_theme, FALSE, FALSE, 5);
		
		GtkWidget *vbox_theme = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		gtk_container_set_border_width(GTK_CONTAINER(vbox_theme), 5);
		gtk_container_add(GTK_CONTAINER(frame_theme), vbox_theme);
		
		/* Theme selector combo box */
		GtkWidget *hbox_theme = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
		gtk_box_pack_start(GTK_BOX(vbox_theme), hbox_theme, FALSE, FALSE, 0);
		
		GtkWidget *theme_label = gtk_label_new(_("Select theme:"));
		gtk_box_pack_start(GTK_BOX(hbox_theme), theme_label, FALSE, FALSE, 0);
		
		theme_combo = gtk_combo_box_text_new();
		gtk_box_pack_start(GTK_BOX(hbox_theme), theme_combo, TRUE, TRUE, 0);
		
		/* Populate combo with only Sentimental and Modern */
		int active_index = 0;
		
		/* Add Sentimental */
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(theme_combo), "sentimental");
		if (strcmp("sentimental", g_game.theme) == 0) {
			active_index = 0;
		}
		
		/* Add Modern (will be disabled) */
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(theme_combo), "modern");
		if (strcmp("modern", g_game.theme) == 0) {
			active_index = 1;
		}
		
		/* If current theme is not one of these, default to sentimental */
		if (strcmp("sentimental", g_game.theme) != 0 && strcmp("modern", g_game.theme) != 0) {
			active_index = 0;
		}
		
		gtk_combo_box_set_active(GTK_COMBO_BOX(theme_combo), active_index);
		g_signal_connect(theme_combo, "changed", G_CALLBACK(on_theme_changed), NULL);

		/* Theme preview image */
		GtkWidget *preview_frame = gtk_frame_new(_("Preview"));
		gtk_box_pack_start(GTK_BOX(vbox_theme), preview_frame, TRUE, TRUE, 5);
		
		GtkWidget *preview_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
		gtk_container_add(GTK_CONTAINER(preview_frame), preview_box);
		gtk_widget_set_size_request(preview_box, 300, 200);
		
		theme_preview_image = gtk_image_new();
		gtk_widget_set_halign(theme_preview_image, GTK_ALIGN_CENTER);
		gtk_widget_set_valign(theme_preview_image, GTK_ALIGN_CENTER);
		gtk_box_pack_start(GTK_BOX(preview_box), theme_preview_image, TRUE, TRUE, 5);
		
		/* Load initial preview */
		update_theme_preview(g_game.theme);

		/* Note about preferences */
		GtkWidget *note_label = gtk_label_new(_("Note: Theme changes require restart."));
		gtk_widget_set_halign(note_label, GTK_ALIGN_START);
		gtk_box_pack_start(GTK_BOX(vbox_theme), note_label, FALSE, FALSE, 5);

		g_signal_connect(pref_dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
		g_signal_connect(pref_dialog, "destroy", G_CALLBACK(gtk_widget_destroyed), &pref_dialog);
	}

	gtk_widget_show_all(pref_dialog);
	gtk_window_present(GTK_WINDOW(pref_dialog));
}
