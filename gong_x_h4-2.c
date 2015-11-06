#include <stdlib.h>
/* compiles with command line  gcc xlibdemo.c -lX11 -lm -L/usr/X11R6/lib */
/* I change this command to gcc xlibdemo.c -lX11 -lm */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <math.h>
/*#include <limits.h> */


#define max(a,b) \
   ({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
     _a > _b ? _a : _b; })
#define min(a,b) \
   ({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
     _a < _b ? _a : _b; })

#define TRUE 1
#define FALSE 0

#define N 200
#define V 400 
#define INT_MAX	999999;



typedef struct {
  int x;
  int y;
}point;

typedef struct {
  point p;
  point q;
  int distance;
  int flag;
}segment;


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
unsigned long valuemask = 0;
XGCValues gc_values, gc_red_values, gc_yellow_values, gc_grey_values, gc_green_values, gc_blue_values;
Colormap color_map;
XColor tmp_color1, tmp_color2;

static int vsize;
static int hsize;

int distant(point p, point q);
int minKey(int key[N], int mstSet[N], int n);
int primMST(int G[V][V], int n);
int orientation(point p, point q, point r);
int intersect(point p1, point q1, point p2, point q2);
int onSegment(point p, point q, point r);

int parent[V];
int key[V];
int mstSet[V];
int GV[V][V];
int GH[V][V];
point varray[V];
point harray[V];
segment sv[V];
segment sh[V];
segment svk[V][V];
segment shk[V][V];


int G[V][V];
static int count;
static int press;
point array[N];


int main(int argc, char **argv)
{
  /* code add by andrew GONG for reading file from command line argument */
    
    point pv[N];
    point ph[N];

    point qv[N];
    point qh[N];
    

    int tmp_p1, tmp_p2, tmp_p3;
    char name[1];
    int i, j, k;
    int press;
    int sumv, sumh;
    int mstsum;
    int mysum;
    i = j = k = 0;
    press = 0;
    sumv = sumh = 0;

    for (i = 0; i < N; i++) {
      pv[i].x = ph[i].y = 0;
      qv[i].x = qh[i].y = 0;
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
	j = 0;
        printf("%s\n", argv[1]);
        while(!feof(file)) {
	  if (fscanf(file, "%c %d, %d, %d\n", name, &tmp_p1, &tmp_p2, &tmp_p3) != 4) {
	    break;
	  }
	  else {
	    if (name[0] == 'v') {
	        pv[i].x = tmp_p1;
		pv[i].y = tmp_p2;
		qv[i].x = tmp_p1;
		qv[i].y = tmp_p3;
		i++;
	    }
	    else {
	        ph[j].x = tmp_p2;
		ph[j].y = tmp_p1;
		qh[j].x = tmp_p3;
		qh[j].y = tmp_p1;
		j++;
	    } 
	  }
      }
      vsize = i;
      hsize = j;

      for (i; i < N; i++) {
	pv[i].x = qv[i].x = 0;
	pv[i].y = qv[i].y = 0;
      }
      for (j; j < N; j++) {
	ph[j].x = qh[j].x = 0;
	ph[j].y = qh[j].y = 0;
      }
     }
      
      fclose(file);
      printf("Please press the left botton to get the MST and myMST and also the ratio:\n");
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
  win_width = display_width/1.5; 
  win_height = (unsigned int) (win_width / 1.2); /* rectangular window */
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
  XSetLineAttributes(display_ptr, gc_red, 1, LineSolid, CapRound, JoinRound); 
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
  gc_green = XCreateGC(display_ptr, win, valuemask, &gc_green_values);
  XSetLineAttributes(display_ptr, gc_green, 4, LineSolid, CapRound, JoinRound); 
  if( XAllocNamedColor(display_ptr, color_map, "green",
			&tmp_color1, &tmp_color2) == 0)
    {printf("failed to get color green\n"); exit(-1);} 
  else
    XSetForeground(display_ptr, gc_green, tmp_color1.pixel);
  gc_blue = XCreateGC(display_ptr, win, valuemask, &gc_blue_values);
  XSetLineAttributes(display_ptr, gc_blue, 4, LineSolid, CapRound, JoinRound); 
  if( XAllocNamedColor(display_ptr, color_map, "blue",
			&tmp_color1, &tmp_color2) == 0)
    {printf("failed to get color blue\n"); exit(-1);} 
  else
    XSetForeground(display_ptr, gc_blue, tmp_color1.pixel);




  /* and now it starts: the event loop */
  while(1) { 
    XNextEvent( display_ptr, &report );
    switch(report.type)
    {
      case Expose:
      /* (re-)draw the example figure. This event happens
       * each time some part ofthe window gets exposed (becomes visible) */
      
    i = j = 0;
    for (i = 0; i < vsize; i++) {
	varray[i] = pv[i];
    }
    for (i = vsize; i < 2*vsize; i++) {
	varray[i] = qv[j];
	j++;
    }

    i = j = 0;
    for (j = 0; j < hsize; j++) {
	harray[j] = ph[j];
    }
    for (j = hsize; j < 2*hsize; j++) {
	harray[j] = qh[i];
	i++;
    }


    for (i = 0; i < V; i++ ) {
        for (j = 0; j < V; j++) {
	    GV[i][j] = GH[i][j] = INT_MAX;
			svk[i][j].p.x = 0;
			svk[i][j].p.y = 0;
			svk[i][j].q.x = 0;
			svk[i][j].q.y = 0; 
			svk[i][j].distance = 0;
			svk[i][j].flag = 0;

			shk[i][j].p.x = 0;
			shk[i][j].p.y = 0;
			shk[i][j].q.x = 0;
			shk[i][j].q.y = 0; 
			shk[i][j].distance = 0;
			shk[i][j].flag = 0;
	}
    }
  

    for (i = 0; i < vsize; i++) {
        sv[i].p = pv[i];
        sv[i].q = qv[i];
        sv[i].distance = distant(pv[i], qv[i]);
        sv[i].flag = 0;
    }
    for (i = 0; i < hsize; i++) {
        sh[i].p = ph[i];
        sh[i].q = qh[i];
        sh[i].distance = distant(ph[i], qh[i]);
        sh[i].flag = 0;
    }


    for (i = 0; i < vsize; i++) {
	for (j = 0; j < vsize; j++) {
	    for ( k = 0; k < hsize; k++) {
		if (i == j) {
		    GV[i][j] = 0;
		    break;
		}
		else {
		    if (intersect(pv[i], qv[i], ph[k], qh[k])) {
			if (intersect(pv[j], qv[j], ph[k], qh[k])) {
			GV[i][j] = abs(qv[i].x - qv[j].x); 			
			svk[i][j].p.x = pv[i].x;
			svk[i][j].p.y = ph[k].y;
			svk[i][j].q.x = qv[j].x;
			svk[i][j].q.y = ph[k].y; 
			svk[i][j].distance = GV[i][j];
			svk[i][j].flag = 0;
			}
		    }
		}
	    }
	}
    }


    for (i = 0; i < hsize; i++) {
	for (j = 0; j < hsize; j++) {
	    for (k = 0; k < vsize; k++) {
		if (i == j) {
		    GH[i][j] = 0;
		    break;
		}
		else {
		    if (intersect(ph[i], qh[i], pv[k], qv[k])) {
			if (intersect(ph[j], qh[j], pv[k], qv[k])) {
			GH[i][j] = abs(qh[i].y - qh[j].y); 			
			shk[i][j].p.x = pv[k].x;
			shk[i][j].p.y = ph[i].y;
			shk[i][j].q.x = pv[k].x;
			shk[i][j].q.y = qh[j].y; 
			shk[i][j].distance = GH[i][j];
			shk[i][j].flag = 0;
			}
		    }
		}
	    }
	}
    }





count = 0;
for (i = 0; i < vsize; i++) {
    for (j = 0; j < hsize; j++) {
	if (intersect(pv[i], qv[i], ph[j], qh[j])) {
	    array[count].x = pv[i].x;
	    array[count].y = ph[j].y;    
	    count++;
	}
    }
}


for (i = 0; i < V; i++) {
    for (j = 0; j < V; j++) {
	G[i][j] = INT_MAX;
    }
}
for (i = 0; i < count; i++) {
    for (j = 0; j < count; j++) {
	if (i == j) {
	    G[i][j] = 0;
	}
	if (i != j) {
	   if (array[i].x != array[j].x && array[i].y != array[j].y) {
		G[i][j] = INT_MAX;
	   }
	   else {
		if (array[i].x == array[j].x) {
		    for (k = 0; k < vsize; k++) {
			if (array[i].x == pv[k].x && min(array[i].y, array[j].y) >= min(pv[k].y, qv[k].y) &&
			    max(array[i].y, array[j].y) <= max(pv[k].y, qv[k].y)) {
			    G[i][j] = abs(array[i].y - array[j].y);	
			}
		    }
		}
		else if (array[i].y == array[j].y) {
		    for (k = 0; k < hsize; k++) {
			if (array[i].y == ph[k].y && min(array[i].x, array[j].x) >= min(ph[k].x, qh[k].x) &&
			    max(array[i].x, array[j].x) <= max(ph[k].x, qh[k].x)) {
			    G[i][j] = abs(array[i].x - array[j].x);
			}
		    }
		}
	   }
	}
    }
}
      /* print all the obstacles */
      for (i = 0; i < vsize; i++) {
          XDrawLine(display_ptr, win, gc, pv[i].x, pv[i].y, qv[i].x, qv[i].y);
      }

      for (i = 0; i < hsize; i++) {
          XDrawLine(display_ptr, win, gc, ph[i].x, ph[i].y, qh[i].x, qh[i].y);
      }

   if (vsize != 0) {
    sumv = primMST(GV, vsize);

    i = 1;
    while(1) {
      XDrawLine(display_ptr, win, gc_blue, svk[i][parent[i]].p.x, svk[i][parent[i]].p.y, svk[i][parent[i]].q.x, svk[i][parent[i]].q.y);
      XFlush(display_ptr);
      i++;
      if (i == vsize) {
	break;
      }
    }
   }

   if (hsize != 0) {
    sumh = primMST(GH, hsize);

    i = 1;
    while(1) {
      XDrawLine(display_ptr, win, gc_green, shk[i][parent[i]].p.x, shk[i][parent[i]].p.y, shk[i][parent[i]].q.x, shk[i][parent[i]].q.y);
      XFlush(display_ptr);
      i++;
      if (i == hsize) {
	break;
      }
    }
   }

    mstsum = sumv + sumh;

    mysum = primMST(G, count);
    i = 1;
    while(1) {
      XDrawLine(display_ptr, win, gc_red, array[i].x, array[i].y, array[parent[i]].x, array[parent[i]].y);
      XFlush(display_ptr);
      i++;
      if (i == count) {
	break;
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
     
      press = press + 1; 
      if (report.xbutton.button == Button1)
	    XFillArc(display_ptr, win, gc_red,
        x - win_height/80, y - win_height/80,
        win_height / 80, win_height / 80, 0, 360*64);
      else
	    XFillArc(display_ptr, win, gc_yellow,
        x - win_height / 70, y - win_height / 70,
        win_height / 40, win_height / 40, 0, 360*64);
      
    }
      if (press == 1) {	
        printf("total distance of MST: %d\n", mstsum);
        printf("my solution %d\n", mysum);
        printf("ratio %f\n", (float)mysum/mstsum);
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


int minKey(int key[N], int mstSet[N], int n)
{
    int min, min_index;
    int v;
    min = INT_MAX;

    for (v = 0; v < n; v++) {
	if (mstSet[v] == FALSE && key[v] < min) {
	    min = key[v];
	    min_index = v;
	}
    } 

    return min_index;
}


int primMST(int G[V][V], int n)
{
    int i, j, k, u, v;
    int sum;


    for ( i = 0; i < n; i++) {
	key[i] = INT_MAX;
	mstSet[i] = FALSE;
	parent[i] = -1;
    }

    key[0] = 0;
    parent[0] = -1;

    for (k = 0; k < n - 1; k++) {
	int u = minKey(key, mstSet, n);

	mstSet[u] = TRUE;

	for ( v = 0; v < n; v++) {
	    if (G[u][v] && mstSet[v] == FALSE && G[u][v] < key[v]) {
		parent[v] = u;
		key[v] = G[u][v];
	    }
	}
    }

    sum = 0; 
    for ( i = 1; i < n; i++) {
/*	  printf("%d - %d 	%d\n", parent[i], i, G[i][parent[i]]); */
	  sum = sum + G[i][parent[i]]; 
    }

    return sum;
}

int onSegment(point p, point q, point r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return TRUE;
 
    return FALSE;
}


int orientation(point p, point q, point r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);
 
    if (val == 0) return 0;  // colinear
 
    return (val > 0)? 1: 2; // clock or counterclock wise
}

int intersect(point p1, point q1, point p2, point q2)
{
    // Find the four orientations needed for general and
    // special cases
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
 
    // General case
    if (o1 != o2 && o3 != o4)
        return TRUE;
 
    // Special Cases
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return TRUE;
 
    // p1, q1 and p2 are colinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return TRUE;
 
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return TRUE;
 
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return TRUE;
 
    return FALSE; // Doesn't fall in any of the above cases
}
