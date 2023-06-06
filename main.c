#include <stdio.h>
#include <stdlib.h>
#include "gui.c"

int main()
{
 // Initialize GTK+
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Nitzan's browser");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a horizontal box container for URL input field and Go button
    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

    // Create URL input field
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Enter URL");
    gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, TRUE, 0);

    // Create Go button
    GtkWidget *button = gtk_button_new_with_label("Go");
    gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 5);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(goButtonClicked), entry);

    // Show all the widgets
    gtk_widget_show_all(window);

       // Create a JavaScript runtime context
    JS_Init();
    gContext = JS_NewContext(JS_GetGlobalObject(JS_NewRuntime(8 * 1024 * 1024)));

    // Set the error reporter callback
    JS_SetErrorReporter(gContext, reportError);

    // Register a JavaScript callback function
    JS::RootedObject globalObj(gContext, JS::CurrentGlobalOrNull(gContext));
    JS_DefineFunction(gContext, globalObj, "callback", jsCallback, 0, 0);

    // Execute the JavaScript code
    executeJavaScript(jsCode);

    // Cleanup the JavaScript runtime context
    JS_DestroyContext(gContext);
    JS_ShutDown();

    // Start the GTK+ main loop
    gtk_main();
}
