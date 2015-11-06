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



#define RED	0 
#define GREEN	1
#define YELLOW	2
#define TRUE	1
#define FALSE	0

#define N 19600	/* array size at most 700/5 = 140, 140 * 140 */
#define B 5	/* smallest yellow number */
#define V 1200
#define MAX(a, b)	((a) > (b))?(a):(b)
#define MIN(a, b)	((a) < (b))?(a):(b)



typedef struct {
  int x;
  int y;
}point;


/* p1 top left, p3 down right clock wise p1, p2, p3, p4 */
typedef struct {
  point p1;	
  point p2;
  point p3;
  point p4;
  int   color;
}rectangle;

rectangle A[N];		/* for the final divided blocks */
rectangle R[N];		/* final green square */
rectangle disc;
static int ac;		/* to count A[N] */
static int svalue;	/* smallest divide value */

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

GC gc,  gc_red, gc_yellow, gc_grey, gc_green;
unsigned long valuemask = 0;
XGCValues gc_values, gc_red_values, gc_yellow_values, gc_grey_values, gc_green_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;


int d;   /* radius */
int c, f;    /* these two variables are for adjacent array start point 0, and end point n */
int adjacentsize;
point s = {0, 0}; /* center */
point t = {0, 0}; /* radius */
point r = {0, 0}; /* destination point */
int divide(point pq, point qp);
int distant(point p, point q);
int intersect(rectangle R1, rectangle R2);
int coline(rectangle R3, rectangle R4);
int reccontain(rectangle R5, rectangle R6);
int mindistant(int dist[], int sptset[]);
int dijkstra(unsigned int G[V][V], int n, point Gpoint[]);


int main(int argc, char **argv)
{
  /* code add by andrew GONG for reading file from command line argument */
    
    point p[N];
    point q[N];
    point Gpoint[N];
    unsigned int adjacent[V][V];
    rectangle array[N];	/* this array is for rectangle obstacles */

    point temp1 = {0, 0};
    point temp2 = {700, 700};
    int i, j, k, m;
    int t1, t2, t3, t4;
    int press;
    int split;	/* for divide */
    int nrec;	/* obstacle rectangles' number */
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
	  if(fscanf(file, "R (%d, %d) (%d, %d)\n", 
		    &p[i].x, &p[i].y, &q[i].x, &q[i].y) != 4) {
	    break;
	  }
	  else {
	    array[i].p1.x = p[i].x;
	    array[i].p1.y = p[i].y;
	    array[i].p2.x = q[i].x;
	    array[i].p2.y = p[i].y;
	    array[i].p3.x = q[i].x;
	    array[i].p3.y = q[i].y;
	    array[i].p4.x = p[i].x;
	    array[i].p4.y = q[i].y;
	    array[i].color = YELLOW;
	    i++;
	  }
	  nrec = i;
	}
      }
      
      fclose(file);
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
  gc_red = XCreateGC( display_ptr, win, valuemask, &gc_red_values);
 /* XSetLineAttributes( display_ptr, gc_red, 6, LineSolid, CapRound, JoinRound); */
  if( XAllocNamedColor( display_ptr, color_map, "red", 
			&tmp_color1, &tmp_color2 ) == 0 )
    {printf("failed to get color red\n"); exit(-1);} 
  else
    XSetForeground( display_ptr, gc_red, tmp_color1.pixel );
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

  /* and now it starts: the event loop */
  while(1) { 
    XNextEvent( display_ptr, &report );
    switch( report.type ) {
    case Expose:
      /* (re-)draw the example figure. This event happens
	 each time some part ofthe window gets exposed (becomes visible) */
      XDrawRectangle(display_ptr, win, gc_green, 0, 0, 700, 700);
      
      /* print all the obstacles */
      for ( i = 0; i < nrec; i++ ) {
	  XDrawLine(display_ptr, win, gc, p[i].x, p[i].y, q[i].x, q[i].y );
	  XDrawRectangle(display_ptr, win, gc, p[i].x, p[i].y, q[i].x - p[i].x, q[i].y - p[i].y);
	  XFillRectangle(display_ptr, win, gc, p[i].x, p[i].y, q[i].x - p[i].x, q[i].y - p[i].y);
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
        if ( report.xbutton.button == Button1 )
        	XFillArc( display_ptr, win, gc_red, 
			  x - win_height / 100, y - win_height / 100,
			  win_height / 80, win_height / 80, 0, 360*64 );
        else
		XFillArc( display_ptr, win, gc_yellow, 
			  x - win_height / 70, y - win_height / 70,
			  win_height / 40, win_height / 40, 0, 360*64 );
      }
      else if (press == 1) {
	t.x = report.xbutton.x;
	t.y = report.xbutton.y;
	d = sqrt((s.x - t.x) * (s.x -t.x) + (s.y - t.y) * (s.y - t.y));
	disc.p1.x = s.x - d;
	disc.p1.y = s.y - d;
	disc.p2.x = s.x + d;
	disc.p2.y = s.y - d;
	disc.p3.x = s.x + d;
	disc.p3.y = s.y + d;
	disc.p4.x = s.x - d;
	disc.p4.y = s.y + d;
	for (i = 0; i < nrec; i++) {
  	  if ((array[i].p1.y - disc.p4.y >= 0) || (disc.p1.x - array[i].p2.x >= 0) || (disc.p1.y - array[i].p4.y >= 0) || (array[i].p1.x - disc.p3.x >= 0)) {
	    ;
	  }
	  else {
	    printf("\n\n\n !!!!robot's can't intersect with obstalces, please reconstruct the disc robot\n");
	    printf("\n\n\n");
	    exit(0);
	  }
	}
	XDrawArc(display_ptr, win, gc_green, s.x - d, s.y - d,
		 2 * d, 2 * d, 0, 360*64); 
	XFillArc(display_ptr, win, gc_grey, s.x - d, s.y - d,
		 2 * d, 2 * d, 0, 360*64); 
	
	XDrawRectangle(display_ptr, win, gc_red, 0, 0, 700, d);
	XFillRectangle(display_ptr, win, gc_red, 0, 0, 700, d);
	XDrawRectangle(display_ptr, win, gc_red, 0, 0, d, 700);
	XFillRectangle(display_ptr, win, gc_red, 0, 0, d, 700);
	XDrawRectangle(display_ptr, win, gc_red, 0, 700 - d, 700, d);
	XFillRectangle(display_ptr, win, gc_red, 0, 700 - d, 700, d);
	XDrawRectangle(display_ptr, win, gc_red, 700 - d, 0, d, 700);
	XFillRectangle(display_ptr, win, gc_red, 700 - d, 0, d, 700); 
      }
      else if (press == 2) {
        r.x = report.xbutton.x;
		r.y = report.xbutton.y; 
        if ( report.xbutton.button == Button1 )
        	XFillArc( display_ptr, win, gc_grey, 
			  x - win_height/70, y - win_height/70,
			  win_height / 40, win_height / 40, 0, 360*64 );
        else
		XFillArc( display_ptr, win, gc_yellow, 
			  x - win_height / 70, y - win_height / 70,
			  win_height / 40, win_height / 40, 0, 360*64 );
	
      }
      
      press += 1;
      
      
      printf("%d, %d, %d, %d, %d, %d\n", s.x, s.y, t.x, t.y, r.x, r.y);
      printf("press = %d\n", press);
      
      if (press == 3) {
	XFlush(display_ptr);
	split = 700;
  	ac = 0; 
	svalue = MAX(d, B);

/* this is the most difficult part */
	divide(temp1, temp2);
	j = 4;
	split = split / 2;
	i = 0;
	while (split > svalue) {
	    for (i; i < j && A[i].p2.x - A[i].p1.x == split; i++) { 
	        for ( m = 0; m < nrec; m++) {
	          if (TRUE == intersect(A[i], array[m])) {
		    divide(A[i].p1, A[i].p3);
		    j = j + 4;
		    break;
		  }
	        }
	    }

	    split = split / 2;
	}


/* why k = 3? because divide all the red square 3 times will enough for find the path */
	k = 3;
   	do {
	for (i = 0; i < ac; i++) {
	  for ( j = 0; j < nrec; j++) {
	    if (TRUE == intersect(A[i], array[j])) {
	      A[i].color = RED;
	      break;
	    }
	    else if (FALSE == intersect(A[i], array[j])) {
	      A[i].color = GREEN;	
	    }  
          }
	}

	for (i = 0; i < ac; i++) {
	  for (j = 0; j < nrec; j++) {
	    if (A[i].color == RED && A[i].p2.x - A[i].p1.x >= d && FALSE == reccontain(A[i], array[j])) {
	      if (j == nrec - 1) {
	        divide(A[i].p1, A[i].p3);
	      }
	      else {
		continue;
  	      }
	    }
	    else if (A[i].color == RED && A[i].p2.x - A[i].p1.x >= d && TRUE == reccontain(A[i], array[j])) {
		break;
	    }
	  }
	}
	k--;
	}while (k > 0);

	for (i = 0; i < ac; i++) {
	  for ( j = 0; j < nrec; j++) {
	    if (1 == intersect(A[i], array[j])) {
	      A[i].color = RED;
	      break;
	    }
	    else if (0 == intersect(A[i], array[j])) {
	      A[i].color = GREEN;	
	    }  
          }
	}

	/* this function remove the green sqare next to the edge of 700 * 700 */
	for (i = 0; i < ac; i++) {
	  if (A[i].color == GREEN && (A[i].p2.x < d || A[i].p3.y < d || A[i].p1.x > 700 - d || A[i].p1.y > 700 - d))
	   A[i].color = RED;
	}

	
	for (i = 0; i < ac; i++) {
          if (A[i].color == RED) {
	    XDrawRectangle(display_ptr, win, gc_red, A[i].p1.x, A[i].p1.y, A[i].p3.x - A[i].p1.x, A[i].p3.y - A[i].p1.y);
	  /*  XFillRectangle(display_ptr, win, gc_red, A[i].p1.x, A[i].p1.y, A[i].p3.x - A[i].p1.x, A[i].p3.y - A[i].p1.y); */
	    XFlush(display_ptr);
	  }
	  else if (A[i].color == GREEN) {
	    XDrawRectangle(display_ptr, win, gc_green, A[i].p1.x, A[i].p1.y, A[i].p3.x - A[i].p1.x, A[i].p3.y - A[i].p1.y);
/*	    XFillRectangle(display_ptr, win, gc_green, A[i].p1.x, A[i].p1.y, A[i].p3.x - A[i].p1.x, A[i].p3.y - A[i].p1.y); */ 
	    XFlush(display_ptr);

	  }	
        } 

    k = 0;
    for (i = 0; i < ac; i++) {  
      if (A[i].color == GREEN) {
	    R[k] = A[i];
	    k++;
	  }
    }
    adjacentsize = k;



	for ( i = 0; i < adjacentsize; i++) {
	  if (R[i].p1.x <= s.x && R[i].p1.y <= s.y && R[i].p2.x >= s.x && R[i].p2.y <= s.y &&
	      R[i].p3.x >= s.x && R[i].p3.y >= s.y && R[i].p4.x <= s.x && R[i].p4.y >= s.y) {
		c = i;
	  }
	  else { 
	  	if (i == adjacentsize -1 && c == 0) {	/* always remember the different between = and == */
		printf("No path go there, You need to choose another point for radius\n");
		exit (0);
		}
	  }
	}


	for (i = 0; i < adjacentsize; i++) {
	  if (R[i].p1.x <= r.x && R[i].p1.y <= r.y && R[i].p2.x >= r.x && R[i].p2.y <= r.y &&
	      R[i].p3.x >= r.x && R[i].p3.y >= r.y && R[i].p4.x <= r.x && R[i].p4.y >= r.y) {
		f = i;
	  }
	  else { 
	  	if (i == adjacentsize - 1 && f == 0) {
		printf("You should choose another target point\n");
		exit (0);
		}
	  }
	}


	for (i = 0; i < adjacentsize; i++) {
		Gpoint[i].x = R[i].p1.x + (R[i].p3.x - R[i].p1.x) / 2;
		Gpoint[i].y = R[i].p1.y + (R[i].p3.y - R[i].p1.y) / 2;
	}


	for (i = 0; i <= adjacentsize + 1; i++) {
	  for (j = 0; j <= adjacentsize + 1; j++) {
	  	if (i == j) {
	  		adjacent[i][j] = 0;
	  	}
	  	else {
	  		adjacent[i][j] = INT_MAX;
	  	}
	  	
	  }
	}
/*	
	adjacent[0][0] = 0;
	adjacent[adjacentsize + 1][adjacentsize + 1] = 0;
*/
	    t3 = R[c].p1.x + (R[c].p3.x - R[c].p1.x) / 2;
	    t4 = R[c].p1.y + (R[c].p3.y - R[c].p1.y) / 2;
	    adjacent[0][c + 1] = adjacent[c + 1][0] = sqrt((t3 - s.x) * (t3 - s.x) + (t4 - s.y) * (t4 - s.y)); 


	    t1 = R[f].p1.x + (R[f].p3.x - R[f].p1.x) / 2;
	    t2 = R[f].p1.y + (R[f].p3.y - R[f].p1.y) / 2;
	    adjacent[adjacentsize + 1][f + 1] = adjacent[f + 1][adjacentsize + 1] = sqrt((t1 - s.x) * (t1 - s.x) + (t2 - s.y) * (t2 - s.y)); 

	for (i = 1; i <= adjacentsize; i++) {
	  for (j = 1; j <= adjacentsize; j++) {
	    if (i != j && 1 == coline(R[i - 1], R[j - 1])) {
	       t1 = R[i - 1].p1.x + (R[i - 1].p3.x - R[i - 1].p1.x) / 2;
	       t2 = R[i - 1].p1.y + (R[i - 1].p3.y - R[i - 1].p1.y) / 2;
	       t3 = R[j - 1].p1.x + (R[j - 1].p3.x - R[j - 1].p1.x) / 2;
	       t4 = R[j - 1].p1.y + (R[j - 1].p3.y - R[j - 1].p1.y) / 2;
	       adjacent[i][j] = sqrt((t3 - t1) * (t3 - t1) + (t4 - t2) * (t4 - t2));
	    }
	  }	
	}


	dijkstra(adjacent, 0, Gpoint);

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


int divide(point pq, point qp)
{
  A[ac].p1.x = pq.x;
  A[ac].p1.y = pq.y;
  A[ac].p2.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p2.y = pq.y;
  A[ac].p3.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p3.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p4.x = pq.x;
  A[ac].p4.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].color = YELLOW;
  ac++;

  A[ac].p1.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p1.y = pq.y;
  A[ac].p2.x = qp.x;
  A[ac].p2.y = pq.y;
  A[ac].p3.x = qp.x;
  A[ac].p3.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p4.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p4.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].color = YELLOW;
  ac++;

  A[ac].p1.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p1.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p2.x = qp.x;
  A[ac].p2.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p3.x = qp.x;
  A[ac].p3.y = qp.y;
  A[ac].p4.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p4.y = qp.y;
  A[ac].color = YELLOW;
  ac++;

  A[ac].p1.x = pq.x;
  A[ac].p1.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p2.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p2.y = pq.y + (qp.y - pq.y) / 2;
  A[ac].p3.x = pq.x + (qp.x - pq.x) / 2;
  A[ac].p3.y = qp.y;
  A[ac].p4.x = pq.x;
  A[ac].p4.y = qp.y;
  A[ac].color = YELLOW;
  ac++;

  return 0;
}

/* R2 obstacles, TRUE intersect, FALSE on-intersect */
int intersect(rectangle R1, rectangle R2)
{
  int i;
  
  if ((R2.p1.y - R1.p4.y >= svalue) || (R1.p1.x - R2.p2.x >= svalue) || (R1.p1.y - R2.p4.y >= svalue) || (R2.p1.x - R1.p3.x >= svalue)) {
    i = FALSE;
  }
  else {
    i = TRUE;
  }

  return i;
}

int distant(point p, point q)
{
  return (sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y) * (p.y - q.y)));
}



int coline(rectangle R3, rectangle R4)
{
  int i;
  if ((R3.p1.y == R4.p4.y && R4.p3.x >= R3.p1.x && R4.p4.x <= R3.p2.x) ||
      (R3.p4.y == R4.p1.y && R4.p3.x >= R3.p1.x && R4.p4.x <= R3.p2.x) ||
      (R3.p2.x == R4.p1.x && R4.p3.y >= R3.p1.y && R4.p1.y <= R3.p3.y) ||
      (R3.p1.x == R4.p2.x && R4.p3.y >= R3.p1.y && R4.p1.y <= R3.p3.y)) {

    i = 1;
  }
  else {
    i = 0;
  }

  return i;
}


int mindistant(int dist[], int sptset[])
{
  int min = INT_MAX;
  int min_index;
  
  int v;
  for ( v = 0; v < adjacentsize + 1; v++ ) {
    if ( sptset[v] == FALSE && dist[v] <= min ) {
      min = dist[v];
      min_index = v;
    }
  }
  
  return min_index;
}


int dijkstra(unsigned int G[V][V], int n, point Gpoint[])
{
  int dist[V];
  int sptset[V];
  int parent[V];
  
  int i, v;
  
  for ( i = 0; i < adjacentsize + 2; i++ ) {
    dist[i] = INT_MAX;
    sptset[i] = FALSE;
    parent[i] = 0;
  }
  
  dist[n] = 0;
  
  for ( i = 0; i < adjacentsize - 1; i++ ) {
    int u = mindistant(dist, sptset);
    sptset[u]=TRUE;
    
    for (v = 0; v < adjacentsize + 2; v++ ) {
      if ( !sptset[v] && G[u][v] && dist[u] != INT_MAX && dist[u] + G[u][v] < dist[v] ) {
        dist[v] = dist[u] + G[u][v];
        parent[v] = u;
      }
    }
  }
  
  printf("shortest path from s to t is %d\n", dist[adjacentsize-1]);
  
  
  XDrawLine(display_ptr, win, gc, Gpoint[c].x, Gpoint[c].y, s.x, s.y);
  XDrawArc(display_ptr, win, gc_green, Gpoint[c].x - d, Gpoint[c].y - d, 2 * d, 2 * d, 0, 360 * 64);
  XFillArc(display_ptr, win, gc_yellow, Gpoint[c].x - d, Gpoint[c].y - d, 2 * d, 2 * d, 0, 360 * 64);
  XDrawLine(display_ptr, win, gc, Gpoint[f].x, Gpoint[f].y, r.x, r.y);
  XDrawArc(display_ptr, win, gc_green, Gpoint[f].x - d, Gpoint[f].y - d, 2 * d, 2 * d, 0, 360 * 64);
  XFillArc(display_ptr, win, gc_yellow, Gpoint[f].x - d, Gpoint[f].y - d, 2 * d, 2 * d, 0, 360 * 64);
  XDrawArc(display_ptr, win, gc_green, r.x - d, r.y - d, 2 * d, 2 * d, 0, 360 * 64);
  XFillArc(display_ptr, win, gc_yellow, r.x - d, r.y - d, 2 * d, 2 * d, 0, 360 * 64);
  i = parent[adjacentsize + 1];
  do {
    XDrawLine(display_ptr, win, gc, Gpoint[i - 1].x, Gpoint[i - 1].y, Gpoint[parent[i] - 1].x, Gpoint[parent[i] - 1].y);
    XDrawArc(display_ptr, win, gc_green, Gpoint[i - 1].x - d, Gpoint[i - 1].y - d, 2 * d, 2 * d, 0, 360*64);
    XFillArc(display_ptr, win, gc_yellow, Gpoint[i - 1].x - d, Gpoint[i - 1].y - d, 2 * d, 2 * d, 0, 360*64);
    i = parent[i];
    XFlush(display_ptr);
  } while(i != c + 1);
  return dist[v];
}


/* R6 contains R5, TRUE contains, FALSE don't contain */
int reccontain(rectangle R5, rectangle R6)
{
    int i;
    if (R5.p1.x >= R6.p1.x && R5.p1.y >= R6.p1.y &&
	R5.p2.x <= R6.p2.x && R5.p2.y >= R6.p2.y &&
	R5.p3.x <= R6.p3.x && R5.p3.y <= R6.p3.y &&
	R5.p4.x >= R6.p4.x && R5.p4.y <= R6.p4.y) {
	
	i = TRUE;
    }
    else {
	i = FALSE;
    }

    return i;
}

