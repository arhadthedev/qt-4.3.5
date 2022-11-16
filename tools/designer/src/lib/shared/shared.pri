
INCLUDEPATH += $$PWD
QT += script

# Input
FORMS += $$PWD/findicondialog.ui \
    $$PWD/orderdialog.ui \
    $$PWD/newactiondialog.ui \
    $$PWD/resourceeditor.ui \
    $$PWD/gridpanel.ui

HEADERS += \
    $$PWD/shared_global_p.h \
    $$PWD/spacer_widget_p.h \
    $$PWD/layoutinfo_p.h \
    $$PWD/layout_p.h \
    $$PWD/connectionedit_p.h \
    $$PWD/pluginmanager_p.h \
    $$PWD/metadatabase_p.h \
    $$PWD/qdesigner_formeditorcommand_p.h \
    $$PWD/qdesigner_formwindowcommand_p.h \
    $$PWD/qdesigner_command_p.h \
    $$PWD/qdesigner_propertycommand_p.h \
    $$PWD/qdesigner_propertycommentcommand_p.h \
    $$PWD/qdesigner_formbuilder_p.h \
    $$PWD/qdesigner_taskmenu_p.h \
    $$PWD/qdesigner_widget_p.h \
    $$PWD/qdesigner_propertysheet_p.h \
    $$PWD/qdesigner_membersheet_p.h \
    $$PWD/qdesigner_propertyeditor_p.h \
    $$PWD/qdesigner_objectinspector_p.h \
    $$PWD/qdesigner_integration_p.h \
    $$PWD/invisible_widget_p.h \
    $$PWD/qlayout_widget_p.h \
    $$PWD/tree_widget_p.h \
    $$PWD/sheet_delegate_p.h \
    $$PWD/qdesigner_stackedbox_p.h \
    $$PWD/qdesigner_tabwidget_p.h \
    $$PWD/qdesigner_dockwidget_p.h \
    $$PWD/qdesigner_toolbox_p.h \
    $$PWD/qdesigner_dnditem_p.h \
    $$PWD/qsimpleresource_p.h \
    $$PWD/widgetfactory_p.h \
    $$PWD/widgetdatabase_p.h \
    $$PWD/qdesigner_promotion_p.h \
    $$PWD/promotionmodel_p.h \
    $$PWD/qdesigner_promotiondialog_p.h \
    $$PWD/resourcefile_p.h \
    $$PWD/iconloader_p.h \
    $$PWD/findicondialog_p.h \
    $$PWD/richtexteditor_p.h \
    $$PWD/actioneditor_p.h \
    $$PWD/actionrepository_p.h \
    $$PWD/qdesigner_toolbar_p.h \
    $$PWD/qdesigner_menubar_p.h \
    $$PWD/qdesigner_menu_p.h \
    $$PWD/actionprovider_p.h \
    $$PWD/orderdialog_p.h \
    $$PWD/newactiondialog_p.h \
    $$PWD/resourceeditor_p.h \
    $$PWD/stylesheeteditor_p.h \
    $$PWD/csshighlighter_p.h \
    $$PWD/shared_enums_p.h \
    $$PWD/textpropertyeditor_p.h \
    $$PWD/propertylineedit_p.h \
    $$PWD/promotiontaskmenu_p.h \
    $$PWD/scripterrordialog_p.h \
    $$PWD/scriptcommand_p.h \
    $$PWD/scriptdialog_p.h \
    $$PWD/qscripthighlighter_p.h \
    $$PWD/gridpanel_p.h \
    $$PWD/grid_p.h \
    $$PWD/formwindowbase_p.h \
    $$PWD/qdesigner_utils_p.h \
    $$PWD/resourcemimedata_p.h \
    $$PWD/qdesigner_widgetbox_p.h 

SOURCES += \
    $$PWD/spacer_widget.cpp \
    $$PWD/layoutinfo.cpp \
    $$PWD/layout.cpp \
    $$PWD/connectionedit.cpp \
    $$PWD/pluginmanager.cpp \
    $$PWD/qdesigner_formwindowcommand.cpp \
    $$PWD/qdesigner_formeditorcommand.cpp \
    $$PWD/qdesigner_command.cpp \
    $$PWD/qdesigner_propertycommand.cpp \
    $$PWD/qdesigner_propertycommentcommand.cpp \
    $$PWD/qdesigner_formbuilder.cpp \
    $$PWD/qdesigner_taskmenu.cpp \
    $$PWD/qdesigner_widget.cpp \
    $$PWD/qdesigner_dockwidget.cpp \
    $$PWD/qdesigner_propertysheet.cpp \
    $$PWD/qdesigner_membersheet.cpp \
    $$PWD/qdesigner_propertyeditor.cpp \
    $$PWD/qdesigner_objectinspector.cpp \
    $$PWD/qdesigner_integration.cpp \
    $$PWD/qdesigner_dnditem.cpp \
    $$PWD/qsimpleresource.cpp \
    $$PWD/invisible_widget.cpp \
    $$PWD/qlayout_widget.cpp \
    $$PWD/tree_widget.cpp \
    $$PWD/sheet_delegate.cpp \
    $$PWD/metadatabase.cpp \
    $$PWD/qdesigner_stackedbox.cpp \
    $$PWD/qdesigner_tabwidget.cpp \
    $$PWD/qdesigner_toolbox.cpp \
    $$PWD/widgetfactory.cpp \
    $$PWD/widgetdatabase.cpp \
    $$PWD/qdesigner_promotion.cpp \
    $$PWD/promotionmodel.cpp \
    $$PWD/qdesigner_promotiondialog.cpp \
    $$PWD/resourcefile.cpp \
    $$PWD/findicondialog.cpp \
    $$PWD/richtexteditor.cpp \
    $$PWD/actioneditor.cpp \
    $$PWD/actionrepository.cpp \
    $$PWD/qdesigner_toolbar.cpp \
    $$PWD/qdesigner_menubar.cpp \
    $$PWD/qdesigner_menu.cpp \
    $$PWD/orderdialog.cpp \
    $$PWD/newactiondialog.cpp \
    $$PWD/resourceeditor.cpp \
    $$PWD/stylesheeteditor.cpp \
    $$PWD/csshighlighter.cpp \
    $$PWD/textpropertyeditor.cpp \
    $$PWD/propertylineedit.cpp \
    $$PWD/promotiontaskmenu.cpp \
    $$PWD/scripterrordialog.cpp \
    $$PWD/scriptcommand.cpp \
    $$PWD/scriptdialog.cpp \
    $$PWD/qscripthighlighter.cpp\
    $$PWD/gridpanel.cpp \
    $$PWD/grid.cpp \
    $$PWD/formwindowbase.cpp \
    $$PWD/qdesigner_utils.cpp \
    $$PWD/resourcemimedata.cpp \
    $$PWD/qdesigner_widgetbox.cpp \
    $$PWD/iconloader.cpp