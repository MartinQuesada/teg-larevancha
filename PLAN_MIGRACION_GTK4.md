# Plan de Migración Completa a GTK4

## Estado Actual

El proyecto está en GTK3 (`gtk+-3.0`) con código parcialmente migrado desde GNOME2/GTK2.

**Archivos con código funcional:** ~1,500 líneas
**Archivos con stubs/vacíos:** ~4,800 líneas (en `.backup`)
**Total a migrar:** ~6,300 líneas

---

## Fase 1: Configuración del Build System

### 1.1 Actualizar `configure.in`

```diff
- PKG_CHECK_MODULES(TEG_LIBGNOME, gtk+-3.0)
+ PKG_CHECK_MODULES(TEG_LIBGNOME, gtk4)
```

### 1.2 Dependencias necesarias

```bash
# macOS (Homebrew)
brew install gtk4 libadwaita

# Debian/Ubuntu
apt install libgtk-4-dev libadwaita-1-dev
```

---

## Fase 2: Cambios de API GTK3 → GTK4

### 2.1 Widgets eliminados/renombrados

| GTK3 | GTK4 | Acción |
|------|------|--------|
| `GtkContainer` | Eliminado | Usar métodos específicos del widget |
| `gtk_container_add()` | `gtk_window_set_child()`, `gtk_box_append()` | Cambiar según el contenedor |
| `gtk_box_pack_start()` | `gtk_box_append()` / `gtk_box_prepend()` | Los parámetros expand/fill se manejan diferente |
| `GtkHBox/GtkVBox` | `GtkBox` con orientación | Ya migrado en GTK3 |
| `GtkTable` | `GtkGrid` | Ya migrado en GTK3 |

### 2.2 Sistema de eventos

| GTK3 | GTK4 |
|------|------|
| Señales de eventos en widgets | `GtkEventController` |
| `button-press-event` | `GtkGestureClick` |
| `motion-notify-event` | `GtkEventControllerMotion` |
| `key-press-event` | `GtkEventControllerKey` |
| `enter-notify-event` | `GtkEventControllerMotion` con `enter`/`leave` |

### 2.3 Dibujo

| GTK3 | GTK4 |
|------|------|
| Señal `"draw"` con callback | `gtk_drawing_area_set_draw_func()` |
| `gboolean callback(widget, cairo_t*, data)` | `void callback(area, cairo_t*, width, height, data)` |

### 2.4 Visibilidad de widgets

| GTK3 | GTK4 |
|------|------|
| `gtk_widget_show()` / `gtk_widget_hide()` | `gtk_widget_set_visible(widget, TRUE/FALSE)` |
| `gtk_widget_show_all()` | Eliminado - widgets visibles por defecto |

### 2.5 Diálogos

| GTK3 | GTK4 |
|------|------|
| `gtk_dialog_run()` | Eliminado - usar señales async |
| `gtk_dialog_get_content_area()` | `gtk_dialog_get_content_area()` (igual) |
| Modal síncrono | Modal con callbacks |

---

## Fase 3: Archivos a Migrar (por prioridad)

### Prioridad 1: Core de la UI (crítico)

| Archivo | Líneas | Complejidad | Descripción |
|---------|--------|-------------|-------------|
| `interface.c` | ~950 | Alta | Ventana principal, menús, toolbar |
| `g_country.c` | ~580 | Alta | Mapa de países (usa GnomeCanvas) |
| `g_cont.c` | ~200 | Media | Continentes |

### Prioridad 2: Componentes del juego

| Archivo | Líneas | Complejidad | Descripción |
|---------|--------|-------------|-------------|
| `cards.c` | ~490 | Media | Sistema de tarjetas |
| `armies.c` | ~350 | Media | Gestión de ejércitos |
| `dices.c` | ~430 | Media | ✅ Ya migrado a Cairo |
| `status.c` | ~750 | Media | Barra de estado y diálogos |
| `colors.c` | ~200 | Baja | Gestión de colores |

### Prioridad 3: Diálogos y utilidades

| Archivo | Líneas | Complejidad | Descripción |
|---------|--------|-------------|-------------|
| `connect.c` | ~400 | Media | Diálogo de conexión |
| `preferences.c` | ~300 | Media | Preferencias |
| `mission.c` | ~200 | Baja | Mostrar misión |
| `g_scores.c` | ~250 | Media | Puntuaciones |
| `locate_country.c` | ~150 | Baja | Localización de países |

### Prioridad 4: Soporte

| Archivo | Líneas | Complejidad | Descripción |
|---------|--------|-------------|-------------|
| `support.c` | ~200 | Baja | Funciones auxiliares |
| `stock.c` | ~150 | Baja | Iconos (ya no necesario en GTK4) |
| `chatline.c` | ~100 | Baja | Chat |

---

## Fase 4: El Problema del GnomeCanvas

El mayor desafío es **GnomeCanvas**, usado para:
- Mapa interactivo de países
- Ejércitos sobre países
- Dados (✅ ya migrado)
- Tarjetas

### Opciones de reemplazo:

#### Opción A: GtkDrawingArea + Cairo (Recomendada)
```c
// Similar a lo hecho en dices.c
GtkWidget *drawing_area = gtk_drawing_area_new();
gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawing_area),
                               map_draw_function, NULL, NULL);

// Para eventos de mouse:
GtkGesture *click = gtk_gesture_click_new();
g_signal_connect(click, "pressed", G_CALLBACK(on_country_click), NULL);
gtk_widget_add_controller(drawing_area, GTK_EVENT_CONTROLLER(click));

GtkEventController *motion = gtk_event_controller_motion_new();
g_signal_connect(motion, "enter", G_CALLBACK(on_country_enter), NULL);
g_signal_connect(motion, "leave", G_CALLBACK(on_country_leave), NULL);
gtk_widget_add_controller(drawing_area, motion);
```

#### Opción B: GtkPicture para imágenes estáticas
Solo para elementos que no necesitan interacción.

#### Opción C: Librería externa (ej: librsvg para SVG)
Si se quiere modernizar los gráficos a SVG.

---

## Fase 5: Plan de Implementación Detallado

### Semana 1-2: Preparación
- [ ] Actualizar `configure.in` para GTK4
- [ ] Crear `gtk4-compat.h` con macros de transición
- [ ] Migrar `support.c` y `stock.c`

### Semana 3-4: Interface principal
- [ ] Migrar `interface.c`:
  - Reemplazar GnomeUIInfo con GMenu/GAction
  - Crear ventana principal con GtkApplicationWindow
  - Migrar toolbar a GtkHeaderBar o GtkBox

### Semana 5-6: Mapa del juego
- [ ] Migrar `g_country.c`:
  - Crear sistema de dibujo con Cairo
  - Implementar hit-testing para clicks en países
  - Implementar hover para destacar países
- [ ] Migrar `g_cont.c`

### Semana 7-8: Componentes del juego
- [ ] Migrar `cards.c`
- [ ] Migrar `armies.c`
- [ ] Migrar `status.c`

### Semana 9-10: Diálogos
- [ ] Migrar `connect.c`
- [ ] Migrar `preferences.c`
- [ ] Migrar `mission.c`
- [ ] Migrar `g_scores.c`

### Semana 11-12: Pulido
- [ ] Testing completo
- [ ] Ajustes de UI/UX
- [ ] Documentación

---

## Fase 6: Código de Ejemplo

### 6.1 Crear ventana principal GTK4

```c
static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "TEG - Tenes Empanadas Graciela");
    gtk_window_set_default_size(GTK_WINDOW(window), 1024, 768);

    // Layout principal
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), vbox);

    // Crear el mapa
    GtkWidget *map = create_map_widget();
    gtk_widget_set_vexpand(map, TRUE);
    gtk_widget_set_hexpand(map, TRUE);
    gtk_box_append(GTK_BOX(vbox), map);

    // Crear barra de estado
    GtkWidget *statusbar = gtk_label_new("Ready");
    gtk_box_append(GTK_BOX(vbox), statusbar);

    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("org.gnome.teg",
                                               G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
```

### 6.2 Mapa con eventos de mouse GTK4

```c
// Estructura para hit-testing de países
typedef struct {
    int country_id;
    GdkPixbuf *image;
    int x, y;
    // Podría incluir un path de Cairo para hit-testing preciso
} CountryRegion;

static CountryRegion countries[COUNTRIES_CANT];
static int hovered_country = -1;

static void map_draw_func(GtkDrawingArea *area, cairo_t *cr,
                          int width, int height, gpointer data) {
    // Dibujar fondo del mapa
    gdk_cairo_set_source_pixbuf(cr, map_background, 0, 0);
    cairo_paint(cr);

    // Dibujar cada país
    for (int i = 0; i < COUNTRIES_CANT; i++) {
        gdk_cairo_set_source_pixbuf(cr, countries[i].image,
                                    countries[i].x, countries[i].y);
        cairo_paint(cr);

        // Si está destacado (hover)
        if (i == hovered_country) {
            cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 0.3);
            // Dibujar highlight...
        }

        // Dibujar número de ejércitos
        draw_army_count(cr, i);
    }
}

static void on_motion(GtkEventControllerMotion *controller,
                      double x, double y, gpointer data) {
    int new_hover = find_country_at(x, y);
    if (new_hover != hovered_country) {
        hovered_country = new_hover;
        gtk_widget_queue_draw(GTK_WIDGET(data));
    }
}

static void on_click(GtkGestureClick *gesture, int n_press,
                     double x, double y, gpointer data) {
    int country = find_country_at(x, y);
    if (country >= 0) {
        handle_country_click(country);
    }
}
```

---

## Recursos

- [GTK4 Migration Guide](https://docs.gtk.org/gtk4/migrating-3to4.html)
- [GTK4 API Reference](https://docs.gtk.org/gtk4/)
- [GLib/GIO Reference](https://docs.gtk.org/glib/)

---

## Decisiones Pendientes

1. **¿Usar Libadwaita?** - Proporciona widgets modernos de GNOME pero agrega dependencia
2. **¿Mantener compatibilidad GTK3?** - Complicaría el código
3. **¿Migrar gráficos a SVG?** - Mejor escalado pero más trabajo
4. **¿Refactorizar arquitectura?** - Separar lógica de vista

---

## Estimación de Esfuerzo

| Fase | Esfuerzo | Riesgo |
|------|----------|--------|
| Build system | 1 día | Bajo |
| Componentes simples | 3-5 días | Bajo |
| Interface principal | 5-7 días | Medio |
| Sistema de mapa | 10-15 días | Alto |
| Diálogos | 5-7 días | Medio |
| Testing | 5-7 días | Medio |

**Total estimado: 30-45 días de trabajo**

---

## Próximos Pasos Inmediatos

1. Decidir si continuar con la migración completa
2. Configurar entorno de desarrollo con GTK4
3. Empezar con `interface.c` ya que es el core de la UI
4. Crear el sistema de mapa con Cairo (reemplazando GnomeCanvas)
