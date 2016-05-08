#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BLOCKSIZE 256

struct rect_list_t {struct rect_list_t *next;
          int x_min; int x_max; int y_min; int y_max;};

struct seg_tree_2d_t {int key;
          struct seg_tree_2d_t *left;
          struct seg_tree_2d_t *right;
          struct seg_tree_2d_t *v_tree;
          struct rect_list_t *rect_interval_list;};

struct seg_tree_2d_t *currentblock = NULL;
int    size_left;
struct seg_tree_2d_t *free_list = NULL;

struct intv {
  int x_min; int x_max;
  int y_min; int y_max;
};
struct intv rect_intv[1000000];
struct intv rect_intv_y[1000000];
int rect_total_number;
int rect_total_number_y;

struct seg_tree_2d_t *get_node()
{ struct seg_tree_2d_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock =
                (struct seg_tree_2d_t *) malloc( BLOCKSIZE * sizeof(struct seg_tree_2d_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}

void return_node(struct seg_tree_2d_t *node)
{  node->left = free_list;
   free_list = node;
}

struct rect_list_t *get_rect_list_node()
{  return( ( struct rect_list_t *) get_node() );
}


struct seg_tree_2d_t *make_tree(struct seg_tree_2d_t *list)
{  typedef struct { struct seg_tree_2d_t  *node1;
                    struct seg_tree_2d_t  *node2;
                    int          number; } st_item;
   st_item current, left, right;
   st_item stack[1000]; int st_p = 0;
   struct seg_tree_2d_t *tmp, *root;
   int length = 0;
   for( tmp = list; tmp != NULL; tmp = tmp->right )
      length += 1; /* find length of list */

   root = get_node();
   current.node1 = root; /* put root node on stack */
   current.node2 = NULL;
   current.number = length;/* root expands to length leaves */
   stack[ st_p++ ] = current;
   while( st_p >0 )/* there is still unexpanded node */
   {  current = stack[ --st_p ];
      if( current.number > 1 ) /* create (empty) tree nodes */
      { left.node1 = get_node();
        left.node2 = current.node2;
        left.number = current.number / 2;
        right.node1 = get_node();
        right.node2 = current.node1;
        right.number = current.number - left.number;
        (current.node1)->left  = left.node1;
        (current.node1)->right = right.node1;
        stack[ st_p++ ] = right;
        stack[ st_p++ ] = left;
      }
      else /* reached a leaf, must be filled with list item */
      { (current.node1)->left  = list->left;   /* fill leaf */
        (current.node1)->key   = list->key;    /* from list */
        (current.node1)->right = NULL;
        if( current.node2 != NULL )
           /* insert comparison key in interior node */
           (current.node2)->key   = list->key;
        tmp = list;          /* unlink first item from list */
        list = list->right;  /* content has been copied to */
        return_node(tmp);    /* leaf, so node is returned */
      }
   }
   return( root );
}

void empty_tree( struct seg_tree_2d_t *stree)
{  stree->rect_interval_list  = NULL;
   if( stree->right != NULL )
   {  empty_tree( stree->left );
      empty_tree( stree->right );
   }
}

void check_tree( struct seg_tree_2d_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  if( *( (int *) tr->left) == 42 )
         printf("%d(%d)  ", tr->key, depth );
      else
         printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key );
      check_tree(tr->right, depth+1, tr->key, upper );
   }
}

struct rect_list_t *find_intervals(struct seg_tree_2d_t *tree, int query_key)
{  struct seg_tree_2d_t *current_tree_node;
   struct rect_list_t *current_list, *result_list, *new_result;
   if (tree == NULL) {
      return NULL;
   }
   if( tree->left == NULL ) /* tree empty */
      return(NULL);
   else /* tree nonempty, follow search path */
   {  current_tree_node = tree;
      result_list = NULL;
      while( current_tree_node->right != NULL )
      {   if( query_key < current_tree_node->key )
              current_tree_node = current_tree_node->left;
          else
              current_tree_node = current_tree_node->right;
          current_list = current_tree_node->rect_interval_list;
          while( current_list != NULL )
          {  /* copy entry from node list to result list */
             new_result = get_rect_list_node();
             new_result->next = result_list;
             new_result->x_min = current_list->x_min;
             new_result->x_max = current_list->x_max;
             new_result->y_min = current_list->y_min;
             new_result->y_max = current_list->y_max;
             //new_result->object = current_list->object;
             result_list = new_result;
             current_list = current_list->next;
          }
      }
      return( result_list );
   }
}


// find_intervals in x seg tree
// find_intervals in y seg tree;
struct rect_list_t * query_seg_tree_2d( struct seg_tree_2d_t *tree, int x, int y)
{  struct seg_tree_2d_t *current_tree_node;
   struct rect_list_t *current_list, *result_list, *new_result;
   if (tree == NULL) {
      return NULL;
   }
   if( tree->left == NULL ) /* tree empty */
      return(NULL);
   else /* tree nonempty, follow search path */
   {  current_tree_node = tree;
      result_list = NULL;
      while( current_tree_node->right != NULL )
      {   if( x < current_tree_node->key )
              current_tree_node = current_tree_node->left;
          else
              current_tree_node = current_tree_node->right;
          current_list = current_tree_node->rect_interval_list;

          // search v_tree if current_list is not NULL;
          // if (current_list != NULL) {
          //   printf("%d,%d,%d,%d\n",current_list->x_min,current_list->x_max,
          //     current_list->y_min,current_list->y_max);
            struct rect_list_t *v_list = find_intervals(current_tree_node->v_tree,y);
            while( v_list != NULL )
            { /* copy entry from node list to result list */
               new_result = get_rect_list_node();
               new_result->next = result_list;
               // x and y change, get the right order ***************;
               new_result->x_min = v_list->y_min;
               new_result->x_max = v_list->y_max;
               new_result->y_min = v_list->x_min;
               new_result->y_max = v_list->x_max;
               //new_result->object = current_list->object;
               result_list = new_result;
               v_list = v_list->next;
            }

          }
        // printf("ha\n");
      }
      return( result_list );
}


void attach_intv_node(struct seg_tree_2d_t *tree_node,
                     int a, int b, int c, int d)
{  struct rect_list_t *new_node;
   new_node = get_rect_list_node();
   new_node->next = tree_node->rect_interval_list;
   new_node->x_min = a; new_node->x_max = b;
   new_node->y_min = c; new_node->y_max = d;
   //new_node->key_a = a; new_node->key_b = b;
   //new_node->object = object;
   tree_node->rect_interval_list = new_node;
}


void insert_interval(struct seg_tree_2d_t *tree,
                     int a, int b, int c, int d)
{  struct seg_tree_2d_t *current_node, *right_path, *left_path;
   struct rect_list_t *current_list, *new_node;
   if( tree->left == NULL )
      exit(-1); /* tree incorrect */
   else
   {  current_node = tree;
      right_path = left_path = NULL;
      while( current_node->right != NULL ) /* not at leaf */
      {   if( b < current_node->key ) /* go left: a < b < key */
              current_node = current_node->left;
          else if( current_node->key < a)
                                     /* go right: key < b < a */
              current_node = current_node->right;
          else if( a < current_node->key &&
               current_node->key < b )  /* split: a < key < b */
    {   right_path = current_node->right; /* both right */
        left_path  = current_node->left;    /* and left */
              break;
          }
          else if( a == current_node->key ) /* a = key < b */
    {   right_path = current_node->right; /* no left */
              break;
          }
          else /*   current_node->key == b, so a < key = b */
    {   left_path  = current_node->left; /* no right */
              break;
          }
      }
      if( left_path != NULL )
      {  /* now follow the path of the left endpoint a*/
         while( left_path->right != NULL )
         {   if( a < left_path->key )
             {   /* right node must be selected */
                 attach_intv_node(left_path->right, a,b,c,d);
                 left_path = left_path->left;
             }
             else if ( a == left_path->key )
             {   attach_intv_node(left_path->right, a,b,c,d);
                 break; /* no further descent necessary */
             }
       else /* go right, no node selected */
                 left_path = left_path->right;
         }
         /* left leaf needs to be selected if reached in descent*/
         if( left_path->right == NULL && left_path->key == a )
             attach_intv_node(left_path, a,b,c,d);
      }  /* end left path */
      if( right_path != NULL )
      {  /* and now follow the path of the right endpoint b */
         while( right_path->right != NULL )
         {   if( right_path->key < b )
             {   /* left node must be selected */
                 attach_intv_node(right_path->left, a,b,c,d);
                 right_path = right_path->right;
             }
             else if ( right_path->key == b)
             {   attach_intv_node(right_path->left, a,b,c,d);
                 break; /* no further descent necessary */
             }
       else /* go left, no node selected */
                 right_path = right_path->left;
         }
      /* on the right side, the leaf of b is never attached */
      }  /* end right path */
   }
}


int compint( const void *a, const void *b)
{  return ( *(int*)a - *(int*)b );
}


struct seg_tree_2d_t *rect_list_to_tree_list(struct rect_list_t *rect_list, int x) {
  int keys[1000000];
  int prev_key;
  int i = 0;
  int j,t;
  int *tmpob;
  struct rect_list_t *tmp_list = rect_list;
  struct seg_tree_2d_t *list, *tmp, *seg_tree;

  if (x == 1) {
    //build x seg tree
    while (tmp_list != NULL) {
      keys[2*i] = tmp_list->x_min;
      keys[2*i+1] = tmp_list->x_max;
      rect_intv[i].x_min = tmp_list->x_min;
      rect_intv[i].x_max = tmp_list->x_max;
      rect_intv[i].y_min = tmp_list->y_min;
      rect_intv[i].y_max = tmp_list->y_max;
      tmp_list = tmp_list->next;
      i++;
  }
  rect_total_number = i;
  printf("i = %d\n",i);

  }else {
    // rect_intv_y init to 0;
    for(t=0; t < rect_total_number_y; t++) {
      rect_intv_y[t].x_min = rect_intv_y[t].x_max = 0;
      rect_intv_y[t].y_min = rect_intv_y[t].y_max = 0;
    }

    //build y tree
  while (tmp_list != NULL) {
      keys[2*i] = tmp_list->y_min;
      keys[2*i+1] = tmp_list->y_max;
      // need to switch y and x
      rect_intv_y[i].x_min = tmp_list->y_min;
      rect_intv_y[i].x_max = tmp_list->y_max;
      rect_intv_y[i].y_min = tmp_list->x_min;
      rect_intv_y[i].y_max = tmp_list->x_max;
      tmp_list = tmp_list->next;
      i++;
  }
  rect_total_number_y = i;
  }
  // for( j = 0; j<i; j++ )
 //     printf("[%d,%d[ ", rect_intv[j].x_min, rect_intv[j].x_max );
 //   printf("\n");
   qsort(keys, 2*i, sizeof( int ), compint );
   // printf("  the keys, sorted in increasing order, are \n");
   // for( j = 0; j<2*i; j++ )
   //   printf("%d, ", keys[j] );
   // printf("\n");
   list = get_node();
   list->right = NULL;
   prev_key = list->key = keys[2*i-1];
   tmpob  = (int *) malloc(sizeof(int));
   *tmpob = 42;
   list->left = (struct seg_tree_2d_t *) tmpob;
   for( j = 2*i-2; j>= 0; j-- )
   {  if( keys[j] != prev_key )
      { tmp = get_node();
        prev_key = tmp->key = keys[j];
        tmp->right = list;
        tmpob  = (int *) malloc(sizeof(int));
        *tmpob = 42;
        tmp->left = (struct seg_tree_2d_t *) tmpob;
        list = tmp;
      }
   }
   tmp = get_node();
   tmp->key = -1000;
   tmp->right = list;
   tmpob  = (int *) malloc(sizeof(int));
   *tmpob = 42;
   tmp->left = (struct seg_tree_2d_t *) tmpob;
   list = tmp;
   //printf("Built sorted list from keys\n");
   tmp = list;
   // while (tmp != NULL )
   // { printf("%d ", tmp->key );
   //   tmp = tmp->right;
   // }
   // printf("\n");
   return list;
}


struct seg_tree_2d_t * create_seg_tree_2d( struct rect_list_t *rect_list){
  struct seg_tree_2d_t *list = rect_list_to_tree_list(rect_list,1);
  struct seg_tree_2d_t *v_list;
  struct seg_tree_2d_t *seg_tree = make_tree(list);
  empty_tree(seg_tree);
  //check_tree(seg_tree,0,-2000,1000);
  //printf("x seg tree build\n");

   struct seg_tree_2d_t *tmp_seg_tree;
   struct seg_tree_2d_t *stack[1000];

  struct rect_list_t *tmp_rect_list = rect_list;
  struct rect_list_t *current_list;
  int j;

  for(  j = rect_total_number-1; j>= 0; j-- )
    {//  struct intv *tmp_ob;
    //    tmp_ob = (struct intv *) malloc( sizeof( struct intv ) );  
    //    tmp_ob->low = intervals[j].low;
    //    tmp_ob->up = intervals[j].up;
       insert_interval( seg_tree, rect_intv[j].x_min, rect_intv[j].x_max, rect_intv[j].y_min, rect_intv[j].y_max);
    }
    //printf("Insert intervals to X tree done\n");

   //tmp_seg_tree = seg_tree;
   int st_p = 0;
   stack[st_p++] = seg_tree;
   while (st_p > 0) {
      struct seg_tree_2d_t *tmp_node = stack[--st_p];
      if (tmp_node->rect_interval_list != NULL) {
        //build y tree
        current_list = tmp_node->rect_interval_list;
        v_list = rect_list_to_tree_list(current_list, 0);
        tmp_node->v_tree = make_tree(v_list);
        empty_tree(tmp_node->v_tree);
        //insert y interval, remember x and y switched, so need to change the result;
        for(  j = rect_total_number_y-1; j>= 0; j-- )
      {//  struct intv *tmp_ob;
      //    tmp_ob = (struct intv *) malloc( sizeof( struct intv ) );  
      //    tmp_ob->low = intervals[j].low;
      //    tmp_ob->up = intervals[j].up;
         insert_interval( tmp_node->v_tree, rect_intv_y[j].x_min, rect_intv_y[j].x_max, rect_intv_y[j].y_min, rect_intv_y[j].y_max);
      }
      //printf("Insert intervals to y done\n");
      }
      // if (tmp_node->left != NULL) {
      //   stack[st_p++] = tmp_node->left;
      // }
      // if (tmp_node->right != NULL) {
      //   stack[st_p++] = tmp_node->right;
      // }
      //
      // if tmp_node->right == NULL, left value == 42, the root node don't need add left and right, if add left , 42 will add to the tree.
      if (tmp_node->right != NULL) {
        stack[st_p++] = tmp_node->left;
        stack[st_p++] = tmp_node->right;
      }
   }
   return seg_tree;
}

int main()
{  int i, j, x, y, l,m; 
   struct rect_list_t rectangles[50000];
   struct rect_list_t * tmp;
   struct seg_tree_2d_t *tr;
   for( i=0; i<50000; i++)
   {  rectangles[(17*i)%50000 ].next = rectangles + ((17*(i+1))%50000);  
   }
   rectangles[(17*49999)%50000 ].next = NULL;
   i=0; tmp = rectangles;
   while(tmp->next != NULL )
   {  tmp = tmp->next; i+=1; }
   printf("List of %d rectangles\n",i);
   for(i=0; i<12500; i++)
   {  rectangles[i].x_min = 500000 + 40*i;
      rectangles[i].x_max = 500000 + 40*i + 20;
      rectangles[i].y_min = 0;
      rectangles[i].y_max = 1000000;
   }
   for(i=12500; i<25000; i++)
   {  rectangles[i].x_min = 500000 + 40*(i-12500) + 10;
      rectangles[i].x_max = 500000 + 40*(i-12500) + 20;
      rectangles[i].y_min = 0;
      rectangles[i].y_max = 1000000;
   }
   for(i=25000; i<37500; i++)
   {  rectangles[i].x_min = 20*(i-25000);
      rectangles[i].x_max = 20*(i-25000) + 250000;
      rectangles[i].y_min = 20*(i-25000);
      rectangles[i].y_max = 20*(i-25000) + 250000;
   }
   for(i=37500; i<50000; i++)
   {  rectangles[i].x_min = 40*(i-37500);
      rectangles[i].x_max = 500000;
      rectangles[i].y_min = 300000;
      rectangles[i].y_max = 500000 + 40*(i-37500);
   }
   printf("Defined the 50000 rectangles\n"); fflush(stdout);
   tr = create_seg_tree_2d( rectangles );
   printf("Created 2d segment tree\n"); fflush(stdout);
   /* test 1 */
   for( i= 0; i<1000; i++ )
   {  x = 500000 + 400*i +30;
      y = ((1379*i)%400000) + 10;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp != NULL )
      {  printf("point %d,%d should not be covered by any rectangle.\n",x,y);
         printf(" instead reported as covered by [%d,%d]x[%d,%d]\n",
                tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max); 
         fflush(stdout);
         exit(0);
      }
   }
   printf("Passed Test 1.\n");
   /* test 2 */
   for( i= 0; i<1000; i++ )
   {  x = 500000 + 400*i +5;
      y = ((3791*i)%400000) + 10;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp == NULL )
      {  printf("point %d,%d should be covered by a rectangle, not found\n"
    ,x,y);fflush(stdout);
         exit(0);
      }
      if( x< tmp->x_min || x > tmp->x_max || y< tmp->y_min || y > tmp->y_max )
      {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
         fflush(stdout); 
         exit(0);
      } 
   }
   printf("Passed Test 2.\n");
   /* test 3 */
   for( i= 0; i<10; i++ )
   {  x = 300000 + 20000*i + 3;
      y = 400001;
      tmp = query_seg_tree_2d( tr, x,y);
      if( tmp == NULL )
      {  printf("point %d,%d should be covered by a rectangle, not found\n"
                 ,x,y);
         exit(0);
      }
      while( tmp != NULL )
      {  if( x< tmp->x_min|| x > tmp->x_max|| y< tmp->y_min|| y > tmp->y_max )
        {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
           exit(0);
        }
        tmp = tmp->next;
      }
   }
   printf("Passed Test 3.\n");
   /* test 4 */
   for( i= 0; i<10; i++ )
   {  x = 10* (rand()%100000) +4;
      y = 10* (rand()%100000) +4;
      m=0;
      for(j=0; j<50000; j++ )
      {  if( rectangles[j].x_min < x && rectangles[j].x_max > x &&
             rectangles[j].y_min < y && rectangles[j].y_max > y )
      m +=1;
      }
      tmp = query_seg_tree_2d( tr, x,y);
      l=0;
      while( tmp != NULL )
      {  if( x< tmp->x_min|| x > tmp->x_max|| y< tmp->y_min|| y > tmp->y_max )
        {  printf("rectangle [%d,%d]x[%d,%d] does not cover point %d,%d\n",
                 tmp->x_min, tmp->x_max, tmp->y_min, tmp->y_max,x,y); 
           exit(0);
        }
        l+= 1;
        tmp = tmp->next;
      }
      if( l!= m )
      {  printf("test 4.%d: Point %d,%d should be in %d rectangles, but %d found\n", i, x, y, m, l);
         fflush(stdout); 
      }     
   }
   printf("End of tests\n");
}