// Header files
#include <dirent.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gtk/gtk.h>


// Define terminal color output
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Prototype 
static GtkWidget *create_view_and_model (void); 
static GtkTreeModel *create_and_fill_model (void) ; 
int getFileType(const char *path);
int getFileSize(const char *file); 
GdkPixbuf *getFileIcon(int fileType); 


// Compile with gcc -o tool ToolBar.c `pkg-config --cflags --libs gtk+-2.0`

enum {
  COL_NAME = 0,
  COL_SIZE,
  COL_ICON,
  NUM_COLS
} ;

int main (int argc, char *argv[]) {
  // Widgets 
  GtkWidget *window;
  GtkWidget *vbox;
  
  GtkWidget *view;
  GtkWidget *toolbar;
  GtkToolItem *new;
  GtkToolItem *open;
  GtkToolItem *save;
  GtkToolItem *exit;

  gtk_init (&argc, &argv);

  // Window 
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), 250, 200);
  gtk_window_set_title(GTK_WINDOW(window), "Directory View");
      
  // Vbox  
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  // Toolbar 
  toolbar = gtk_toolbar_new();
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  
  // New 
  new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new, -1);
  
  // Open 
  open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);
  
  // Save 
  save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);
  
  // Exit 
  exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), exit, -1);
  
  // Pack Vbox
  gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 5);
  
  // Pack View 
  view = create_view_and_model ();
  gtk_box_pack_start(GTK_BOX(vbox), view, TRUE, TRUE, 0);
  gtk_widget_grab_focus(view);
   

  gtk_widget_show_all(window);

  gtk_main();
  return 0;
}

// Set the pointer to the Model and fill it 
static GtkTreeModel *create_and_fill_model (void) {
  GtkListStore  *store;
  GtkTreeIter    iter;
  
  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT, GDK_TYPE_PIXBUF); // types derived from standard GObjects   
  
  DIR           *directory;
  struct dirent *dir;
  // Get working directory value 
  char myCWD[1024];
  getcwd(myCWD, 1024);

  directory = opendir(myCWD);// Open current working directory stream  
  if (directory){
    while ((dir = readdir(directory)) != NULL){
			// Append a row and fill in some data 
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter,
                      COL_NAME,dir->d_name, // Set COL_NAME to dir name 
                      COL_SIZE, getFileSize(dir->d_name),
                      COL_ICON, getFileIcon(getFileType(dir->d_name)),
                      -1); //remember to end the list of columns with a -1
				  }
  }
  else {
  // could not open directory 
  perror ("Error could not open directory ");
}


  return GTK_TREE_MODEL (store);
}

// Create the columns 
static GtkWidget *create_view_and_model (void) {
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();

  // Column 1 
  renderer = gtk_cell_renderer_pixbuf_new();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Icon",  
                                               renderer,
                                               "pixbuf", COL_ICON,
                                               NULL);

  // Column 2 
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Name",  
                                               renderer,
                                               "text", COL_NAME,
                                               NULL);
                                               
  // Column 3 
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Size (Byte)",  
                                               renderer,
                                               "text", COL_SIZE,
                                               NULL);
                                               
  model = create_and_fill_model ();

  gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

  /* The tree view has acquired its own reference to the
   *  model, so we can drop ours. That way the model will
   *  be freed automatically when the tree view is destroyed 
   */

  g_object_unref (model);

  return view;
}

// Get fil info 
int getFileType(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

//Get file size 
int getFileSize(const char *file){
	 struct stat fileStat;
	 stat(file, &fileStat);
     return fileStat.st_size ; 
}

// Point to correct icon 
GdkPixbuf *getFileIcon(int fileType){
    GdkPixbuf     *icon;
    GError        *error = NULL;
    
      switch(fileType) {
        case 0:
            icon = gdk_pixbuf_new_from_file("icon.png", &error);
            break;
        default:
            icon = gdk_pixbuf_new_from_file("file.png", &error);
            break;
    }
    // Could not load icon 
    if (error){
      g_warning ("Could not load icon: %s\n", error->message);
      g_error_free(error);
      error = NULL;
    }

    return icon;
  }
	
	





