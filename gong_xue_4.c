#include <stdlib.h>
/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
/* I change this command to gcc xlibdemo.c -lX11 -lm */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>


#define MAX(a, b) ((a) > (b))?(a):(b)
#define MIN(a, b) ((a) < (b))?(a):(b)

#define TRUE 1
#define FALSE 0

#define N 500
#define V 1002


typedef struct {
  int x;
  int y;
}point;


Display *display_ptr;
Screen *screen_ptr;
int screen_num;
char *display_name = NULL;
int display_width, display_height;

Window win;
int border_width;
int win_width, win_height;
int win_x, win_y;

XWMHints *wm_hints;
XClassHint *class_hints;
XSizeHints *size_hints;
XTextProperty win_name, icon_name;
char *win_name_string = "Andrew GONG's Window"; /* change by andrew */
char *icon_name_string = "Icon for Andrew's Window";

XEvent report;

GC gc,  gc_red, gc_yellow, gc_grey;
unsigned long valuemask = 0;
XGCValues gc_values, gc_red_values, gc_yellow_values, gc_grey_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

int mindistant(int dist[], int sptset[]);
int dijkstra(unsigned int G[V][V], int n, point array[]);
int intersect(point p, point q, point s, point t);
int onsegment(point p, point q, point s);
int orientation(point p, point q, point s);
int distant(point p, point q);

int main(int argc, char **argv)
{
  /* code add by andrew GONG for reading file from command line argument */
    
    point p[N];
    point q[N];
    point array[V];
    unsigned int G[V][V];

    point s = {0, 0};
    point t = {0, 0};
    int i, j, k;
    int press;
    i = j = k = 0;
    press = 0;

    for (i = 0; i < N; i++) {
      p[i].x = p[i].y = 0;
      q[i].x = q[i].y = 0;
    }
    
    if(argc != 2) {
      printf("usage:%s filename", argv[0]);
    }
    else {
      FILE *file = fopen(argv[1], "r");
      if(file == 0) {
        printf("Could not open file!\n");
      }
      else {
        i = 0;
        printf("%s\n", argv[1]);
        while(!feof(file)) {
          if(fscanf(file, "S (%d, %d) (%d, %d)\n",
		    &p[i].x, &p[i].y, &q[i].x, &q[i].y) != 4) {
            break;
          }
          else {
            i++;
          }
        }
      }
      
      fclose(file);
    }
    
    
    


  /* opening display: basic connection to X Server */
  if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); exit(-1); }
  printf("Connected to X server  %s\n", XDisplayName(display_name));
  screen_num = DefaultScreen(display_ptr);
  screen_ptr = DefaultScreenOfDisplay(display_ptr);
  color_map  = XDefaultColormap(display_ptr, screen_num);
  display_width  = DisplayWidth(display_ptr, screen_num);
  display_height = DisplayHeight(display_ptr, screen_num);

  printf("Width %d, Height %d, Screen Number %d\n", 
           display_width, display_height, screen_num);

  /* creating the window */
  border_width = 10;
  win_x = 0; win_y = 0;
  win_width = display_width/2; 
  win_height = (unsigned int) (win_width / 1.7); /* rectangular window */
  win= XCreateSimpleWindow(display_ptr, RootWindow(display_ptr, screen_num), win_x, win_y, win_width, win_height, border_width, BlackPixel(display_ptr, screen_num), WhitePixel(display_ptr, screen_num));

/* now try to put it on screen, this needs cooperation of window manager */
  size_hints = XAllocSizeHints();
  wm_hints = XAllocWMHints(); 
  class_hints = XAllocClassHint(); 
  if( size_hints == NULL || wm_hints == NULL || class_hints == NULL ) {
    printf("Error allocating memory for hints. \n"); exit(-1); }
  size_hints -> flags = PPosition | PSize | PMinSize  ; 
  size_hints -> min_width = 60; 
  size_hints -> min_height = 60; 
  XStringListToTextProperty(&win_name_string, 1, &win_name);
  XStringListToTextProperty(&icon_name_string, 1, &icon_name);
  wm_hints -> flags = StateHint | InputHint ; 
  wm_hints -> initial_state = NormalState; 
  wm_hints -> input = False; 
  class_hints -> res_name = "x_use_example"; 
  class_hints -> res_class = "examples"; 
  XSetWMProperties(display_ptr, win, &win_name, &icon_name, argv, argc, size_hints, wm_hints, class_hints);

  /* what events do we want to receive */
  XSelectInput(display_ptr, win,
            ExposureMask | StructureNotifyMask | ButtonPressMask);
  
  /* finally: put window on screen */
  XMapWindow(display_ptr, win);

  XFlush(display_ptr);
  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC(display_ptr, win, valuemask, &gc_values);
  XSetForeground(display_ptr, gc, BlackPixel(display_ptr, screen_num));
  XSetLineAttributes(display_ptr, gc, 2, LineSolid, CapRound, JoinRound);

  /* and three other graphics contexts, to draw in yellow and red and grey */
  gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 2, LineSolid, CapRound, JoinRound);
  if(XAllocNamedColor(display_ptr, color_map, "yellow",
			&tmp_color1, &tmp_color2 ) == 0)
    {printf("failed to get color yellow\n"); exit(-1); } 
  else
    XSetForeground(display_ptr, gc_yellow, tmp_color1.pixel);
  gc_red = XCreateGC(display_ptr, win, valuemask, &gc_red_values);
 /* XSetLineAttributes(display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound); */
  if( XAllocNamedColor(display_ptr, color_map, "red",
			&tmp_color1, &tmp_color2) == 0 )
    {printf("failed to get color red\n"); exit(-1);} 
  else
    XSetForeground(display_ptr, gc_red, tmp_color1.pixel);
  gc_grey = XCreateGC(display_ptr, win, valuemask, &gc_grey_values);
  if( XAllocNamedColor(display_ptr, color_map, "light grey",
			&tmp_color1, &tmp_color2) == 0)
    {printf("failed to get color grey\n"); exit(-1);} 
  else
    XSetForeground(display_ptr, gc_grey, tmp_color1.pixel);

  /* and now it starts: the event loop */
  while(1) { 
    XNextEvent( display_ptr, &report );
    switch(report.type)
    {
      case Expose:
      /* (re-)draw the example figure. This event happens
       * each time some part ofthe window gets exposed (becomes visible) */
      
      /* print all the obstacles */
      for (i = 0; i < N; i++) {
        if (p[i].x != 0 && p[i].y != 0 && q[i].x != 0 && q[i].y !=0) {
          XDrawLine(display_ptr, win, gc, p[i].x, p[i].y, q[i].x, q[i].y);
        }
      }
      break;
      case ConfigureNotify:
      /* This event happens when the user changes the size of the window */
      win_width = report.xconfigure.width;
      win_height = report.xconfigure.height;
      break;

      /* This event happens when the user pushes a mouse button. I draw
       * a circle to show the point where it happened, but do not save 
       * the position; so when the next redraw event comes, these circles
       * disappear again. */
      case ButtonPress: {
      int x, y;
      x = report.xbutton.x;
      y = report.xbutton.y;
      /* read point s, t */
      if (press == 0) {
	    s.x = report.xbutton.x;
	    s.y = report.xbutton.y;
      }
      else if (press == 1) {
	    t.x = report.xbutton.x;
	    t.y = report.xbutton.y;
      }
      
      press += 1;
      
      if (report.xbutton.button == Button1)
	    XFillArc(display_ptr, win, gc_red,
        x - win_height/40, y - win_height/40,
        win_height / 20, win_height / 20, 0, 360*64);
      else
	    XFillArc(display_ptr, win, gc_yellow,
        x - win_height / 40, y - win_height / 40,
        win_height / 20, win_height / 20, 0, 360*64);
      
      
      printf("%d, %d, %d, %d\n", s.x, s.y, t.x, t.y);
      printf("press = %d\n", press);
      
      if (press == 2) {
        XFlush(display_ptr);
          /* construction graph G
           *s is the first point and t is the last point */
	
        array[0].x = s.x;
        array[0].y = s.y;
        array[V-1].x = t.x;
        array[V-1].y = t.y;
	  
        for (i = 1; i < V-1; i++) {
          if ( i <= N ) {
            array[i].x = p[i-1].x;
            array[i].y = p[i-1].y;
          }
          else if (i > N) {
            array[i].x = q[i-N-1].x;
            array[i].y = q[i-N-1].y;
          }
        }
	
        for (i = 0; i < V; i++) {
          int j;
          for(j = 0; j < V; j++) {
            if (( array[i].x == 0 && array[i].y == 0 ) || ( array[j].x == 0 && array[j].y == 0)) {
              G[i][j] = INT_MAX;
            }
            else {
              G[i][j] = distant(array[i],array[j]);
            }
            for (k = 0; k < N; k++) {
              if (p[k].x != 0 && p[k].y != 0 && q[k].x != 0 &&
                  q[k].y != 0 && array[i].x !=0 && array[i].y !=0 &&
                  intersect( array[i], array[j], p[k], q[k])) {
                G[i][j] = INT_MAX;
              }
            }
          }
        }
	    
        dijkstra(G, 0, array);
      }
    }
      break;
    default:
      /* this is a catch-all for other events; it does not do anything.
       *One could look at the report type to see what the event was */ 
      break;
    }
    
  }
  
  exit(0);
}



/* add my shortest path algorithm code here */
int distant(point p, point q)
{
  int d = sqrt((q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y));
  return d;
}

int orientation(point p, point q, point s)
{
  /*counterclockwise == -1, clockwise = 1 colinear = 0 */
  
  int temp;
  temp = (s.y - q.y) * (q.x - p.x) - (q.y - p.y) * (s.x - q.x);
        if (temp == 0) {
	  return 0;
        }
        else if (temp > 0) {
	  return 1;
        }
        else if (temp < 0) {
	  return -1;
        }
	return 0;
}



int onsegment(point p, point q, point s) /* s on pq */
{
  if (s.x <= MAX(p.x, q.x) && s.x >= MIN(p.x, q.x) &&
           s.y <= MAX(p.y, q.y) && s.y >= MIN(p.y, q.y)) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}




int intersect(point p, point q, point s, point t)
{
  int i, j, k, f;
  
/* 1 clockwise, -1 counterclockwise, 0 colinear */
  i = orientation(p, q, s);
  j = orientation(p, q, t);
  k = orientation(s, t, p);
  f = orientation(s, t, q);
  
  if (i != 0 && j != 0 && k != 0 && f != 0 && i != j && k != f) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}




int mindistant(int dist[], int sptset[])
{
  int min = INT_MAX;
  int min_index;
  
  int v;
  for (v = 0; v < V; v++) {
    if (sptset[v] == FALSE && dist[v] <= min) {
      min = dist[v];
      min_index = v;
    }
  }
  
  return min_index;
}




int dijkstra(unsigned int G[V][V], int n, point array[])
{
  int dist[V];
  int sptset[V];
  int parent[V]; 
  
  int i, v;
  
  for (i = 0; i < V; i++) {
    dist[i] = INT_MAX;
    sptset[i] = FALSE;
    parent[i] = 0;
  }
  
  dist[n] = 0;
  
  for (i = 0; i < V-1; i++) {
    int u = mindistant(dist, sptset);
    sptset[u]=TRUE;
    
    for (v = 0; v < V; v++ ) {
      if (!sptset[v] && G[u][v] && dist[u] != INT_MAX && dist[u] + G[u][v] < dist[v]) {
	dist[v] = dist[u] + G[u][v];
	parent[v] = u;
      }
    }
  }
  
  printf("shortest path from s to t is %d\n", dist[v-1]);
  
  
  i = V-1;
  do {
    XDrawLine(display_ptr, win, gc_red, 
	      array[i].x, array[i].y, 
	      array[parent[i]].x, array[parent[i]].y);
    XFlush(display_ptr);
    i = parent[i];
  } while(i != 0);


  XFlush(display_ptr);	
  return dist[v];
}

