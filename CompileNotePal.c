/*
 *	NotePal
 *	Created by: Akash Nagaraj
 *	Created on: 13th November, 2016
 */

#include <string.h>
#include <libgen.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#define DEFAULT_HEIGHT 500 /* Initial Height of the text editor window */
#define DEFAULT_WIDTH 900  /* Initial Width of the text editor window */

#define alphabet "abcdefghijk ABCDEFHIJK"

typedef struct
{
   GtkWidget *menu_label;
   GtkWidget *menu;
   GtkWidget *new;
   GtkWidget *open;
   GtkWidget *save;
   GtkWidget *close;
   GtkWidget *separator;
   GtkWidget *quit;
} FileMenu;

typedef struct
{
   GtkWidget *menu_label;
   GtkWidget *menu;
   GtkWidget *cut;
   GtkWidget *copy;
   GtkWidget *paste;
} EditMenu;

typedef struct
{
   GtkWidget *menu_label;
   GtkWidget *menu;
   GtkWidget *font;
} OptionsMenu;

typedef struct
{
   GtkWidget *menu_label;
   GtkWidget *menu;
   GtkWidget *about;
} HelpMenu;

typedef struct
{
   GtkWidget *scrolled_window;
   GtkWidget *textview;
   GtkWidget *tab_label;
} FileObject;

typedef struct
{
   GtkWidget *toplevel;
   GtkWidget *vbox;
   GtkWidget *notebook;
   GtkWidget *menubar;
   GtkWidget *toolbar;
   FileMenu *filemenu;
   EditMenu *editmenu;
   OptionsMenu *options_menu;
   HelpMenu *helpmenu;
} TextEditUI;

typedef struct
{
   gchar *filename;
   gint tab_number;
} FileData;

static GList *filename_data = NULL;       /* Linked list for all open file names */
static PangoFontDescription *desc = NULL; /* Common Font for all tabs */

static void quit_application(GtkWidget *, gpointer);
static void text_edit_init_GUI(TextEditUI *);
static void text_edit_create_menus(TextEditUI *);
static void text_edit_create_toolbar_items(TextEditUI *);
FileObject *text_edit_file_new(void);
static void text_edit_tab_new_with_file(GtkMenuItem *, GtkNotebook *);
static void text_edit_select_font(GtkMenuItem *, gpointer);
static void text_edit_apply_font_selection(GtkNotebook *);
static void text_edit_open_file(GtkMenuItem *, GtkNotebook *);
static void text_edit_cut_to_clipboard(GtkMenuItem *, GtkNotebook *);
static void text_edit_copy_to_clipboard(GtkMenuItem *, GtkNotebook *);
static void text_edit_paste_from_clipboard(GtkMenuItem *, GtkNotebook *);
static void text_edit_show_about_dialog(GtkMenuItem *, GtkWindow *);
static void text_edit_close_file(GtkMenuItem *, GtkNotebook *);
static void text_edit_save_file(GtkMenuItem *, GtkNotebook *);
static void text_edit_register_filename(gchar *fname, gint tab_num);
static gchar *text_edit_get_filename(gint tab_num);

int main(int argc, char *argv[])
{
   TextEditUI app;
   gtk_init(&argc, &argv);
   app.toplevel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   gtk_window_set_title(GTK_WINDOW(app.toplevel), "NotePal");                            /* Name on window */
   gtk_window_set_default_size(GTK_WINDOW(app.toplevel), DEFAULT_WIDTH, DEFAULT_HEIGHT); /* Text editor window size */
   g_signal_connect(G_OBJECT(app.toplevel), "destroy",
                    G_CALLBACK(quit_application), NULL);

   text_edit_init_GUI(&app); /* Build the interface */
   gtk_widget_show_all(app.toplevel);
   gtk_main();
   return 0;
}

static void quit_application(GtkWidget *window, gpointer data)
{ /* Quitinf the application */
   gtk_main_quit();
}

static void text_edit_init_GUI(TextEditUI *app)
{
   desc = pango_font_description_from_string("Progsole normal 12");

   FileObject *file = text_edit_file_new();

   text_edit_register_filename("Untitled File", 0);

   // Change: getk_box_new now requires GtkOrientation
   // GTK_ORIENTATION_VERTICAL/GTK_ORIENTATION_HORIZONZAL
   // instead of a boolean for horizontal and breaks layout otherwise
   app->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
   app->notebook = gtk_notebook_new();
   app->menubar = gtk_menu_bar_new();

   text_edit_create_menus(app);
   text_edit_create_toolbar_items(app);

   gtk_notebook_set_show_tabs(GTK_NOTEBOOK(app->notebook), TRUE);
   gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), file->scrolled_window, file->tab_label);
   gtk_box_pack_start(GTK_BOX(app->vbox), app->notebook, TRUE, TRUE, 0);
   gtk_container_add(GTK_CONTAINER(app->toplevel), app->vbox);
}

static void text_edit_create_menus(TextEditUI *app)
{
   FileMenu *file;
   EditMenu *edit;
   OptionsMenu *options;
   HelpMenu *help;
   GtkAccelGroup *group = gtk_accel_group_new();
   /* File Menu options follow */
   app->filemenu = g_new(FileMenu, 1);
   app->editmenu = g_new(EditMenu, 1);
   app->options_menu = g_new(OptionsMenu, 1);
   app->helpmenu = g_new(HelpMenu, 1);
   file = app->filemenu;
   edit = app->editmenu;
   options = app->options_menu;
   help = app->helpmenu;

   gtk_window_add_accel_group(GTK_WINDOW(app->toplevel), group);

   file->menu_label = gtk_menu_item_new_with_label("File");
   file->menu = gtk_menu_new();

   // Change: Gtk.stock is deprecated since 3-11 and can be replaced
   // with a new widget signal gtk_widget_add_accelerator
   // Change: GTK_'Letter' keyboard event throws compillation error "undeclared",
   // is replaced through GDK_KEY_'letter'

   file->new = gtk_menu_item_new_with_mnemonic("_New");
   gtk_widget_add_accelerator(file->new, "activate", group, GDK_KEY_n, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   file->open = gtk_menu_item_new_with_mnemonic("_Open");
   gtk_widget_add_accelerator(file->open, "activate", group, GDK_KEY_o, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   file->save = gtk_menu_item_new_with_mnemonic("_Save");
   gtk_widget_add_accelerator(file->save, "activate", group, GDK_KEY_s, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   file->close = gtk_menu_item_new_with_mnemonic("_Close");
   gtk_widget_add_accelerator(file->close, "activate", group, GDK_KEY_w, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   file->separator = gtk_separator_menu_item_new();
   file->quit = gtk_menu_item_new_with_mnemonic("_Quit");
   gtk_widget_add_accelerator(file->quit, "activate", group, GDK_KEY_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

   gtk_menu_item_set_submenu(GTK_MENU_ITEM(file->menu_label), file->menu);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->new);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->open);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->save);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->close);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->separator);
   gtk_menu_shell_append(GTK_MENU_SHELL(file->menu), file->quit);

   edit->menu_label = gtk_menu_item_new_with_label("Edit");
   edit->menu = gtk_menu_new();
   edit->cut = gtk_menu_item_new_with_mnemonic("Cu_t");
   gtk_widget_add_accelerator(edit->cut, "activate", group, GDK_KEY_x, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   edit->copy = gtk_menu_item_new_with_mnemonic("_Copy");
   gtk_widget_add_accelerator(edit->copy, "activate", group, GDK_KEY_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
   edit->paste = gtk_menu_item_new_with_mnemonic("_Paste");
   gtk_widget_add_accelerator(edit->paste, "activate", group, GDK_KEY_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

   gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit->menu_label), edit->menu);
   gtk_menu_shell_append(GTK_MENU_SHELL(edit->menu), edit->cut);
   gtk_menu_shell_append(GTK_MENU_SHELL(edit->menu), edit->copy);
   gtk_menu_shell_append(GTK_MENU_SHELL(edit->menu), edit->paste);

   options->menu_label = gtk_menu_item_new_with_label("Options");
   options->menu = gtk_menu_new();
   options->font = gtk_menu_item_new_with_mnemonic("_Select Font");

   gtk_menu_item_set_submenu(GTK_MENU_ITEM(options->menu_label), options->menu);
   gtk_menu_shell_append(GTK_MENU_SHELL(options->menu), options->font);

   help->menu_label = gtk_menu_item_new_with_label("Help");
   help->menu = gtk_menu_new();
   help->about = gtk_menu_item_new_with_mnemonic("_About");

   gtk_menu_item_set_submenu(GTK_MENU_ITEM(help->menu_label), help->menu);
   gtk_menu_shell_append(GTK_MENU_SHELL(help->menu), help->about);

   gtk_menu_shell_append(GTK_MENU_SHELL(app->menubar), file->menu_label);
   gtk_menu_shell_append(GTK_MENU_SHELL(app->menubar), edit->menu_label);
   gtk_menu_shell_append(GTK_MENU_SHELL(app->menubar), options->menu_label);
   gtk_menu_shell_append(GTK_MENU_SHELL(app->menubar), help->menu_label);

   g_signal_connect(G_OBJECT(file->new), "activate", G_CALLBACK(text_edit_tab_new_with_file), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(file->open), "activate", G_CALLBACK(text_edit_open_file), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(file->save), "activate", G_CALLBACK(text_edit_save_file), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(file->close), "activate", G_CALLBACK(text_edit_close_file), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(file->quit), "activate", G_CALLBACK(quit_application), NULL);
   g_signal_connect(G_OBJECT(edit->cut), "activate", G_CALLBACK(text_edit_cut_to_clipboard), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(edit->copy), "activate", G_CALLBACK(text_edit_copy_to_clipboard), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(edit->paste), "activate", G_CALLBACK(text_edit_paste_from_clipboard), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(options->font), "activate", G_CALLBACK(text_edit_select_font), (gpointer)app->notebook);
   g_signal_connect(G_OBJECT(help->about), "activate", G_CALLBACK(text_edit_show_about_dialog), (gpointer)app->toplevel);

   gtk_box_pack_start(GTK_BOX(app->vbox), app->menubar, FALSE, FALSE, 0);
}

static void text_edit_create_toolbar_items(TextEditUI *app)
{
   GtkWidget *toolbar;
   GtkToolItem *new, *open, *save;
   GtkWidget *icon;

   toolbar = app->toolbar = gtk_toolbar_new();
   gtk_toolbar_set_show_arrow(GTK_TOOLBAR(toolbar), TRUE);

   // Change: Gtk.stock is deprecated since 3-11 and can be replaced
   // with gtk_image_new_from_icon_name
   icon = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_LARGE_TOOLBAR);
   new = gtk_tool_button_new(icon, "New");
   icon = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_LARGE_TOOLBAR);
   open = gtk_tool_button_new(icon, "Open");
   icon = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_LARGE_TOOLBAR);
   save = gtk_tool_button_new(icon, "Save");
   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new, 0);
   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, 1);
   gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, 2);

   g_signal_connect_swapped(G_OBJECT(new), "clicked",
                            G_CALLBACK(gtk_menu_item_activate),
                            (gpointer)app->filemenu->new);
   g_signal_connect_swapped(G_OBJECT(open), "clicked",
                            G_CALLBACK(gtk_menu_item_activate),
                            (gpointer)app->filemenu->open);
   g_signal_connect_swapped(G_OBJECT(save), "clicked",
                            G_CALLBACK(gtk_menu_item_activate),
                            (gpointer)app->filemenu->save);
   gtk_box_pack_start(GTK_BOX(app->vbox), toolbar, FALSE, FALSE, 0);
}

FileObject *text_edit_file_new(void)
{
   FileObject *new_file = g_new(FileObject, 1);

   new_file->scrolled_window = gtk_scrolled_window_new(NULL, NULL);
   new_file->textview = gtk_text_view_new();
   new_file->tab_label = gtk_label_new("Untitled File");

   gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(new_file->scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
   gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(new_file->scrolled_window),
                                       GTK_SHADOW_IN);
   gtk_container_set_border_width(GTK_CONTAINER(new_file->scrolled_window), 3);
   gtk_text_view_set_left_margin(GTK_TEXT_VIEW(new_file->textview), 3);
   gtk_text_view_set_right_margin(GTK_TEXT_VIEW(new_file->textview), 3);
   gtk_text_view_set_pixels_above_lines(GTK_TEXT_VIEW(new_file->textview), 1);
   // Change: gtk_widget_modify_font deprecated
   gtk_widget_override_font(new_file->textview, desc);

   gtk_container_add(GTK_CONTAINER(new_file->scrolled_window), new_file->textview);

   return new_file;
}

static void text_edit_tab_new_with_file(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   FileObject *f = text_edit_file_new();
   gint current_tab;

   current_tab = gtk_notebook_append_page(notebook, f->scrolled_window, f->tab_label);
   text_edit_register_filename("Untitled File", current_tab); /* Default name of each tab is Untitled */
   gtk_widget_show_all(GTK_WIDGET(notebook));
}

static void text_edit_select_font(GtkMenuItem *menu_item,
                                  gpointer notebook)
{
   // Change: gtk_font_selection_dialog_new is deprecated and replaced
   //  gtk_font_chooser_dialog_new that now requires parent window
   GtkWidget *parent_window = NULL;
   GtkWidget *font_dialog = NULL;

   if (GTK_IS_WIDGET(notebook))
   {
      parent_window = gtk_widget_get_toplevel(GTK_WIDGET(notebook));
   }

   if (parent_window == NULL || !GTK_IS_WINDOW(parent_window))
   {
      // Handle the case where parent_window is not valid or not a GtkWindow
      g_warning("Failed to get valid parent window.");
      return;
   }

   // Create the font chooser dialog with the retrieved parent window
   font_dialog = gtk_font_chooser_dialog_new("Choose A Font", GTK_WINDOW(parent_window));

   gchar *fontname;
   gint id;

   // Change: gtk_font_selection_dialog_set_preview_text is deprecated and
   // is repaced through a style context provider with a font setting
   //  Create a CSS provider
   GtkCssProvider *provider = gtk_css_provider_new();
   GdkScreen *screen = gdk_screen_get_default();
   gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
   gchar *css = g_strdup_printf("font { font-family: '%s'; font-size: 12px; }", pango_font_description_get_family(desc));

   // Load the CSS data into the provider
   gtk_css_provider_load_from_data(provider, css, -1, NULL);
   id = gtk_dialog_run(GTK_DIALOG(font_dialog));

   switch (id)
   {
   case GTK_RESPONSE_OK:
   case GTK_RESPONSE_APPLY:
      fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(font_dialog));
      desc = pango_font_description_from_string(fontname);
      break;

   case GTK_RESPONSE_CANCEL:
      break;
   }
   gtk_widget_destroy(font_dialog);

   text_edit_apply_font_selection(notebook);
}

static void text_edit_apply_font_selection(GtkNotebook *notebook)
{
   GList *child_list;
   gint pages;
   gint i;
   GtkWidget *swin;

   pages = gtk_notebook_get_n_pages(notebook);

   for (i = 0; i < pages; i++)
   {
      swin = gtk_notebook_get_nth_page(notebook, i);
      child_list = gtk_container_get_children(GTK_CONTAINER(swin));
      if (GTK_IS_TEXT_VIEW(child_list->data))
         gtk_widget_override_font(child_list->data, desc);
   }
}

static void text_edit_open_file(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GList *child_list;
   GtkWidget *dialog;
   GtkWidget *save_dialog, *error_dialog;
   gint current_page;
   gint id;
   gint offset;
   gchar *filename;
   gchar *contents;
   GtkWidget *scrolled_win;
   GtkWidget *view;
   GtkTextBuffer *buffer;
   GtkWidget *tab_name;
   GtkTextIter start, end;
   GtkTextMark *mark;
   GtkWidget *prompt_label;
   GtkWidget *content_area;

   // Change: gtk_file_chooser_dialog_new requires an GtkFileChooserAction
   // and a parent window
   GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

   GtkWidget *parent_window = NULL;

   // Get the parent window from the notebook
   if (GTK_IS_WIDGET(notebook))
   {
      parent_window = gtk_widget_get_toplevel(GTK_WIDGET(notebook));
   }

   if (parent_window == NULL || !GTK_IS_WINDOW(parent_window))
   {
      // Handle the case where parent_window is not valid or not a GtkWindow
      g_warning("Failed to get valid parent window.");
      return;
   }

   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);
   child_list = gtk_container_get_children(GTK_CONTAINER(scrolled_win));

   view = (GTK_IS_TEXT_VIEW(child_list->data) ? child_list->data : NULL);

   if (view != NULL)
   {
      dialog = gtk_file_chooser_dialog_new("Open A File",
                                           GTK_WINDOW(parent_window),
                                           action,
                                           "_Open", GTK_RESPONSE_ACCEPT,
                                           "_Cancel", GTK_RESPONSE_CANCEL,
                                           NULL);

      id = gtk_dialog_run(GTK_DIALOG(dialog));

      tab_name = gtk_notebook_get_tab_label(notebook, scrolled_win);

      switch (id)
      {
      case GTK_RESPONSE_ACCEPT:
         filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

         /* Prompt the user to save the File. */
         buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
         gtk_text_buffer_get_end_iter(buffer, &end);
         offset = gtk_text_iter_get_offset(&end);
         if (offset > 0)
         {
            save_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_INFO,
                                                 GTK_BUTTONS_NONE, NULL);

            gtk_dialog_add_button(GTK_DIALOG(save_dialog),
                                  "Save", GTK_RESPONSE_ACCEPT);
            gtk_dialog_add_button(GTK_DIALOG(save_dialog),
                                  "Delete", GTK_RESPONSE_CLOSE);
            gtk_dialog_add_button(GTK_DIALOG(save_dialog),
                                  "Cancel", GTK_RESPONSE_CANCEL);

            prompt_label = gtk_label_new("Save buffer contents?");
            content_area = gtk_dialog_get_content_area(GTK_DIALOG(save_dialog));

            gtk_box_pack_start(GTK_BOX(content_area), prompt_label, FALSE, FALSE, 0);
            gtk_widget_show_all(save_dialog);

            gtk_widget_hide(dialog);

            id = gtk_dialog_run(GTK_DIALOG(save_dialog));

            switch (id)
            {
            case GTK_RESPONSE_ACCEPT:
               text_edit_save_file(NULL, notebook);
               text_edit_register_filename(filename, current_page);
               break;

            case GTK_RESPONSE_CLOSE:
               gtk_text_buffer_get_bounds(buffer, &start, &end);
               gtk_text_buffer_delete(buffer, &start, &end);
               break;

            case GTK_RESPONSE_CANCEL:
               gtk_widget_destroy(save_dialog);
               return;
            }
            gtk_widget_destroy(save_dialog);
         }

         if (g_file_test(filename, G_FILE_TEST_EXISTS))
         {
            g_file_get_contents(filename, &contents, NULL, NULL);

            mark = gtk_text_buffer_get_insert(buffer);
            gtk_text_buffer_get_iter_at_mark(buffer, &start, mark);

            gtk_text_buffer_set_text(buffer, contents, -1);

            text_edit_register_filename(filename, current_page);

            gtk_label_set_text(GTK_LABEL(tab_name), basename(filename));
         }
         else
         {
            /* Unknown file name */
            error_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                                  GTK_MESSAGE_ERROR,
                                                  GTK_BUTTONS_OK, NULL);

            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
         }
         break;

      case GTK_RESPONSE_REJECT:
         break;
      }
      gtk_widget_destroy(dialog);
   }
}

static void text_edit_cut_to_clipboard(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GList *child_list;
   GtkWidget *textview;
   GtkTextBuffer *buffer;
   GtkWidget *scrolled_win;
   gint current_page;
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);
   child_list = gtk_container_get_children(GTK_CONTAINER(scrolled_win));
   textview = child_list->data;

   buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
   gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
}

static void text_edit_copy_to_clipboard(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GList *child_list;
   GtkWidget *textview;
   GtkTextBuffer *buffer;
   GtkWidget *scrolled_win;
   gint current_page;
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);
   child_list = gtk_container_get_children(GTK_CONTAINER(scrolled_win));
   textview = child_list->data;

   buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
   gtk_text_buffer_copy_clipboard(buffer, clipboard);
}

static void text_edit_paste_from_clipboard(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GList *child_list;
   GtkWidget *textview;
   GtkTextBuffer *buffer;
   GtkWidget *scrolled_win;
   gint current_page;
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);
   child_list = gtk_container_get_children(GTK_CONTAINER(scrolled_win));
   textview = child_list->data;

   buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
   gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}

static void text_edit_show_about_dialog(GtkMenuItem *menu_item, GtkWindow *parent_window)
{
   const gchar *authors[] = {"Akash Nagaraj", NULL};

   gtk_show_about_dialog(parent_window,
                         "program-name", "Text Editor",
                         "authors", authors,
                         "license", "GNU General Public License",
                         "comments", "A simple GTK+ text editor",
                         NULL);
}

static void text_edit_close_file(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GtkWidget *scrolled_win;
   gint current_page;

   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);

   gtk_widget_destroy(scrolled_win); /* Remove current tab */
}

static void text_edit_save_file(GtkMenuItem *menu_item, GtkNotebook *notebook)
{
   GList *child_list;
   GtkWidget *dialog;
   GtkWidget *textview;
   GtkTextBuffer *buffer;
   GtkWidget *scrolled_win;
   gint current_page;
   gint response;
   GtkWidget *tab_label;
   GtkTextIter start, end;
   gchar *filename;
   gchar *contents;

   GtkWidget *parent_window = NULL;

   // Change: gtk_file_chooser_dialog_new now requires a parent window and an action
   if (GTK_IS_WIDGET(notebook))
   {
      parent_window = gtk_widget_get_toplevel(GTK_WIDGET(notebook));
   }

   if (parent_window == NULL || !GTK_IS_WINDOW(parent_window))
   {
      // Handle the case where parent_window is not valid or not a GtkWindow
      g_warning("Failed to get valid parent window.");
      return;
   }

   current_page = gtk_notebook_get_current_page(notebook);
   scrolled_win = gtk_notebook_get_nth_page(notebook, current_page);
   child_list = gtk_container_get_children(GTK_CONTAINER(scrolled_win));
   textview = child_list->data;
   tab_label = gtk_notebook_get_tab_label(notebook, scrolled_win);

   if (strcmp(gtk_label_get_text(GTK_LABEL(tab_label)), "Untitled File") == 0)
   { /* Check if filename is the same as default name */
      /* Allow user to name the file.*/
      dialog = gtk_file_chooser_dialog_new("Save File",
                                           GTK_WINDOW(parent_window),
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Save", GTK_RESPONSE_ACCEPT,
                                           "_Cancel", GTK_RESPONSE_CANCEL,
                                           NULL);

      response = gtk_dialog_run(GTK_DIALOG(dialog));

      if (response == GTK_RESPONSE_APPLY)
      {
         filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

         buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
         gtk_text_buffer_get_bounds(buffer, &start, &end);
         contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

         g_file_set_contents(filename, contents, -1, NULL);

         text_edit_register_filename(filename, current_page); // ADDED

         gtk_label_set_text(GTK_LABEL(tab_label), basename(filename));
      }
      else if (response == GTK_RESPONSE_CANCEL)
      {
         gtk_widget_destroy(dialog);

         return;
      }

      gtk_widget_destroy(dialog);
   }
   else
   {
      filename = text_edit_get_filename(current_page);

      buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));
      gtk_text_buffer_get_bounds(buffer, &start, &end);
      contents = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

      g_file_set_contents(filename, contents, -1, NULL);
   }
}

static void text_edit_register_filename(gchar *fname, gint tab_num)
{
   gint found = FALSE;
   FileData *f = g_new(FileData, 1);
   GList *node = g_list_alloc();
   f->filename = fname;
   f->tab_number = tab_num;
   node->data = f;

   if (filename_data == NULL)
      filename_data = node; /* First node in list */
   else
   {
      GList *list = filename_data;

      while (list != NULL)
      {
         if (((FileData *)list->data)->tab_number == tab_num)
         {
            found = TRUE;

            ((FileData *)list->data)->filename = fname;

            break;
         }
         else
            list = g_list_next(list);
      }
      if (!found)
         list = g_list_append(filename_data, node);
   }
}

static gchar *text_edit_get_filename(gint tab_num)
{
   GList *list = filename_data;
   while (list != NULL)
   {
      if (((FileData *)list->data)->tab_number == tab_num)
         return ((FileData *)list->data)->filename;
      else
         list = g_list_next(filename_data);
   }
   return NULL;
}
