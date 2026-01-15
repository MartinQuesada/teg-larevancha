# Migración a GTK4 y Python3

Este documento describe los cambios realizados para migrar T.E.G. (Tenes Empanadas Graciela) de GTK2 a GTK4 y de Python2 a Python3.

## Cambios Principales

### GTK2 → GTK4

1. **Librerías actualizadas:**
   - `libgnomeui-2.0` → `gtk4`
   - `gconf` → `gsettings`

2. **Funciones obsoletas reemplazadas:**
   - `gtk_widget_set_usize()` → `gtk_widget_set_size_request()`
   - `gtk_signal_connect()` → `g_signal_connect()`
   - `gnome_program_init()` → `gtk_init()`

3. **Configuración:**
   - GConf → GSettings
   - Nuevo esquema XML: `org.gnome.teg.gschema.xml`

### Python2 → Python3

1. **Sintaxis actualizada:**
   - `class ClassName( object ):` → `class ClassName(object):`
   - `def method( self ):` → `def method(self):`

2. **Compatibilidad:**
   - Todos los archivos Python actualizados para Python3
   - Makefiles configurados para Python3

## Archivos Modificados

### Configuración
- `configure.in` - Agregado soporte para GTK4 y Python3
- `Makefile.am` - Incluido directorio python
- `python/Makefile.am` - Nuevo Makefile para Python
- `python/client/Makefile.am` - Makefile para subdirectorio client
- `python/client/gui/Makefile.am` - Makefile para subdirectorio gui

### Código GTK
- `client/gui-gnome/gui.c` - Migrado a GTK4
- `client/gui-gnome/connect.c` - Actualizado para GSettings
- `client/gui-gnome/preferences.c` - Actualizado para GSettings
- `client/gui-gnome/priv.c` - Actualizado para GSettings
- `client/gui-gnome/support.c` - Funciones obsoletas actualizadas
- `client/gui-gnome/mission.c` - Funciones obsoletas actualizadas
- `client/gui-gnome/status.c` - Funciones obsoletas actualizadas
- `client/gui-gnome/interface.c` - Funciones obsoletas actualizadas
- `client/gui-gnome/cards.c` - Funciones obsoletas actualizadas
- `client/gui-gnome/connect.c` - Funciones obsoletas actualizadas

### Código Python
- `python/client/tegclient.py` - Sintaxis Python3
- `python/client/gui.py` - Sintaxis Python3
- `python/client/ttoolkit.py` - Sintaxis Python3

### Esquemas
- `client/gui-gnome/org.gnome.teg.gschema.xml` - Nuevo esquema GSettings

## Compilación

Para compilar el proyecto migrado:

```bash
./autogen.sh
./configure
make
```

## Dependencias

### GTK4
- gtk4-dev
- glib2-dev
- gsettings-desktop-schemas

### Python3
- python3-dev
- python3-pygame

## Notas Importantes

1. **GNOME Canvas:** El código aún usa GNOME Canvas que no está disponible en GTK4. Se necesitará migrar a Cairo o GtkDrawingArea.

2. **Funciones pendientes:** Algunas funciones de GTK2 aún necesitan ser migradas completamente.

3. **Pruebas:** Se recomienda probar exhaustivamente después de la migración.

## Estado de la Migración

- ✅ Configuración básica (configure.in, Makefiles)
- ✅ Migración de librerías principales
- ✅ Actualización de funciones obsoletas básicas
- ✅ Migración de configuración (GConf → GSettings)
- ✅ Actualización de sintaxis Python
- ⚠️ GNOME Canvas (requiere migración adicional)
- ⚠️ Algunas funciones GTK2 pendientes 