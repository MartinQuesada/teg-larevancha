/*	$Id: interface.c,v 1.96 2002/08/31 17:52:54 riq Exp $	*/
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
/* file generated by glade and modified by riq */
/**
 * @file interface.c
 * Maneja las ventanas que se crean, salvo algunas
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "gui.h"
#include "client.h"
#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "g_country.h"
#include "g_cont.h"
#include "stock.h"
#include "status.h"
#include "armies.h"
#include "dices.h"
#include "themes.h"
#include "fonts.h"

#include "robot.xpm"

extern TTheme gui_theme;

GtkTextView *main_message_area = NULL;
GtkWidget *main_window = NULL;
GtkWidget *statusbar = NULL;
GtkWidget *canvas_map = NULL;
GtkWidget *window_mainstatus = NULL;

double canvas_map_zoom =1.00;

static GtkWidget *tropas_dialog=NULL;
static GtkWidget *tropas_hscale_cant=NULL;
static int tropas_cant;
static int tropas_src;
static int tropas_dst;

static GtkWidget *reagrupe_dialog=NULL;
static GtkWidget *reagrupe_hscale_cant=NULL;
static int reagrupe_cant=-1;
static int reagrupe_src=-1;
static int reagrupe_dst=-1;

static void canvas_map_zoom_in(GtkWidget *widget, gpointer data);
static void canvas_map_zoom_out(GtkWidget *widget, gpointer data);
static void canvas_map_zoom_1(GtkWidget *widget, gpointer data);

/*
 * IMPORTANT:
 * if you add/remove an entry in the menu, modify this
 */
enum {
	ON_CONNECT,
	ON_DISCONNECT,
	ON_GAME_SEPARATOR_1,
	ON_LAUNCHROBOT,
	ON_GAME_SEPARATOR_2,
	ON_SCORES,
	ON_GAME_SEPARATOR_3,
	ON_EXIT
};

static GnomeUIInfo game_menu_uiinfo[] =
{
	{ GNOME_APP_UI_ITEM, N_("_Connect"), N_("Connect to server"),
	on_connect_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("_Disconnect"), N_("Disconnect from server"),
	on_disconnect_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	GNOMEUIINFO_SEPARATOR,

	{ GNOME_APP_UI_ITEM, N_("_Launch robot"), N_("Launch a robot"), 
	on_launchrobot_activate, NULL, NULL, 
	GNOME_APP_PIXMAP_DATA, robot_xpm,
	0, 0, NULL }, 

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_MENU_SCORES_ITEM (on_scores_activate, NULL),

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_MENU_EXIT_ITEM (on_exit_activate, NULL),
	GNOMEUIINFO_END
};


/*
 * IMPORTANT:
 * Si se agrega o se saca algo de actions_menu_uiinfo, sacarlo de aca
 */
enum {
	ON_START,
	ON_FICHAS,
	ON_UPDATEMAP,
	ON_ATTACK_RE,
	ON_ATTACK_RESET,
	ON_REGROUP,
	ON_GETCARD,
	ON_ENDTURN,
	ON_ACTIONS_SEPARATOR,
	ON_SURRENDER,
};


static GnomeUIInfo actions_menu_uiinfo[] =
{
	{ GNOME_APP_UI_ITEM, N_("Start"), N_("Start the game"),
	on_start_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("Send armies"), N_("Send the armies recently placed"),
	on_enviarfichas_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("_Update Map"), N_("Show the armies that others player had placed"),
	on_updatemap_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("_Reattck"), N_("Attack your enemy, again using same source and destination"),
	on_enviarreattack_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	'r', GDK_CONTROL_MASK, NULL },

	{ GNOME_APP_UI_ITEM, N_("Reset the attack"), N_("Start again selecting the source and destination country for the attack"),
	on_enviarattackreset_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("Re_group armies"), N_("Regroup your armies, moving from one country to another"),
	on_reagrupe_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("Get card"), N_("Get a country card"),
	on_pedirtarjeta_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("_End turn"), N_("Pass the turn to another player"),
	on_endturn_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	GNOMEUIINFO_SEPARATOR,

	{ GNOME_APP_UI_ITEM, N_("Surrender"), N_("I'm a coward"),
	on_surrender_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	GNOMEUIINFO_END
};

/*
static GnomeUIInfo edit_menu_uiinfo[] =
{
	GNOMEUIINFO_MENU_CUT_ITEM (on_cut1_activate, NULL),
	GNOMEUIINFO_MENU_COPY_ITEM (on_copy1_activate, NULL),
	GNOMEUIINFO_MENU_PASTE_ITEM (on_paste1_activate, NULL),
	GNOMEUIINFO_MENU_CLEAR_ITEM (on_clear1_activate, NULL),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_MENU_PROPERTIES_ITEM (on_properties1_activate, NULL),
	GNOMEUIINFO_END
};
*/

/*
 * IMPORTANT:
 * Si se agrega o se saca algo de view_menu_uiinfo, sacarlo de aca
 */
enum {
	ON_STATUS,
	ON_MISSION,
	ON_CARDS,
};
static GnomeUIInfo view_menu_uiinfo[] =
{
	{ GNOME_APP_UI_ITEM, N_("View _players"), N_("View the status of the players"),
	on_status_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("View _mission"), N_("View your mission of this game"),
	on_viewmission_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

	{ GNOME_APP_UI_ITEM, N_("View _cards"), N_("View all the card that you won"),
	on_viewcards_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },

#if 0
	{ GNOME_APP_UI_TOGGLEITEM, N_("View _dices"), N_("View the result of the dices"),
	on_viewdices_activate, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	0, 0, NULL },
#endif

	GNOMEUIINFO_SEPARATOR,

	{ GNOME_APP_UI_ITEM, N_("Zoom In"), N_("Increase zoom factor by 5%"),
	canvas_map_zoom_in, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	'=', GDK_CONTROL_MASK, NULL },

	{ GNOME_APP_UI_ITEM, N_("Zoom Out"), N_("Decrease zoom factor by 5%"),
	canvas_map_zoom_out, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	'-', GDK_CONTROL_MASK, NULL },

	{ GNOME_APP_UI_ITEM, N_("Zoom _1:1"), N_("Display the map at 1:1 scale"),
	canvas_map_zoom_1, NULL, NULL,
	GNOME_APP_PIXMAP_NONE, NULL,
	'1', GDK_CONTROL_MASK, NULL },

	GNOMEUIINFO_END
};

static GnomeUIInfo settings_menu_uiinfo[] =
{
	GNOMEUIINFO_MENU_PREFERENCES_ITEM (on_preferences1_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo help_menu_uiinfo[] =
{
        GNOMEUIINFO_HELP("teg"),
	GNOMEUIINFO_MENU_ABOUT_ITEM (on_about_activate, NULL),
	GNOMEUIINFO_END
};

static GnomeUIInfo menubar_uiinfo[] =
{
	GNOMEUIINFO_MENU_GAME_TREE (game_menu_uiinfo),
	GNOMEUIINFO_SUBTREE(N_("_Actions"), actions_menu_uiinfo),
/*	GNOMEUIINFO_MENU_EDIT_TREE (edit_menu_uiinfo), */
	GNOMEUIINFO_MENU_VIEW_TREE (view_menu_uiinfo),
	GNOMEUIINFO_MENU_SETTINGS_TREE (settings_menu_uiinfo),
	GNOMEUIINFO_MENU_HELP_TREE (help_menu_uiinfo),
	GNOMEUIINFO_END
};

GnomeUIInfo toolbar_main[] =
{
	{GNOME_APP_UI_ITEM, N_("Connect"), N_("Connect to server"),
	on_connect_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_NEW, 0, 0, NULL},

	{GNOME_APP_UI_ITEM, N_("Start"), N_("Start the game"),
	on_start_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_TIMER, 0, 0, NULL},

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_ITEM_STOCK (N_("Send armies"), N_("Send your armies"), on_enviarfichas_activate, STOCK_SENDARMIES),

	{GNOME_APP_UI_ITEM, N_("1. Regroup"), N_("Regroup your armies, moving from one country to another"),
	on_reagrupe_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_REFRESH, 0, 0, NULL},

	GNOMEUIINFO_ITEM_STOCK (N_("2. Get card"), N_("Get a country card"), on_pedirtarjeta_activate, STOCK_GETCARD),

	GNOMEUIINFO_ITEM_STOCK (N_("3. End turn"), N_("Pass the turn to another player"), on_endturn_activate, STOCK_ENDTURN),

	GNOMEUIINFO_SEPARATOR,

	GNOMEUIINFO_ITEM_STOCK (N_("View players"), N_("View players status"), on_status_activate, STOCK_VIEWPLAYERS),
#if 0
	GNOMEUIINFO_ITEM_STOCK (N_("In"), NULL, canvas_map_zoom_in, STOCK_ZOOM_IN),
	GNOMEUIINFO_ITEM_STOCK (N_("Out"), NULL,canvas_map_zoom_out, STOCK_ZOOM_OUT),
	GNOMEUIINFO_ITEM_STOCK (N_("1:1"), NULL,canvas_map_zoom_1, STOCK_ZOOM_1),
	GNOMEUIINFO_SEPARATOR,
#endif


#if 0
	GNOMEUIINFO_SEPARATOR,
	{GNOME_APP_UI_ITEM, N_("Exit"), N_("Exit TEG"),
	on_exit_activate, NULL, NULL,
	GNOME_APP_PIXMAP_STOCK, GNOME_STOCK_PIXMAP_EXIT, 0, 0, NULL},
#endif

	GNOMEUIINFO_END
};


struct _SensiData {
	GtkWidget *connect_button;
	GtkWidget *start_button;
	GtkWidget *enviarfichas_button;
	GtkWidget *reagrupe_button;
	GtkWidget *getcard_button;
	GtkWidget *endturn_button;
	GtkWidget *status_button;
} SensiData;


static void
canvas_map_zoom_1(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom = 1.0;
	gnome_canvas_set_pixels_per_unit ((GnomeCanvas*)canvas_map, canvas_map_zoom);
}

static void
canvas_map_zoom_in(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom *= 1.05;
	gnome_canvas_set_pixels_per_unit ((GnomeCanvas*)canvas_map, canvas_map_zoom);
}

static void
canvas_map_zoom_out(GtkWidget *widget, gpointer data)
{
	canvas_map_zoom *= 0.95;
	gnome_canvas_set_pixels_per_unit ((GnomeCanvas*)canvas_map, canvas_map_zoom);
}

static TEG_STATUS
canvas_create_countries( GnomeCanvasGroup *group)
{
	int i=0;

	for(i=0;i<CONT_CANT;i++)
		G_cont_create(i,group);

	for(i=0;i<COUNTRIES_CANT;i++) {
		G_country_create(i);
		G_country_draw(i);
		G_country_draw_ejer(i);
	}

	/* Pone a los ejercitos arriba de todo */
#if 0
	for(i=0;i<n;i++) {
		if( G_countries[i].ellip_item)
			gnome_canvas_item_raise_to_top ( G_countries[i].ellip_item );
		if( G_countries[i].text_item)
			gnome_canvas_item_raise_to_top ( G_countries[i].text_item );
	}
#endif
	return TEG_STATUS_SUCCESS;
}

static TEG_STATUS canvas_create_map( void )
{
	char *filename;
	GnomeCanvasGroup* root;
	GdkPixbuf *im;
	GnomeCanvasItem *image;

	gtk_widget_push_colormap (gdk_rgb_get_cmap ());
	canvas_map = gnome_canvas_new ();
	root = gnome_canvas_root(GNOME_CANVAS(canvas_map));

	filename = theme_load_file( gui_theme.board );

	im = gdk_pixbuf_new_from_file(filename, NULL);

	if (im){
		image = gnome_canvas_item_new (
				root,
				gnome_canvas_pixbuf_get_type (),
				"pixbuf", im,
				"x", 0.0,
				"y", 0.0,
				"width", (double) gdk_pixbuf_get_width(im),
				"height", (double) gdk_pixbuf_get_height(im),
				"anchor", GTK_ANCHOR_NW,
				NULL);
		canvas_create_countries(root);
		armies_init(root);
		dices_init(root);
		gnome_canvas_set_scroll_region(GNOME_CANVAS(canvas_map),0,0,gdk_pixbuf_get_width(im),gdk_pixbuf_get_height(im));

		gdk_pixbuf_unref(im);

	} else {
		g_warning (_("Could not find the %s file"),gui_theme.board);
		return TEG_STATUS_ERROR;
	}


	gtk_widget_pop_colormap ();
	gtk_widget_pop_visual ();

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn GtkWidget* create_mainwin (void)
 * Crea la ventana principal
 */
GtkWidget* create_mainwin (void)
{
	GtkWidget *app1;
	GtkWidget *dock1;
	GtkWidget *eventbox1;
	GtkWidget *viewport1;
	GtkWidget *vbox_main;
	GtkWidget *vbox_paned;
	GtkWidget *scrolledwindow;
	GtkWidget *paned;
	GtkWidget *text;
	GtkWidget *inputline;
	GtkWidget *sw;
	GtkWidget *ahbox;

	app1 = gnome_app_new ( PACKAGE, "Tenes Empanadas Graciela");
	gtk_object_set_data (GTK_OBJECT (app1), "app1", app1);
	gtk_window_set_default_size (GTK_WINDOW (app1), 664, 768);
	gtk_window_set_policy (GTK_WINDOW (app1), TRUE, TRUE, FALSE);

	dock1 = GNOME_APP (app1)->dock;
	gtk_widget_ref (dock1);
	gtk_object_set_data_full (GTK_OBJECT (app1), "dock1", dock1,
		(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (dock1);

	gnome_app_create_menus (GNOME_APP (app1), menubar_uiinfo);
	gnome_app_create_toolbar(GNOME_APP(app1), toolbar_main);

	/* TODO: ask in GNOME if this is wanted */
	/* gtk_menu_item_right_justify( GTK_MENU_ITEM(menubar_uiinfo[4].widget)); */

	gtk_widget_realize (app1);

	gtk_signal_connect ( GTK_OBJECT(app1), "destroy",
		GTK_SIGNAL_FUNC( on_exit_activate ), NULL );

	gtk_signal_connect ( GTK_OBJECT(app1), "delete_event",
		GTK_SIGNAL_FUNC( on_exit_activate ), NULL );


	SensiData.connect_button = toolbar_main[0].widget;
	SensiData.start_button = toolbar_main[1].widget;
	/* 2 is a separator */
	SensiData.enviarfichas_button = toolbar_main[3].widget;
	SensiData.reagrupe_button = toolbar_main[4].widget;
	SensiData.getcard_button = toolbar_main[5].widget;
	SensiData.endturn_button = toolbar_main[6].widget;
	/* 7 is a separator */
	SensiData.status_button = toolbar_main[8].widget;

	eventbox1 = gtk_event_box_new ();
	gtk_widget_ref (eventbox1);
	gtk_object_set_data_full (GTK_OBJECT (app1), "eventbox1", eventbox1,
		(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (eventbox1);
	gnome_app_set_contents (GNOME_APP (app1), eventbox1);

	viewport1 = gtk_viewport_new (NULL, NULL);
	gtk_widget_show (viewport1);
	gtk_container_add (GTK_CONTAINER (eventbox1), viewport1);

	vbox_main = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox_main);
	gtk_container_add (GTK_CONTAINER (viewport1), vbox_main);

	/* status map */
	if( mainstatus_create(&window_mainstatus) != TEG_STATUS_SUCCESS )
		goto error;
	gtk_box_pack_start( GTK_BOX(vbox_main), window_mainstatus, FALSE,FALSE,0);

	/* paned */
	paned = gtk_vpaned_new();
	gtk_box_pack_start( GTK_BOX( vbox_main ), paned, TRUE, TRUE, 0 );
	vbox_paned = gtk_vbox_new (FALSE, 0);
	gtk_paned_add1(GTK_PANED(paned), vbox_paned);
	gtk_widget_show (vbox_paned);

	/* map */
	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW(scrolledwindow),
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledwindow, 640, 500);

	if( canvas_create_map() != TEG_STATUS_SUCCESS )
		goto error;

	gtk_container_add(GTK_CONTAINER(scrolledwindow), canvas_map);
	gtk_container_add(GTK_CONTAINER(vbox_paned),scrolledwindow);
	gtk_widget_show (canvas_map);
	gtk_widget_show (scrolledwindow);


	/* text */
	ahbox = gtk_hbox_new(FALSE, 2);
	gtk_paned_pack2(GTK_PANED(paned), ahbox, TRUE, TRUE);

	sw = gtk_scrolled_window_new(NULL, NULL);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw), GTK_POLICY_AUTOMATIC,
					 GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(ahbox), sw, TRUE, TRUE, 0);

	gtk_widget_set_size_request(sw, 620, 60);

	text = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text), FALSE);
	gtk_container_add(GTK_CONTAINER(sw), text);

	gtk_widget_set_name(text, "chatline");

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text), GTK_WRAP_WORD);
	gtk_widget_realize(text);
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(text), 5);

	main_message_area = GTK_TEXT_VIEW(text);

/*	set_output_window_text(_("Tenes Empanadas Graciela")); */

	gtk_widget_show(text);
	gtk_widget_show(sw);
	gtk_widget_show(ahbox);
	gtk_widget_show(paned);

	/* the chat line */
	inputline = gtk_entry_new();
	gtk_box_pack_start( GTK_BOX( vbox_main ), inputline, FALSE, FALSE, 0 );
	gtk_widget_show(inputline);
	g_signal_connect(inputline, "activate", G_CALLBACK(inputline_return), NULL);


	chatline_init();


	/* status bar */
	statusbar = gnome_appbar_new (FALSE, TRUE, GNOME_PREFERENCES_NEVER);
	gtk_widget_ref (statusbar);
	gtk_object_set_data_full (GTK_OBJECT (app1), "statusbar", statusbar,
		(GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (statusbar);
	gnome_app_set_statusbar (GNOME_APP (app1), statusbar);

	gtk_box_pack_start(GTK_BOX(statusbar),
		ministatus_build(), FALSE, TRUE, 0);


	gnome_appbar_set_status(GNOME_APPBAR(statusbar),
				_("Welcome to TEG!"));

	gnome_app_install_menu_hints( GNOME_APP(app1), menubar_uiinfo );

	/* icono */
	{
		GdkBitmap *mask;
		GdkPixmap *icono;
		#include "../teg_pix/teg_icono.xpm"
		gtk_widget_realize (app1);
		icono = gdk_pixmap_create_from_xpm_d( app1->window, &mask, &app1->style->bg[GTK_STATE_NORMAL],teg_icono_xpm );
		gdk_window_set_icon (app1->window, NULL, icono , mask );
	}

	gtk_widget_show(app1);
	return app1;

error:
	g_error(_("Unable to create map!"));
	return NULL;
}

/**
 * @fn TEG_STATUS set_sensitive_tb(void)
 * 'prende' y 'apaga' los botones segun el estado del game
 */
TEG_STATUS set_sensitive_tb(void)
{
	PLAYER_STATUS e = ESTADO_GET();

	/* CONNECT & START */
	if( e == PLAYER_STATUS_DESCONECTADO ) {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_CONNECT].widget,TRUE);
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_DISCONNECT].widget,FALSE);
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_START].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.connect_button, TRUE);
		gtk_widget_set_sensitive (SensiData.start_button, FALSE);

	} else if( e == PLAYER_STATUS_HABILITADO ) {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_CONNECT].widget,FALSE);
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_DISCONNECT].widget,TRUE);
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_START].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.connect_button, FALSE);
		gtk_widget_set_sensitive (SensiData.start_button, TRUE);

	} else {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_CONNECT].widget,FALSE);
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_DISCONNECT].widget,TRUE);
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_START].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.connect_button, FALSE);
		gtk_widget_set_sensitive (SensiData.start_button, FALSE);
	}

	if( g_game.with_ggz ) {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_CONNECT].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.connect_button, FALSE);
	} 
	if( g_game.observer ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_START].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.start_button, FALSE);
	}

	/* STATUS */
	if( e == PLAYER_STATUS_DESCONECTADO ) {
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_STATUS].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.status_button, FALSE);
	} else {
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_STATUS].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.status_button, TRUE);
	}

	/* ATAQUE */
	if( e == PLAYER_STATUS_ATAQUE ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ATTACK_RE].widget,TRUE);
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ATTACK_RESET].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ATTACK_RE].widget,FALSE);
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ATTACK_RESET].widget,FALSE);
	}

	/* ENVIAR FICHAS */
	if( e==PLAYER_STATUS_FICHAS || e==PLAYER_STATUS_FICHAS2 || e==PLAYER_STATUS_FICHASC ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_FICHAS].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.enviarfichas_button, TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_FICHAS].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.enviarfichas_button, FALSE);
	}

	/* REAGRUPE */
	if( e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_REGROUP].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.reagrupe_button, TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_REGROUP].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.reagrupe_button, FALSE);
	}

	/* GET CARD */
	if( e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_GETCARD].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.getcard_button, TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_GETCARD].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.getcard_button, FALSE);
	}

	/* END TURN */
	if( e==PLAYER_STATUS_ATAQUE || e==PLAYER_STATUS_TROPAS || e==PLAYER_STATUS_REAGRUPE || e==PLAYER_STATUS_TARJETA) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ENDTURN].widget,TRUE);
		gtk_widget_set_sensitive (SensiData.endturn_button, TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_ENDTURN].widget,FALSE);
		gtk_widget_set_sensitive (SensiData.endturn_button, FALSE);
	}

	/* UPDATE MAP */
	if( e >= PLAYER_STATUS_START || (g_game.observer && e >= PLAYER_STATUS_CONNECTED) ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_UPDATEMAP].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_UPDATEMAP].widget,FALSE);
	}

	/* VIEW CARDS, MISSION */
	if( e >= PLAYER_STATUS_START ) {
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_MISSION].widget,TRUE);
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_CARDS].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_MISSION].widget,FALSE);
		gtk_widget_set_sensitive( view_menu_uiinfo[ON_CARDS].widget,FALSE);
	}

	/* LAUNCH ROBOT */
	if( e >= PLAYER_STATUS_CONNECTED && e < PLAYER_STATUS_START ) {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_LAUNCHROBOT].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_LAUNCHROBOT].widget,FALSE);
	}

	/* SURRENDER */
	if( e>=PLAYER_STATUS_START ) {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_SURRENDER].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( actions_menu_uiinfo[ON_SURRENDER].widget,FALSE);
	}

	/* SCORES */
	if( e >= PLAYER_STATUS_CONNECTED ) {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_SCORES].widget,TRUE);
	} else {
		gtk_widget_set_sensitive( game_menu_uiinfo[ON_SCORES].widget,FALSE);
	}


	return TEG_STATUS_SUCCESS;
}


/*
 * tropas
 */
static void tropas_close_cb (GtkWidget *window )
{
//	out_tropas( tropas_src, tropas_dst, 0 );
	gtk_widget_destroy(tropas_dialog);
}


static void tropas_ok_cb (GtkWidget *window )
{
	tropas_cant = (gint) gtk_range_get_adjustment(GTK_RANGE(tropas_hscale_cant))->value;

	if( tropas_cant > 0)
		out_tropas( tropas_src, tropas_dst, tropas_cant );

	gtk_widget_destroy(tropas_dialog);
}

/* shows how many armies you can move after an attack */
void tropas_window( int src, int dst, int cant )
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *vbox_dia,*vbox;
	GtkWidget *hbox;
        GtkAdjustment *adj;
	char buf[200];

	tropas_src = src;
	tropas_dst = dst;

	if( tropas_dialog != NULL ) {
		gdk_window_show( tropas_dialog->window);
		gdk_window_raise( tropas_dialog->window);
		return ;
	}

	tropas_cant = 0;

//	tropas_dialog = gnome_dialog_new(_("A country was conquered"),GNOME_STOCK_BUTTON_OK,NULL);
	tropas_dialog = teg_dialog_new(_("A country was conquered"),_("A country was conquered"));
	gnome_dialog_append_button( GNOME_DIALOG(tropas_dialog),GNOME_STOCK_BUTTON_OK);

	gnome_dialog_set_parent (GNOME_DIALOG (tropas_dialog),
			GTK_WINDOW (main_window));

	gtk_signal_connect (GTK_OBJECT (tropas_dialog), "destroy",
			GTK_SIGNAL_FUNC (gtk_widget_destroyed), &tropas_dialog);

	if( cant > 0 )
		gnome_dialog_button_connect(GNOME_DIALOG(tropas_dialog), 0, GTK_SIGNAL_FUNC(tropas_ok_cb), NULL);
	else
		gnome_dialog_button_connect(GNOME_DIALOG(tropas_dialog), 0, GTK_SIGNAL_FUNC(tropas_close_cb), NULL);

	vbox_dia = GNOME_DIALOG(tropas_dialog)->vbox;

	table = gtk_table_new (2, 2, FALSE);
	gtk_container_border_width (GTK_CONTAINER (table), GNOME_PAD);
	gtk_table_set_row_spacings (GTK_TABLE (table), GNOME_PAD);
	gtk_table_set_col_spacings (GTK_TABLE (table), GNOME_PAD);

	gtk_box_pack_start( GTK_BOX( vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new (_("A new country"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_vbox_new (TRUE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	snprintf(buf,sizeof(buf)-1,_("You conquered '%s' in your attack from '%s'!!\n"), countries_get_name(dst), countries_get_name(src) );
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start( GTK_BOX( vbox), label, TRUE, TRUE, 0);

	if( cant >  0) {
		hbox = gtk_hbox_new( FALSE, 0);
		gtk_container_add( GTK_CONTAINER( vbox ), hbox );
		label = gtk_label_new(_("Armies to move"));
		gtk_box_pack_start( GTK_BOX( hbox), label, TRUE, TRUE, 0);
		adj = (GtkAdjustment *) gtk_adjustment_new( 0.0, 0.0, (gfloat) (cant + 1), 1.0, 1.0, 1.0 );
		// tropas_spinner_cant = gtk_spin_button_new( adj, 0.0, 0);
		tropas_hscale_cant = gtk_hscale_new( adj);
		gtk_box_pack_start( GTK_BOX( hbox), tropas_hscale_cant, TRUE, TRUE, 0);
		gtk_scale_set_digits(GTK_SCALE(tropas_hscale_cant), 0);
		gtk_scale_set_draw_value(GTK_SCALE(tropas_hscale_cant),1);
/*		gtk_range_draw_trough(GTK_RANGE(tropas_hscale_cant));  */

		snprintf(buf, sizeof(buf) -1, _("[Note: The server moved 1 army to '%s' for you.\nChoose how many more armies you want to move]"), countries_get_name(dst) );
		buf[ sizeof(buf) -1 ] = 0;
		label = gtk_label_new(buf);
		gtk_box_pack_start( GTK_BOX( vbox), label, TRUE, TRUE, 0);
	}

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_table_attach (GTK_TABLE (table), frame, 0, 1, 0, 1, GTK_EXPAND |
			GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);


	if (!GTK_WIDGET_VISIBLE (tropas_dialog))
		gtk_widget_show_all (tropas_dialog);
	else
		gtk_widget_destroy (tropas_dialog);
}

/* regroup */
static void reagrupe_ok_cb (GtkWidget *window )
{
	reagrupe_cant = (gint) gtk_range_get_adjustment(GTK_RANGE(reagrupe_hscale_cant))->value;

	if( reagrupe_cant > 0) {
		reagrupe_out( reagrupe_src, reagrupe_dst, reagrupe_cant );
	} else
		reagrupe_reset();

	gtk_widget_destroy(reagrupe_dialog);
}

/* window that shows armies to move in a regroup */
void reagrupe_window( int src, int dst, int cant )
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *frame;
	GtkWidget *vbox_dia,*vbox;
	GtkWidget *hbox;
        GtkAdjustment *adj;
	char buf[200];

	reagrupe_src = src;
	reagrupe_dst = dst;

	if( reagrupe_dialog != NULL ) {
		gdk_window_show( reagrupe_dialog->window);
		gdk_window_raise( reagrupe_dialog->window);
		return ;
	}

	reagrupe_cant = 0;

//	reagrupe_dialog = gnome_dialog_new(_("Regrouping armies"),GNOME_STOCK_BUTTON_OK,NULL);
	reagrupe_dialog = teg_dialog_new(_("Regrouping armies"),_("Regroup your armies"));

	gnome_dialog_append_button(GNOME_DIALOG(reagrupe_dialog), GNOME_STOCK_BUTTON_OK);

	gnome_dialog_set_parent (GNOME_DIALOG (reagrupe_dialog),
			GTK_WINDOW (main_window));

	gtk_signal_connect (GTK_OBJECT (reagrupe_dialog), "destroy",
			GTK_SIGNAL_FUNC (gtk_widget_destroyed), &reagrupe_dialog);

	gnome_dialog_button_connect(GNOME_DIALOG(reagrupe_dialog), 0, GTK_SIGNAL_FUNC(reagrupe_ok_cb), NULL);

	vbox_dia = GNOME_DIALOG(reagrupe_dialog)->vbox;

	table = gtk_table_new (2, 2, FALSE);
	gtk_container_border_width (GTK_CONTAINER (table), GNOME_PAD);
	gtk_table_set_row_spacings (GTK_TABLE (table), GNOME_PAD);
	gtk_table_set_col_spacings (GTK_TABLE (table), GNOME_PAD);

	gtk_box_pack_start( GTK_BOX( vbox_dia), table, TRUE, TRUE, 0);

	frame = gtk_frame_new (_("Regrouping"));
	gtk_container_border_width (GTK_CONTAINER (frame), 0);

	vbox = gtk_vbox_new (TRUE, 0);
	gtk_container_border_width (GTK_CONTAINER (vbox), GNOME_PAD);

	snprintf(buf, sizeof(buf) -1, _("Moving armies from '%s' to '%s'\n"), countries_get_name(src), countries_get_name(dst) );
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start( GTK_BOX( vbox), label, TRUE, TRUE, 0);

	hbox = gtk_hbox_new( FALSE, 0);
	gtk_container_add( GTK_CONTAINER( vbox ), hbox );
	label = gtk_label_new(_("Armies to move"));
	gtk_box_pack_start( GTK_BOX( hbox), label, TRUE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new( 0.0, 0.0, (gfloat) (cant + 1), 1.0, 1.0, 1.0 );

	reagrupe_hscale_cant = gtk_hscale_new( adj);

	gtk_box_pack_start( GTK_BOX( hbox), reagrupe_hscale_cant, TRUE, TRUE, 0);
	gtk_scale_set_digits(GTK_SCALE(reagrupe_hscale_cant), 0);
	gtk_scale_set_draw_value(GTK_SCALE(reagrupe_hscale_cant),1);
/*	gtk_range_draw_trough(GTK_RANGE(reagrupe_hscale_cant));  */

	snprintf(buf, sizeof(buf) -1,_("[Note: You can regroup as many times as you want,\nas long as you dont regroup an armie that were\nregrouped before in this turn.]"));
	buf[ sizeof(buf) -1 ] = 0;
	label = gtk_label_new(buf);
	gtk_box_pack_start( GTK_BOX( vbox), label, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (frame), vbox);

	gtk_table_attach (GTK_TABLE (table), frame, 0, 1, 0, 1, GTK_EXPAND |
			GTK_FILL, GTK_EXPAND | GTK_FILL, 0, 0);


	if (!GTK_WIDGET_VISIBLE (reagrupe_dialog))
		gtk_widget_show_all (reagrupe_dialog);
	else
		gtk_widget_destroy (reagrupe_dialog);
}
