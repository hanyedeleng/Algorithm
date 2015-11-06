#include <stdlib.h>
/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
/* I change this command to gcc xlibdemo.c -lX11 -lm */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>



#define RED	0 
#define GREEN	1
#define YELLOW	2
#define BLUE	3
#define TRUE	1
#define FALSE	0

#define N	50	/* this is the max number of the point */
#define INT_MAX	9999    /* windows size 700 * 700, this size should be enough,(700 * sqrt2) */


typedef struct {
  int x;
  int y;
}point;

static int sum_non_optimal_dist;		
static int sum_optimal_dist;	
static int sum_point_number;

int c[N][N][N][N];
int parent[N][N][N][N];

/* init the yellow, red, blue server, the window size is 700 * 700*/
point server_y = {0, 0};
point server_r = {700, 0};
point server_b = {350, 700};

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

GC gc,  gc_red, gc_yellow, gc_grey, gc_green, gc_blue;
GC gc_red_thick, gc_yellow_thick, gc_blue_thick;
unsigned long valuemask = 0;
XGCValues gc_values, gc_red_values, gc_yellow_values, gc_grey_values, gc_green_values, gc_blue_values;
XGCValues gc_red_thick_values, gc_yellow_thick_values, gc_blue_thick_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

/* function declaration */
int distant(point p, point q);
int minvalue(int p, int q, int r);



int main(int argc, char **argv)
{
  int x, y, t, i, j, k,h, n;
  int tmp, tmpi, tmpj, tmpk;
  int parent_tmpi, parent_tmpj, parent_tmpk;
  int distpy, distpr, distpb;
  static int mindist;
  int index;
  point array[N];
  point optarray[N];
  int minarray[N];
  i = j = k = 0;
  
  for (i = 0; i < N; i++) {
    array[i].x = array[i].y = 0;
    optarray[i].x = optarray[i].y = 0;
  }


  /* opening display: basic connection to X Server */
  if( (display_ptr = XOpenDisplay(display_name)) == NULL )
    { printf("Could not open display. \n"); exit(-1); }
  printf("Connected to X server  %s\n", XDisplayName(display_name) );
  screen_num = DefaultScreen( display_ptr );
  screen_ptr = DefaultScreenOfDisplay( display_ptr );
  color_map  = XDefaultColormap( display_ptr, screen_num );
  display_width  = DisplayWidth( display_ptr, screen_num );
  display_height = DisplayHeight( display_ptr, screen_num );
  printf("Width %d, Height %d, Screen Number %d\n", 
           display_width, display_height, screen_num);

  /* creating the window */
  border_width = 10;
  win_x = 0; win_y = 0;
  win_width = display_width/1.5; 
  win_height = (unsigned int) (win_width / 1.2); /* rectangular window */
  win= XCreateSimpleWindow( display_ptr, RootWindow( display_ptr, screen_num), win_x, win_y, win_width, win_height, border_width, BlackPixel(display_ptr, screen_num), WhitePixel(display_ptr, screen_num) ); 

/* now try to put it on screen, this needs cooperation of window manager */
  size_hints = XAllocSizeHints();
  wm_hints = XAllocWMHints(); 
  class_hints = XAllocClassHint(); 
  if( size_hints == NULL || wm_hints == NULL || class_hints == NULL ) 
    { printf("Error allocating memory for hints. \n"); exit(-1); } 
  size_hints -> flags = PPosition | PSize | PMinSize  ; 
  size_hints -> min_width = 60; 
  size_hints -> min_height = 60; 
  XStringListToTextProperty( &win_name_string, 1, &win_name); 
  XStringListToTextProperty( &icon_name_string, 1, &icon_name); 
  wm_hints -> flags = StateHint | InputHint ; 
  wm_hints -> initial_state = NormalState; 
  wm_hints -> input = False; 
  class_hints -> res_name = "x_use_example"; 
  class_hints -> res_class = "examples"; 
  XSetWMProperties( display_ptr, win, &win_name, &icon_name, argv, argc, size_hints, wm_hints, class_hints );

  /* what events do we want to receive */
  XSelectInput( display_ptr, win, 
            ExposureMask | StructureNotifyMask | ButtonPressMask );
  
  /* finally: put window on screen */
  XMapWindow( display_ptr, win );

  XFlush(display_ptr);
  /* create graphics context, so that we may draw in this window */
  gc = XCreateGC( display_ptr, win, valuemask, &gc_values);
  XSetForeground( display_ptr, gc, BlackPixel( display_ptr, screen_num ) );
  XSetLineAttributes( display_ptr, gc, 2, LineSolid, CapRound, JoinRound); 

  /* and three other graphics contexts, to draw in yellow and red and grey */
  gc_yellow = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow, 2, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "yellow", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color yellow\n"); exit(-1); } 
  else
    XSetForeground( display_ptr, gc_yellow, tmp_color1.pixel );

  gc_yellow_thick = XCreateGC( display_ptr, win, valuemask, &gc_yellow_values);
  XSetLineAttributes(display_ptr, gc_yellow_thick, 5, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "yellow", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color yellow\n"); exit(-1); } 
  else
    XSetForeground( display_ptr, gc_yellow_thick, tmp_color1.pixel );


  gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes( display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "red", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_red, tmp_color1.pixel );

  gc_red_thick = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
  XSetLineAttributes( display_ptr, gc_red_thick, 5, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "red", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_red_thick, tmp_color1.pixel );


  gc_grey = XCreateGC( display_ptr, win, valuemask, &gc_grey_values);
  if( XAllocNamedColor( display_ptr, color_map, "light grey", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color grey\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_grey, tmp_color1.pixel ); 

  gc_green = XCreateGC( display_ptr, win, valuemask, &gc_green_values);
  if( XAllocNamedColor( display_ptr, color_map, "light green", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color green\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_green, tmp_color1.pixel ); 


  gc_blue = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
  XSetLineAttributes( display_ptr, gc_blue, 1, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "light blue", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color blue\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_blue, tmp_color1.pixel ); 

  gc_blue_thick = XCreateGC( display_ptr, win, valuemask, &gc_blue_values);
  XSetLineAttributes( display_ptr, gc_blue_thick, 5, LineSolid, CapRound, JoinRound);
  if( XAllocNamedColor( display_ptr, color_map, "light blue", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color blue\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_blue_thick, tmp_color1.pixel ); 



  /* and now it starts: the event loop */

  i = 0;
  sum_non_optimal_dist = 0;
  while(1) { 
    XNextEvent( display_ptr, &report );
    switch( report.type ) {
    case Expose:
      /* (re-)draw the example figure. This event happens
	 each time some part ofthe window gets exposed (becomes visible) */
      XDrawRectangle(display_ptr, win, gc_green, 0, 0, 700, 700);
      XDrawArc(display_ptr, win, gc, server_y.x - 8, server_y.y - 8, 2 * 8, 2 * 8, 0, 360*64);
      XFillArc(display_ptr, win, gc_yellow, server_y.x - 8, server_y.y - 8, 2 * 8, 2 * 8, 0, 360*64);
      XDrawArc(display_ptr, win, gc, server_r.x - 8, server_r.y - 8, 2 * 8, 2 * 8, 0, 360*64);
      XFillArc(display_ptr, win, gc_red, server_r.x - 8, server_r.y - 8, 2 * 8, 2 * 8, 0, 360*64);
      XDrawArc(display_ptr, win, gc, server_b.x - 8, server_b.y - 8, 2 * 8, 2 * 8, 0, 360*64);
      XFillArc(display_ptr, win, gc_blue, server_b.x - 8, server_b.y - 8, 2 * 8, 2 * 8, 0, 360*64);
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
    case ButtonPress: 
    {  
     /*  int x, y; */
      x = report.xbutton.x;
      y = report.xbutton.y;
      if (report.xbutton.button == Button1 ) {
          XFillArc( display_ptr, win, gc_grey, x -win_height/100, y- win_height/100, win_height/100, win_height/100, 0, 360*64);
	  array[i].x = x;
	  array[i].y = y;

	  distpy = distant(array[i], server_y);
	  distpr = distant(array[i], server_r);
	  distpb = distant(array[i], server_b);
	
	  if (distpy <= distpr && distpy <= distpb) {
		sum_non_optimal_dist += distpy;
		XDrawLine(display_ptr, win, gc_yellow, server_y.x, server_y.y, array[i].x, array[i].y );
	  	XFlush(display_ptr);
		server_y.x = array[i].x;
		server_y.y = array[i].y;
	  }
	  else if (distpr <= distpy && distpr <= distpb) {
		sum_non_optimal_dist += distpr;
		XDrawLine(display_ptr, win, gc_red, server_r.x, server_r.y, array[i].x, array[i].y );
	  	XFlush(display_ptr);
		server_r.x = array[i].x;
		server_r.y = array[i].y;
	  }
	  else if (distpb <= distpy && distpb <= distpr) {
		sum_non_optimal_dist += distpb;
		XDrawLine(display_ptr, win, gc_blue, server_b.x, server_b.y, array[i].x, array[i].y );
	  	XFlush(display_ptr);
		server_b.x = array[i].x;
		server_b.y = array[i].y;

	  }

	  
	  i++;
      }
      else {
          XFillArc( display_ptr, win, gc, x - win_height/100, y - win_height/100, win_height/100, win_height/100, 0, 360*64);
	  XFlush(display_ptr);
	  goto outprint; 
      }


    }
      sum_point_number = i;
      break;
    default:
      /* this is a catch-all for other events; it does not do anything.
       *One could look at the report type to see what the event was */ 
      break;
    }
    
  }
  
 outprint: 
	printf("\nNon-optimal result: %d", sum_non_optimal_dist);
	t = i = j = k = 0;
	server_y.x = 0;
	server_y.y = 0;
	server_r.x = 700;
	server_r.y = 0;
	server_b.x = 350;
	server_b.y = 700;

	optarray[0].x = server_y.x;
	optarray[0].y = server_y.y;
	optarray[1].x = server_r.x;
	optarray[1].y = server_r.y;
	optarray[2].x = server_b.x;
	optarray[2].y = server_b.y;

	for (i = 0; i < sum_point_number; i++) {
	  optarray[i+3].x = array[i].x;
	  optarray[i+3].y = array[i].y;
	}

	for (t = 0; t < N; t++) {
	  for (i = 0; i < N; i++) {
	    for (j = 0; j < N; j++) {
	      for ( k = 0; k < N; k++) {
		c[t][i][j][k] = INT_MAX;
	      }
	    }
	  }
	}

	c[2][0][1][2] = 0; /* 0: yellow server, 1: red_server, 2 blue_server */

/* compute all the values according to the previous ones, c[t][i][j][k] <== c[t-1][i][j][k](i, j, k < t)*/
	for (t = 3; t < sum_point_number+3; t++) {
	  for (i = 0; i < t + 1; i++) {
	    for (j = 1; j < t + 1; j++) {
	      for (k = 2; k < t + 1; k++) {
		if (t != i && t != j && t != k) {
		  c[t][i][j][k] = INT_MAX;	
		} 
		else {
		  for (h = 0; h < t; h++) {
		    tmpi = c[t-1][h][j][k] + distant(optarray[i], optarray[h]);
		    if (c[t][i][j][k] > tmpi) {
		       c[t][i][j][k] = tmpi;
		       parent_tmpi = h;
		    }
		  }
		    tmpi = c[t][i][j][k];
		 
		  for (h = 0; h < t; h++) {
		    tmpj = c[t-1][i][h][k] + distant(optarray[j], optarray[h]);
		    if (c[t][i][j][k] > tmpj) {
		      c[t][i][j][k] = tmpj;
		      parent_tmpj = h;
		    }
		  }
	    	    tmpj = c[t][i][j][k];

		  for (h = 0; h < t; h++) {
		    tmpk = c[t-1][i][j][h] + distant(optarray[k], optarray[h]);
		    if (c[t][i][j][k] > tmpk) {
		      c[t][i][j][k] = tmpk;
		      parent_tmpk = h;
		    }
		  }
		    tmpk = c[t][i][j][k];

		  c[t][i][j][k] = minvalue(tmpi, tmpj, tmpk);
		  if (c[t][i][j][k] == tmpi) {
		    /*optarray[t].color = YELLOW;*/
		    parent[t][i][j][k] = parent_tmpi;
		  }
		  else if (c[t][i][j][k] == tmpj) {
		    parent[t][i][j][k] = parent_tmpj;
		  }
		  else if (c[t][i][j][k] == tmpk) {
		    parent[t][i][j][k] = parent_tmpk;
		  }
		}
	      }
	    }
	  }
	}


	t = sum_point_number + 3;;
	mindist = INT_MAX;
	for (i = 0; i < t; i++) {
	  for (j = 1; j < t; j++) {
	    for (k = 2; k < t; k++) {
	      if (c[t-1][i][j][k] < mindist) { 
	        mindist = c[t-1][i][j][k];
		tmpi = i;
		tmpj = j;
		tmpk = k;
  	      }
	    }
	  }
	}

	t = sum_point_number + 2;
	do {
	  if(tmpi == t) {
	    XDrawLine(display_ptr, win, gc_yellow_thick, optarray[t].x, optarray[t].y, optarray[parent[t][tmpi][tmpj][tmpk]].x, optarray[parent[t][tmpi][tmpj][tmpk]].y);
            XFlush(display_ptr);
	    tmpi = parent[t][tmpi][tmpj][tmpk];
	  }
	  else if (tmpj == t) {
	    XDrawLine(display_ptr, win, gc_red_thick, optarray[t].x, optarray[t].y, optarray[parent[t][tmpi][tmpj][tmpk]].x, optarray[parent[t][tmpi][tmpj][tmpk]].y);
            XFlush(display_ptr);
	    tmpj = parent[t][tmpi][tmpj][tmpk];
	  }
	  else if (tmpk == t) {
	    XDrawLine(display_ptr, win, gc_blue_thick, optarray[t].x, optarray[t].y, optarray[parent[t][tmpi][tmpj][tmpk]].x, optarray[parent[t][tmpi][tmpj][tmpk]].y);
            XFlush(display_ptr);
	    tmpk = parent[t][tmpi][tmpj][tmpk];
	  } 
	  t--;
	}while (t > 2);

	printf("\nOptimal offline: %d\n", mindist);
	printf("Ratio: %f\n", (float)sum_non_optimal_dist / mindist);
	sleep(60);
  exit(0); 
}



int distant(point p, point q)
{
   return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y));
}



int minvalue(int p, int q, int r)
{
  int i;

  if (p <= q && p <= r) {
    i = p;
  }
  else if (q <= p && q <= r) {
    i = q;
  }
  else if (r <= p && r <= q) {
    i = r;
  }

  return i;
}
