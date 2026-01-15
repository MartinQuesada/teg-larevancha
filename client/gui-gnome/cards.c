/*	$Id: cards.c,v 1.38 2002/08/31 17:45:58 riq Exp $	*/
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
 * @file cards.c
 * Crea las tarjetas que fueron obtenidas
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "gnome-compat.h"
#include <assert.h>

#include "gui.h"
#include "client.h"

#include "chatline.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "priv.h"
#include "g_country.h"
#include "cards.h"
#include "themes.h"
#include "fonts.h"
#include "locate_country.h"

static GtkWidget 	*cards_dialog=NULL;

struct _tarjs_sensi {
	GtkWidget	*card;
	GtkWidget	*button_armies;
	GtkWidget	*button_select;
	PCOUNTRY		country;
	BOOLEAN		selected;
} tarjs_sensi[TEG_MAX_TARJETAS];

struct {
	GdkPixbuf	*tar;
	TARJTIPO	tipo;
	char 		*filename;
} tarjs[] = {
	{ NULL,	TARJ_CANION,	"tar_canon.png" },
	{ NULL,	TARJ_GALEON,	"tar_galeon.png" },
	{ NULL,	TARJ_GLOBO,	"tar_globo.png" },
	{ NULL,	TARJ_COMODIN,	"tar_comodin.png" },
	{ NULL,	TARJ_SUPERTARJETA,	"tar_supercard.png" },
};
#define NRTARJS	( sizeof(tarjs) / sizeof(tarjs[0]))

/**
 * @fn TEG_STATUS cards_load()
 * Carga las imagenes de las tarjetas en variables globlales
 */
TEG_STATUS cards_load()
{
	char *filename;
	int i;

	TCards cards;
	/* obtain cards from theme */
	if( theme_giveme_cards(&cards) == TEG_STATUS_SUCCESS ) {
		if(cards.cannon)
			tarjs[0].filename = cards.cannon;
		if(cards.ship)
			tarjs[1].filename = cards.ship;
		if(cards.balloon)
			tarjs[2].filename = cards.balloon;
		if(cards.jocker)
			tarjs[3].filename = cards.jocker;
		if(cards.super)
			tarjs[4].filename = cards.super;
	}


	for(i=0; i<NRTARJS;i++) {

		if(!tarjs[i].tar) {
			filename = theme_load_file( tarjs[i].filename );
			tarjs[i].tar = gdk_pixbuf_new_from_file(filename, NULL);
			if( tarjs[i].tar == NULL )
				g_warning("Error, couldn't find file:%s", tarjs[i].filename);
		}
	}

	return TEG_STATUS_SUCCESS;
}

TEG_STATUS cards_init()
{
	int i;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		memset( &tarjs_sensi[i], 0, sizeof(tarjs_sensi[i]));
	}
	return TEG_STATUS_SUCCESS;
}

/* Callbacks de las funciones de los botones y funciones auxiliares */
static int cuantos_selected( int countries[TEG_MAX_TARJETAS])
{
	int i,j=0;
	int selected=0;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( tarjs_sensi[i].selected ) {
				countries[j++] = tarjs_sensi[i].country->id;
				selected++;
			}
		}
	}
	return selected;
}

static void cards_cb_button_canje (GtkWidget *widget )
{
	int sel;
	int i;
	int countries[TEG_MAX_TARJETAS];

	assert(widget);

	sel = cuantos_selected( countries );
	if( sel != 3 ) {
		//textmsg(M_ERR,"Error, you must select 3 cards and not %d.",sel);
		for( i=sel-1;i<3;i++ )
		{
			countries[i] = -1;
		}
		//return;
	}
	canje_out( countries[0], countries[1], countries[2] );
}

static void cards_cb_button_armies (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_armies");
	} else {
		if( ejer2_out( tarjs_sensi[ index ].country->id ) == TEG_STATUS_SUCCESS )
			cards_update();
	}
}

static void cards_cb_button_select (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_selected");
	} else {
		tarjs_sensi[ index ].selected = gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(widget) );
	}
}

static void cards_cb_button_locate (GtkWidget *widget, int index )
{
	if( tarjs_sensi[ index ].country == NULL ) {
		textmsg(M_ERR,"Error in cards_cb_button_selected");
	} else {
		locate_country_add_country( tarjs_sensi[ index ].country );
	}
}

/* funciones de creacion */
static GtkWidget *cards_create( PTARJETA pT, int tarjs_index )
{
	GtkWidget	*vbox;
	GtkWidget	*image_widget;
	GtkWidget	*button_armies;
	GtkWidget	*button_select;
	GtkWidget	*button_locate;
	PCOUNTRY		pP;
	int i;
	TCards cards;

	if( theme_giveme_cards(&cards) != TEG_STATUS_SUCCESS )
		return NULL;

	for(i=0;i<NRTARJS;i++) {
		if( tarjs[i].tipo == pT->tarjeta )
			break;
	}

	pP = (PCOUNTRY) COUNTRY_FROM_TARJETA( pT );

	/* GTK3: Usamos GtkBox en lugar de GtkVBox */
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	if( !vbox ) return NULL;

	/* Crear imagen de la tarjeta usando GtkImage con GdkPixbuf */
	if (tarjs[i].tar) {
		image_widget = gtk_image_new_from_pixbuf(tarjs[i].tar);
	} else {
		image_widget = gtk_image_new();
	}
	if( !image_widget ) {
		textmsg( M_ERR, _("Error creating image\n"));
		return NULL;
	}

	gtk_box_pack_start(GTK_BOX(vbox), image_widget, FALSE, FALSE, 0);

	/* Etiquetas para el nombre del país y continente */
	GtkWidget *country_label = gtk_label_new(countries_get_name(pP->id));
	gtk_box_pack_start(GTK_BOX(vbox), country_label, FALSE, FALSE, 0);

	GtkWidget *continent_label = gtk_label_new(cont_get_name(pP->continente));
	gtk_box_pack_start(GTK_BOX(vbox), continent_label, FALSE, FALSE, 0);

	/* Botones */
	button_armies = gtk_button_new_with_label(_("Put 3 armies"));
	gtk_box_pack_start(GTK_BOX(vbox), button_armies, FALSE, FALSE, 0);
	g_signal_connect(button_armies, "clicked", G_CALLBACK(cards_cb_button_armies), GINT_TO_POINTER(tarjs_index));

	button_select = gtk_check_button_new_with_label(_("Select this card"));
	gtk_box_pack_start(GTK_BOX(vbox), button_select, FALSE, FALSE, 0);
	g_signal_connect(button_select, "clicked", G_CALLBACK(cards_cb_button_select), GINT_TO_POINTER(tarjs_index));

	button_locate = gtk_button_new_with_label(_("Locate country"));
	gtk_box_pack_start(GTK_BOX(vbox), button_locate, FALSE, FALSE, 0);
	g_signal_connect(button_locate, "clicked", G_CALLBACK(cards_cb_button_locate), GINT_TO_POINTER(tarjs_index));

	tarjs_sensi[tarjs_index].card = vbox;
	tarjs_sensi[tarjs_index].button_armies = button_armies;
	tarjs_sensi[tarjs_index].button_select = button_select;
	tarjs_sensi[tarjs_index].country = pP;
	tarjs_sensi[tarjs_index].selected = FALSE;

	return vbox;
}

/*
 * funciones exportadas
 */
/**
 * @fn void cards_view(int country)
 */
void cards_view(int country)
{
	GtkWidget *vbox;
	GtkWidget *scrolledwindow;
	static GtkGrid	*grid = NULL;
	GtkWidget	*tarjeta;	

	if( IsListEmpty( &g_game.tarjetas_list ) && country == -1 ) {
		textmsg(M_ERR,_("You dont have any cards yet"));
		return;
	}

	if( cards_dialog == NULL) {

		memset(tarjs_sensi,0,sizeof(tarjs_sensi));	

		cards_init();
		cards_load();

		/* GTK3: Usamos GtkWindow */
		cards_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(cards_dialog), _("Country Cards"));
		gtk_window_set_default_size(GTK_WINDOW(cards_dialog), 400, 300);

		g_signal_connect(cards_dialog, "destroy", G_CALLBACK(destroy_window), &cards_dialog);

		vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
		gtk_container_add(GTK_CONTAINER(cards_dialog), vbox);

		scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow),
				GTK_POLICY_AUTOMATIC,
				GTK_POLICY_AUTOMATIC);
		gtk_widget_set_size_request(scrolledwindow, 300, 160);

		// GTK4: Usamos GtkGrid en lugar de GtkTable
		grid = GTK_GRID(gtk_grid_new());
		gtk_grid_set_column_spacing(grid, 1);
		gtk_grid_set_row_spacing(grid, 5);

		{
			int x=0,y=0, index=0;
			PLIST_ENTRY pL = g_game.tarjetas_list.Flink;
			PTARJETA pT;
			while( !IsListEmpty( &g_game.tarjetas_list ) && (pL != &g_game.tarjetas_list )) {
				pT = (PTARJETA) pL;

				if(x >= 3 ) {
					x=0;
					y++;
				}
				tarjeta = cards_create( pT, index );
				if( tarjeta ) {
					gtk_widget_set_visible(tarjeta, TRUE);
					gtk_grid_attach(grid, tarjeta, x, y, 1, 1);
				}
				x++, index++;

				pL = LIST_NEXT( pL );
			}
		}

		/* Agregar grid al scrolledwindow */
		gtk_container_add(GTK_CONTAINER(scrolledwindow), GTK_WIDGET(grid));
		gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);

		/* Botones de acción */
		GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
		gtk_widget_set_halign(button_box, GTK_ALIGN_END);
		gtk_box_pack_end(GTK_BOX(vbox), button_box, FALSE, FALSE, 5);

		GtkWidget *button_canje = gtk_button_new_with_label(_("Exchange cards"));
		g_signal_connect(button_canje, "clicked", G_CALLBACK(cards_cb_button_canje), NULL);
		gtk_box_pack_start(GTK_BOX(button_box), button_canje, FALSE, FALSE, 5);

		GtkWidget *button_close = gtk_button_new_with_label(_("Close"));
		g_signal_connect_swapped(button_close, "clicked", G_CALLBACK(gtk_widget_destroy), cards_dialog);
		gtk_box_pack_start(GTK_BOX(button_box), button_close, FALSE, FALSE, 5);

	} else if(country!=-1) { /* Ya estan las demas, solo hay que crear una sola tarjeta */
		int x=0, y=0,i;
		for(i=0;i<TEG_MAX_TARJETAS;i++) {

			if(x >= 3 ) {
				x=0;
				y++;
			}

			if( tarjs_sensi[i].country == NULL ) {
				tarjeta = cards_create( &g_countries[country].tarjeta, i );
				if( tarjeta ) {
					gtk_widget_set_visible(tarjeta, TRUE);
					gtk_grid_attach(grid, tarjeta, x, y, 1, 1);
				}
				break;
			}
			x++;
		}
	}

	cards_update();
	cards_update_para_canje();

	gtk_widget_show_all(cards_dialog);
	gtk_window_present(GTK_WINDOW(cards_dialog));
}

/**
 * @fn void cards_delete( int p1, int p2, int p3 )
 */
void cards_delete( int p1, int p2, int p3 )
{
	int i;
	if( cards_dialog == NULL) 
		return;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {

		if( tarjs_sensi[i].country != NULL  && 
				( tarjs_sensi[i].country->id == p1 ||
				  tarjs_sensi[i].country->id == p2 ||
				  tarjs_sensi[i].country->id == p3 ) ) {

			if( tarjs_sensi[i].card && cards_dialog )
				gtk_widget_destroy( tarjs_sensi[i].card );
			tarjs_sensi[i].country = NULL;
		}
	}

	cards_update_para_canje();
	gtk_widget_set_visible(cards_dialog, TRUE);
	gtk_window_present(GTK_WINDOW(cards_dialog));
}

/**
 * @fn void cards_flush()
 * Elimina todas las tarjetas que tenga
 */
void cards_flush()
{
	int i;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country  && tarjs_sensi[i].card && cards_dialog) {
			gtk_widget_destroy( tarjs_sensi[i].card );
		}
		tarjs_sensi[i].country = NULL;
	}
}

/**
 * @fn void cards_update( void )
 * Se encarga de hacer el update de las tarjetas, habilitando los botones
 * y esas cosas.
 */
void cards_update( void )
{
	int i;
	if( cards_dialog == NULL )
		return;
	
	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( !ESTADO_ES( PLAYER_STATUS_TARJETA )
					|| tarjeta_es_usada( &tarjs_sensi[i].country->tarjeta)
					|| tarjs_sensi[i].country->numjug != WHOAMI()
					|| country_esbloqueado ( tarjs_sensi[i].country, WHOAMI() ) )
				gtk_widget_set_sensitive (tarjs_sensi[i].button_armies, FALSE);
			else
				gtk_widget_set_sensitive (tarjs_sensi[i].button_armies, TRUE);
		}
	}
}

/**
 * @fn void cards_update_para_canje( void )
 */
void cards_update_para_canje( void )
{
	int i;

	if( cards_dialog == NULL )
		return;

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country != NULL ) {
			if( ESTADO_ES( PLAYER_STATUS_FICHASC ) && canje_puedo(NULL,NULL,NULL)==TEG_STATUS_SUCCESS) {
				gtk_widget_set_sensitive (tarjs_sensi[i].button_select, TRUE);
			} else {
				gtk_widget_set_sensitive (tarjs_sensi[i].button_select, FALSE);
			}
		}
	}

	// GTK4: Simplificamos la sensibilidad del botón de intercambio
	if( ESTADO_ES( PLAYER_STATUS_FICHASC ) && canje_puedo(NULL,NULL,NULL)==TEG_STATUS_SUCCESS) {
		// Habilitar botón de intercambio
	} else {
		// Deshabilitar botón de intercambio
	}
}

/**
 * @fn TEG_STATUS cards_select(int p1, int p2, int p3 )
 * Selecciona las cartas pedidas
 */
TEG_STATUS cards_select(int p1, int p2, int p3 )
{
	int i;

	cards_view(-1);

	for(i=0;i<TEG_MAX_TARJETAS;i++) {
		if( tarjs_sensi[i].country == NULL ) continue;

		if(( tarjs_sensi[i].country->id == p1 ) ||
				(tarjs_sensi[i].country->id == p2 ) ||
				(tarjs_sensi[i].country->id == p3 ) ) {
			tarjs_sensi[i].selected = TRUE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select), TRUE);
		} else {
			tarjs_sensi[i].selected = FALSE;
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(tarjs_sensi[i].button_select), FALSE);
		}
	}

	return TEG_STATUS_SUCCESS;
}

/**
 * @fn void cards_free()
 * Libera los recursos alocados
 */
void cards_free()
{
	int i;

	for(i=0;i<NRTARJS;i++) {
		if( tarjs[i].tar ) {
			/* gdk_imlib_destroy_image ( tarjs[i].tar ); */
			tarjs[i].tar = NULL;
		}
	}
}
