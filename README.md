# **NotePal**

An intuitive Text Editor with a simple Graphical User Interface for Linux, built with GTK+ Libraries. With **NotePal**, you can open multiple files at once (as tabs) and can interact with your files intuitively. **NotePal** was built using GTK 2.0. It's great for those who prefer a Grapic based text editor.

It has only the basic functions such as:

- Open
- Close
- Save
- Copy
- Paste
- Change Font

## Installation

##### To use **NotePal**, simply run the executable file: '_Notepal_'

If you wish to change the properties **NotePal**, simply use compile the command:

```gcc -g -Wall CompileNotePal.c -o NotePal \`pkg-config --cflags --libs gtk+-3.0\````

Which produces an executable **NotePal** instance.

**_NOTE:_** The only dependencies for **NotePal** are the GTK Libraries.
If you encounter dependency errors, simply run:

`sudo apt-get install libgtk-3-dev`

followed by the compile command again.

##### To run previous GTK2 Version, checkout the last related commit:

`git checkout 15d0a61cdd672ec877d872fc49b1cb54d3293a71 `

and then run `` gcc -g -Wall CompileNotePal.c -o NotePal `pkg-config --cflags --libs gtk+-2.0 ``

##### Changes log for GTK3 upgrade

- **line 166:** Change: Gtk.stock is deprecated since 3-11 and can be replaced with a new widget signal gtk_widget_add_accelerator
- **line 168:** Change: GTK*'Letter' keyboard event throws compillation error "undeclared", is replaced through GDK_KEY*'letter'
- **line 247:** Change: Gtk.stock is deprecated since 3-11 and can be replaced with gtk_image_new_from_icon_name
- **line 287:** Change: gtk_widget_modify_font deprecated
- **line 308:** Change: gtk_font_selection_dialog_new is deprecated and replaced gtk_font_chooser_dialog_new that now requires parent window
- **line 331:** Change: gtk_font_selection_dialog_set_preview_text is deprecated and is repaced through a style context provider with a font setting
- **line 396:** Change: gtk_file_chooser_dialog_new requires an GtkFileChooserAction and a parent window
- **line 607:** Change: gtk_file_chooser_dialog_new now requires a parent window and an action
