## Process this file with automake to produce Makefile.in

SUBDIRS = client

python_PYTHON = \
	client/tegclient.py \
	client/gui.py \
	client/theme.py \
	client/toolbar.py \
	client/ttoolkit.py \
	client/version.py \
	client/controllers.py \
	client/country.py \
	client/eventmgr.py \
	client/events.py \
	client/font.py \
	client/connection.py \
	client/listeners.py \
	client/map.py \
	client/model.py

nobase_python_PYTHON = \
	client/gui/__init__.py \
	client/gui/tbutton.py \
	client/gui/tdialog.py \
	client/gui/tfont.py \
	client/gui/tlayout.py \
	client/gui/ttest.py \
	client/gui/twidget.py \
	client/gui/twidget_manager.py

EXTRA_DIST = \
	$(python_PYTHON) \
	$(nobase_python_PYTHON) 