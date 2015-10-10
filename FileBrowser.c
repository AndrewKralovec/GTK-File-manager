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
int checkFile(const char *path);
static GtkWidget *create_view_and_model (void); 
static GtkTreeModel *create_and_fill_model (void) ; 

enum {
  COL_NAME = 0,
  COL_AGE,
  NUM_COLS
} ;

int main (int argc, char *argv[]) {
  GtkWidget *window;
  GtkWidget *view;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); // dirty signal

  view = create_view_and_model ();

  gtk_container_add (GTK_CONTAINER (window), view);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}

// Set the pointer to the Model and fill it 
static GtkTreeModel *create_and_fill_model (void) {
  GtkListStore  *store;
  GtkTreeIter    iter;
  
  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);
  
  DIR           *directory;
  struct dirent *dir;
  // Get working directory value 
  char myCWD[1024];
  getcwd(myCWD, 1024);

  directory = opendir(myCWD);// Open current working directory stream  
  if (directory){
    while ((dir = readdir(directory)) != NULL){
			/* Append a row and fill in some data */
			gtk_list_store_append (store, &iter);
			gtk_list_store_set (store, &iter,
                      COL_NAME,dir->d_name, // Set COL_NAME to dir name 
                      COL_AGE, 51,
                      -1); //remember to end the list of columns with a -1
				  }
  }
  else {
  // could not open directory 
  perror ("Error could not open directory ");
}


  return GTK_TREE_MODEL (store);
}

static GtkWidget *create_view_and_model (void) {
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();

  // Column 1 
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Name",  
                                               renderer,
                                               "text", COL_NAME,
                                               NULL);

  // Column 2 
  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Age",  
                                               renderer,
                                               "text", COL_AGE,
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



int checkFile(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}






