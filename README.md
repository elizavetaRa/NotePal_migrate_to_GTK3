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
