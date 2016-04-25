#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;
typedef struct tr_n_t {
	key_t key;
	struct tr_n_t *left;
	struct tr_n_t *right;
	struct tr_n_t *back;
	int height;
} o_t;

o_t *currentblock = NULL;
int size_left;
o_t *free_list = NULL;

o_t *get_node()
{ o_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = 
                (o_t *) malloc( BLOCKSIZE * sizeof(o_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}

void return_node(o_t *node)
{  node->left = free_list;
   free_list = node;
}

o_t *create_tree(void)
{  o_t *tmp_node;
   tmp_node = get_node();
   tmp_node->left = NULL;
   return( tmp_node );
}

void left_rotation(o_t *n)
{  o_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->left; 
   tmp_key  = n->key;

   n->left  = n->right;        
   n->key   = n->right->key;
   n->left->back = n;
   n->right = n->left->right;
   n->right->back = n;

   n->left->right = n->left->left;
   n->left->right->back = n->left;
   n->left->left  = tmp_node;
   n->left->key   = tmp_key;
   tmp_node->back = n->left;
}

void right_rotation(o_t *n)
{  o_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->right; 
   tmp_key  = n->key;

   n->right = n->left;        
   n->key   = n->left->key;
   n->right->back = n;
   n->left  = n->right->left;
   n->left->back = n;

   n->right->left = n->right->right;
   n->right->left->back = n->right;
   n->right->right  = tmp_node;
   n->right->key   = tmp_key;
   tmp_node->back = n->right;
}

object_t *find(o_t *tree, key_t query_key)
{  o_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}

// create two tree,
o_t * create_order() {
  o_t *root;
  o_t *root2;
  root = get_node();
  root2 = get_node();
  root2->key = 0;
  root2->back = NULL;
  root2->left = NULL;
  root2->right = NULL;
  root->left = NULL;
  root->right = NULL;
  root->back = root2;
  root->key = 0;
  return root;
}


void insert_before(o_t *ord, key_t a, key_t b) {
    int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;
    //find b first
    o_t *tmp_node;
    o_t *ord2, *cur;
    o_t *path_stack[100], *path_stack2[100], *tmp_path_stack2[100];
    int path_st_p = 0;
    int path_st_p2 = 0;
    int tmp_path2_index = 0;
    if (ord->left == NULL) {
      return;
    }else {
      tmp_node = ord;
      while (tmp_node->right != NULL) {
        if (b < tmp_node->key) {
          tmp_node = tmp_node->left;
        }else {
          tmp_node = tmp_node->right;
        }
      }//find the insert node in tree1

      if (tmp_node->key == b) {
        ord2 = tmp_node->back;
        while (ord2->back != NULL) {
          tmp_path_stack2[tmp_path2_index++] = ord2;
          ord2 = ord2->back;
        }
        //reverse stack;
        while(tmp_path2_index >= 0) {
          path_stack2[path_st_p2++] = tmp_path_stack2[tmp_path2_index--];
        }

        //insert a before b
        cur = tmp_node->back;
        o_t *old_leaf, *new_leaf;
        o_t *old_leaf2, *new_leaf2;
        int finished, finished2;

        {
          old_leaf2 = get_node();
          old_leaf2->left = cur->left;
          old_leaf2->key = cur->key;
          old_leaf2->right = NULL;
          old_leaf2->height = 0;
          new_leaf2 = get_node();
          new_leaf2->left = (o_t *) insobj2;
          new_leaf2->key = a;
          new_leaf2->right = NULL;
          new_leaf2->height = 0;

          cur->left = new_leaf2;
          new_leaf2->back = cur;
          cur->right = old_leaf2;
          old_leaf2->back = cur;
          cur->height = 1;          
        }//finish insert a to tree2;
        {
          //find node a position first;
          tmp_node = ord;
          while (tmp_node->right != NULL) {
            if (a < tmp_node->key) {
              tmp_node = tmp_node->left;
            }else {
              tmp_node = tmp_node->right;
            }
          }//find the insert node in tree1
          //insert ord;
             old_leaf = get_node();
             old_leaf->left = tmp_node->left; 
             old_leaf->key = tmp_node->key;
             old_leaf->right  = NULL;
             old_leaf->height = 0;
             new_leaf = get_node();
             new_leaf->left = (o_t *) insobj; 
             new_leaf->key = a;
             new_leaf->right  = NULL;
             new_leaf->height = 0;
             if( tmp_node->key < a )
             {   tmp_node->left  = old_leaf;
                 tmp_node->right = new_leaf;
                 tmp_node->key = a;
                 tmp_node->back = NULL;
                 tmp_node->right->back = cur->left;
             } 
             else
             {   tmp_node->left  = new_leaf;
                 tmp_node->right = old_leaf;
                 tmp_node->back = NULL;
                 tmp_node->left->back = cur->left;
             } 
             tmp_node->height = 1;


        }//finish insert a to tree1;

        //start rebalance
            //rebalance
            finished = 0;
            while( path_st_p > 0 && !finished )
            {  int tmp_height, old_height;
               o_t *tmp_node;
               tmp_node = path_stack[--path_st_p];
               old_height= tmp_node->height;
               if( tmp_node->left->height - 
                                       tmp_node->right->height == 2 )
               {  if( tmp_node->left->left->height - 
                                       tmp_node->right->height == 1 )
                  {  right_rotation( tmp_node );
                     tmp_node->right->height = 
                                  tmp_node->right->left->height + 1;
                     tmp_node->height = tmp_node->right->height + 1;
                  }
                  else
                  {  left_rotation( tmp_node->left );
                     right_rotation( tmp_node );
                     tmp_height = tmp_node->left->left->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else if ( tmp_node->left->height - 
                                      tmp_node->right->height == -2 )
               {  if( tmp_node->right->right->height - 
                                        tmp_node->left->height == 1 )
                  {  left_rotation( tmp_node );
                     tmp_node->left->height = 
                                 tmp_node->left->right->height + 1;
                     tmp_node->height = tmp_node->left->height + 1;
                  }
                  else
                  {  right_rotation( tmp_node->right );
                     left_rotation( tmp_node );
                     tmp_height = tmp_node->right->right->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else /* update height even if there was no rotation */ 
               {  if( tmp_node->left->height > tmp_node->right->height )
                     tmp_node->height = tmp_node->left->height + 1;
                  else
                     tmp_node->height = tmp_node->right->height + 1;
               }
               if( tmp_node->height == old_height )
                  finished = 1;
            }

            //rebalance 2
            finished2 = 0;
            while( path_st_p2 > 0 && !finished2 )
            {  int tmp_height, old_height;
               o_t *tmp_node;
               tmp_node = path_stack2[--path_st_p2];
               old_height= tmp_node->height;
               if( tmp_node->left->height - 
                                       tmp_node->right->height == 2 )
               {  if( tmp_node->left->left->height - 
                                       tmp_node->right->height == 1 )
                  {  right_rotation( tmp_node );
                     tmp_node->right->height = 
                                  tmp_node->right->left->height + 1;
                     tmp_node->height = tmp_node->right->height + 1;
                  }
                  else
                  {  left_rotation( tmp_node->left );
                     right_rotation( tmp_node );
                     tmp_height = tmp_node->left->left->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else if ( tmp_node->left->height - 
                                      tmp_node->right->height == -2 )
               {  if( tmp_node->right->right->height - 
                                        tmp_node->left->height == 1 )
                  {  left_rotation( tmp_node );
                     tmp_node->left->height = 
                                 tmp_node->left->right->height + 1;
                     tmp_node->height = tmp_node->left->height + 1;
                  }
                  else
                  {  right_rotation( tmp_node->right );
                     left_rotation( tmp_node );
                     tmp_height = tmp_node->right->right->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else /* update height even if there was no rotation */ 
               {  if( tmp_node->left->height > tmp_node->right->height )
                     tmp_node->height = tmp_node->left->height + 1;
                  else
                     tmp_node->height = tmp_node->right->height + 1;
               }
               if( tmp_node->height == old_height )
                  finished2 = 1;
            }//finish rebalance

        //end rebalance
      }
    }
}


void insert_after(o_t *ord, key_t a, key_t b) {
    int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;
    //find b first
    o_t *tmp_node;
    o_t *ord2, *cur;
    o_t *path_stack[100], *path_stack2[100], *tmp_path_stack2[100];
    int path_st_p = 0;
    int path_st_p2 = 0;
    int tmp_path2_index = 0;
    if (ord->left == NULL) {
      return;
    }else {
      tmp_node = ord;
      while (tmp_node->right != NULL) {
        if (b < tmp_node->key) {
          tmp_node = tmp_node->left;
        }else {
          tmp_node = tmp_node->right;
        }
      }//find the insert node in tree1

      if (tmp_node->key == b) {
        ord2 = tmp_node->back;
        while (ord2->back != NULL) {
          tmp_path_stack2[tmp_path2_index++] = ord2;
          ord2 = ord2->back;
        }
        //reverse stack;
        while(tmp_path2_index >= 0) {
          path_stack2[path_st_p2++] = tmp_path_stack2[tmp_path2_index--];
        }

        //insert a after b
        cur = tmp_node->back;
        o_t *old_leaf, *new_leaf;
        o_t *old_leaf2, *new_leaf2;
        int finished, finished2;

        {
          old_leaf2 = get_node();
          old_leaf2->left = cur->left;
          old_leaf2->key = cur->key;
          old_leaf2->right = NULL;
          old_leaf2->height = 0;
          new_leaf2 = get_node();
          new_leaf2->left = (o_t *) insobj2;
          new_leaf2->key = a;
          new_leaf2->right = NULL;
          new_leaf2->height = 0;

          cur->right = new_leaf2;
          new_leaf2->back = cur;
          cur->left = old_leaf2;
          old_leaf2->back = cur;
          cur->height = 1;          
        }//finish insert a to tree2;
        {
          //find node a position first;
          tmp_node = ord;
          while (tmp_node->right != NULL) {
            if (a < tmp_node->key) {
              tmp_node = tmp_node->left;
            }else {
              tmp_node = tmp_node->right;
            }
          }//find the insert node in tree1
          //insert ord;
             old_leaf = get_node();
             old_leaf->left = tmp_node->left; 
             old_leaf->key = tmp_node->key;
             old_leaf->right  = NULL;
             old_leaf->height = 0;
             new_leaf = get_node();
             new_leaf->left = (o_t *) insobj; 
             new_leaf->key = a;
             new_leaf->right  = NULL;
             new_leaf->height = 0;
             if( tmp_node->key < a )
             {   tmp_node->left  = old_leaf;
                 tmp_node->right = new_leaf;
                 tmp_node->key = a;
                 tmp_node->back = NULL;
                 tmp_node->right->back = cur->left;
             } 
             else
             {   tmp_node->left  = new_leaf;
                 tmp_node->right = old_leaf;
                 tmp_node->back = NULL;
                 tmp_node->left->back = cur->left;
             } 
             tmp_node->height = 1;


        }//finish insert a to tree1;

        //start rebalance
            //rebalance
            finished = 0;
            while( path_st_p > 0 && !finished )
            {  int tmp_height, old_height;
               o_t *tmp_node;
               tmp_node = path_stack[--path_st_p];
               old_height= tmp_node->height;
               if( tmp_node->left->height - 
                                       tmp_node->right->height == 2 )
               {  if( tmp_node->left->left->height - 
                                       tmp_node->right->height == 1 )
                  {  right_rotation( tmp_node );
                     tmp_node->right->height = 
                                  tmp_node->right->left->height + 1;
                     tmp_node->height = tmp_node->right->height + 1;
                  }
                  else
                  {  left_rotation( tmp_node->left );
                     right_rotation( tmp_node );
                     tmp_height = tmp_node->left->left->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else if ( tmp_node->left->height - 
                                      tmp_node->right->height == -2 )
               {  if( tmp_node->right->right->height - 
                                        tmp_node->left->height == 1 )
                  {  left_rotation( tmp_node );
                     tmp_node->left->height = 
                                 tmp_node->left->right->height + 1;
                     tmp_node->height = tmp_node->left->height + 1;
                  }
                  else
                  {  right_rotation( tmp_node->right );
                     left_rotation( tmp_node );
                     tmp_height = tmp_node->right->right->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else /* update height even if there was no rotation */ 
               {  if( tmp_node->left->height > tmp_node->right->height )
                     tmp_node->height = tmp_node->left->height + 1;
                  else
                     tmp_node->height = tmp_node->right->height + 1;
               }
               if( tmp_node->height == old_height )
                  finished = 1;
            }

            //rebalance 2
            finished2 = 0;
            while( path_st_p2 > 0 && !finished2 )
            {  int tmp_height, old_height;
               o_t *tmp_node;
               tmp_node = path_stack2[--path_st_p2];
               old_height= tmp_node->height;
               if( tmp_node->left->height - 
                                       tmp_node->right->height == 2 )
               {  if( tmp_node->left->left->height - 
                                       tmp_node->right->height == 1 )
                  {  right_rotation( tmp_node );
                     tmp_node->right->height = 
                                  tmp_node->right->left->height + 1;
                     tmp_node->height = tmp_node->right->height + 1;
                  }
                  else
                  {  left_rotation( tmp_node->left );
                     right_rotation( tmp_node );
                     tmp_height = tmp_node->left->left->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else if ( tmp_node->left->height - 
                                      tmp_node->right->height == -2 )
               {  if( tmp_node->right->right->height - 
                                        tmp_node->left->height == 1 )
                  {  left_rotation( tmp_node );
                     tmp_node->left->height = 
                                 tmp_node->left->right->height + 1;
                     tmp_node->height = tmp_node->left->height + 1;
                  }
                  else
                  {  right_rotation( tmp_node->right );
                     left_rotation( tmp_node );
                     tmp_height = tmp_node->right->right->height; 
                     tmp_node->left->height  = tmp_height + 1; 
                     tmp_node->right->height = tmp_height + 1; 
                     tmp_node->height = tmp_height + 2; 
                  }
               }
               else /* update height even if there was no rotation */ 
               {  if( tmp_node->left->height > tmp_node->right->height )
                     tmp_node->height = tmp_node->left->height + 1;
                  else
                     tmp_node->height = tmp_node->right->height + 1;
               }
               if( tmp_node->height == old_height )
                  finished2 = 1;
            }//finish rebalance

        //end rebalance
      }
    }
}


void insert_top(o_t *ord, key_t a) {
  int *insobj = (int *) malloc(sizeof(int));
  *insobj = a;
  int *insobj2 = (int *) malloc(sizeof(int));
  *insobj2 = a;

  o_t *ord2, *tmp;
  tmp = ord;
  if (ord->left == NULL) {
    ord2 = ord->back;

    //insert ord2
    ord2->left = (o_t *) insobj2;
    ord2->left->back = ord2;
    ord2->key = a;
    ord2->height = 0;
    ord2->right = NULL;

    //insert ord
    ord->left = (o_t *) insobj;
    ord->key = a;
    ord->left->back = ord2->left;
    ord->back = NULL;
    ord->height = 0;
    ord->right = NULL;
  }else {
    while (tmp != NULL) {
      tmp = tmp->left;
    }
    tmp = tmp->back;
    while (tmp->back != NULL) {
      tmp = tmp->back;
    }
    ord2 = tmp;
    o_t *insertp = ord;
    o_t *insertp2 = ord2;
    o_t *path_stack2[100];
    o_t *path_stack[100];
    int path_st_p = 0;
    int path_st_p2 = 0;
    int finished;
    int finished2;
    while(insertp2->right != NULL) {
      path_stack2[path_st_p2++] = insertp2;
      insertp2 = insertp2->left;
    }
    while (insertp->right != NULL) {
      path_stack[path_st_p++] = insertp;
      if (a < insertp->key) {
        insertp = insertp->left;
      }else {
        insertp = insertp->right;
      }
    }
    // ------ for now, not handle tmp_node->key == new_key
    {
      o_t *old_leaf, *new_leaf;
      o_t *old_leaf2, *new_leaf2;

      old_leaf2 = get_node();
      old_leaf2->left = insertp2->left;
      old_leaf2->key = insertp2->key;
      old_leaf2->right = NULL;
      old_leaf2->height = 0;
      new_leaf2 = get_node();
      new_leaf2->left = (o_t *) insobj2;
      new_leaf2->key = a;
      new_leaf2->right = NULL;
      new_leaf2->height = 0;

      insertp2->left = new_leaf2;
      new_leaf2->back = insertp2;
      insertp2->right = old_leaf2;
      old_leaf2->back = insertp2;
      insertp2->height = 1;
      //insert ord;
         old_leaf = get_node();
         old_leaf->left = insertp->left; 
         old_leaf->key = insertp->key;
         old_leaf->right  = NULL;
         old_leaf->height = 0;
         new_leaf = get_node();
         new_leaf->left = (o_t *) insobj; 
         new_leaf->key = a;
         new_leaf->right  = NULL;
         new_leaf->height = 0;
         if( insertp->key < a )
         {   insertp->left  = old_leaf;
             insertp->right = new_leaf;
             insertp->key = a;
             insertp->back = NULL;
             insertp->right->back = insertp2->left;
         } 
         else
         {   insertp->left  = new_leaf;
             insertp->right = old_leaf;
             insertp->back = NULL;
             insertp->left->back = insertp2->left;
         } 
         insertp->height = 1;

    }//end insert;
    //rebalance
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         o_t *tmp_node;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }

      //rebalance 2
      finished2 = 0;
      while( path_st_p2 > 0 && !finished2 )
      {  int tmp_height, old_height;
         o_t *tmp_node;
         tmp_node = path_stack2[--path_st_p2];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished2 = 1;
      }//finish rebalance
  }

}


void insert_bottom(o_t *ord, key_t a) {
  int *insobj = (int *) malloc(sizeof(int));
  *insobj = a;
  int *insobj2 = (int *) malloc(sizeof(int));
  *insobj2 = a;

  o_t *ord2, *tmp;
  tmp = ord;
  if (ord->left == NULL) {
    ord2 = ord->back;

    //insert ord2
    ord2->left = (o_t *) insobj2;
    ord2->left->back = ord2;
    ord2->key = a;
    ord2->height = 0;
    ord2->right = NULL;

    //insert ord
    ord->left = (o_t *) insobj;
    ord->key = a;
    ord->left->back = ord2->left;
    ord->back = NULL;
    ord->height = 0;
    ord->right = NULL;
  }else {
    while (tmp != NULL) {
      tmp = tmp->right;
    }
    tmp = tmp->back;
    while (tmp->back != NULL) {
      tmp = tmp->back;
    }
    ord2 = tmp;
    o_t *insertp = ord;
    o_t *insertp2 = ord2;
    o_t *path_stack2[100];
    o_t *path_stack[100];
    int path_st_p = 0;
    int path_st_p2 = 0;
    int finished;
    int finished2;
    while(insertp2->right != NULL) {
      path_stack2[path_st_p2++] = insertp2;
      insertp2 = insertp2->right;
    }
    while (insertp->right != NULL) {
      path_stack[path_st_p++] = insertp;
      if (a < insertp->key) {
        insertp = insertp->left;
      }else {
        insertp = insertp->right;
      }
    }
    // ------ for now, not handle tmp_node->key == new_key
    {
      o_t *old_leaf, *new_leaf;
      o_t *old_leaf2, *new_leaf2;

      old_leaf2 = get_node();
      old_leaf2->left = insertp2->left;
      old_leaf2->key = insertp2->key;
      old_leaf2->right = NULL;
      old_leaf2->height = 0;
      new_leaf2 = get_node();
      new_leaf2->left = (o_t *) insobj2;
      new_leaf2->key = a;
      new_leaf2->right = NULL;
      new_leaf2->height = 0;

      insertp2->right = new_leaf2;
      new_leaf2->back = insertp2;
      insertp2->left = old_leaf2;
      old_leaf2->back = insertp2;
      insertp2->height = 1;
      //insert ord;
         old_leaf = get_node();
         old_leaf->left = insertp->left; 
         old_leaf->key = insertp->key;
         old_leaf->right  = NULL;
         old_leaf->height = 0;
         new_leaf = get_node();
         new_leaf->left = (o_t *) insobj; 
         new_leaf->key = a;
         new_leaf->right  = NULL;
         new_leaf->height = 0;
         if( insertp->key < a )
         {   insertp->left  = old_leaf;
             insertp->right = new_leaf;
             insertp->key = a;
             insertp->back = NULL;
             insertp->right->back = insertp2->right;
         } 
         else
         {   insertp->left  = new_leaf;
             insertp->right = old_leaf;
             insertp->back = NULL;
             insertp->left->back = insertp2->right;
         } 
         insertp->height = 1;

    }//end insert;
    //rebalance
      finished = 0;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         o_t *tmp_node;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished = 1;
      }

      //rebalance 2
      finished2 = 0;
      while( path_st_p2 > 0 && !finished2 )
      {  int tmp_height, old_height;
         o_t *tmp_node;
         tmp_node = path_stack2[--path_st_p2];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation( tmp_node->left );
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
            {  left_rotation( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished2 = 1;
      }//finish rebalance
  }

}

void delete_o(o_t *ord, key_t a) {
  o_t *tmp_node, *upper_node, *other_node;
  o_t *tmp_node2, *upper_node2, *other_node2;
  o_t *ord2;
  object_t *deleted_object;
  object_t *deleted_object2;
  int finished;
  int finished2;
  if (ord->left == NULL) {
    return ;
  }else if (ord->right == NULL) {
    if (ord->key == a) {
      ord2 = ord->left->back->back;  
      deleted_object = (object_t *) ord->left;
      ord->left = NULL;
      deleted_object2 = (object_t *) ord2->left;
      ord2->left = NULL;
      ord->back = ord2;
      return ;
    }else {
      return ;
    }
  }else {
      o_t *path_stack[100];
      o_t *path_stack2[100];
      int path_st_p = 0;
      int path_st_p2 = 0;
      tmp_node = ord;
      while (tmp_node->right != NULL) {
        path_stack[path_st_p++] = tmp_node;
        upper_node = tmp_node;
        if (a < tmp_node->key) {
          tmp_node = upper_node->left;
          other_node = upper_node->right;
        }else {
          tmp_node = upper_node->right;
          other_node = upper_node->left;
        }
      }
      if (tmp_node->key != a) {
        deleted_object = NULL;
      }else {
        tmp_node2 = tmp_node->back;
        upper_node2 = tmp_node2->back;
        if (tmp_node2->key == upper_node2->left->key) {
          other_node2 = upper_node2->right;
        }else{
          other_node2 = upper_node2->left;
        }
        upper_node2->key = other_node2->key;
        upper_node2->left = other_node2->left;
        upper_node2->right = other_node2->right;
        upper_node2->height = other_node2->height;
        deleted_object2 = (object_t *) tmp_node2->left;

        upper_node->key = other_node->key;
        upper_node->left = other_node->left;
        upper_node->right = other_node->right;
        upper_node->height = other_node->height;
        deleted_object = (object_t *) tmp_node->left;

        //add tree 2 path to path_stack2
        while (tmp_node2 != NULL) {
          path_stack2[path_st_p2++] = tmp_node2;
          tmp_node2 = tmp_node2->back;
        }
        return_node (tmp_node);
        return_node (other_node);
        return_node (tmp_node2);
        return_node (other_node2);
      }

      //start rebalance
      finished = 0; path_st_p -= 1;
      while( path_st_p > 0 && !finished )
      {  int tmp_height, old_height;
         tmp_node = path_stack[--path_st_p];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
      {  right_rotation( tmp_node ); 
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
      {  left_rotation( tmp_node->left ); 
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
      {  left_rotation( tmp_node ); 
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
      finished = 1;
      }

      //start rebalance tree_2
      finished2 = 0; path_st_p2 -= 1;
      while( path_st_p2 > 0 && !finished2 )
      {  int tmp_height, old_height;
         tmp_node = path_stack2[--path_st_p2];
         old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
      {  right_rotation( tmp_node ); 
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
      {  left_rotation( tmp_node->left ); 
               right_rotation( tmp_node );
               tmp_height = tmp_node->left->left->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else if ( tmp_node->left->height - 
                                tmp_node->right->height == -2 )
         {  if( tmp_node->right->right->height - 
                                  tmp_node->left->height == 1 )
      {  left_rotation( tmp_node ); 
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation( tmp_node->right );
               left_rotation( tmp_node );
               tmp_height = tmp_node->right->right->height; 
               tmp_node->left->height  = tmp_height + 1; 
               tmp_node->right->height = tmp_height + 1; 
               tmp_node->height = tmp_height + 2; 
            }
         }
         else /* update height even if there was no rotation */ 
         {  if( tmp_node->left->height > tmp_node->right->height )
               tmp_node->height = tmp_node->left->height + 1;
            else
               tmp_node->height = tmp_node->right->height + 1;
         }
         if( tmp_node->height == old_height )
            finished2 = 1;
      }
      /*end rebalance*/
      return;
  }
}


int is_before(o_t *ord, key_t a, key_t b) {
  // find a and b in ord
  o_t *node_a, *node_b;
  o_t *tmp_node;
  if (ord->left == NULL) {
    return 0;
  }else {
    //find a
      tmp_node = ord;
      while( tmp_node->right != NULL )
      {   if( a < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == a )
         node_a = tmp_node->back;
      else
         return 0;  
    // end find a

    // find b
      tmp_node = ord;
      while( tmp_node->right != NULL )
      {   if( b < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == b )
         node_b = tmp_node->back;
      else
         return 0;
    // end find b;

    // check height;
    o_t *tmp_node_a = node_a;
    o_t *tmp_node_b = node_b;
    int height_a = 0;
    int height_b = 0;
    while (tmp_node_a->back != NULL) {
      height_a++;
      tmp_node_a = tmp_node_a->back;
    }
    while (tmp_node_b->back != NULL) {
      height_b++;
      tmp_node_b = tmp_node_b->back;
    }

    //if height not equal, back the longer one
    if (height_a != height_b) {
      if (height_a > height_b) {
        while (height_a != height_b) {
          node_a = node_a->back;
          height_a--;
        }
      }else {
        while (height_b != height_a) {
          node_b = node_b->back;
          height_b--;
        }
      }
    }// height equal;
    while (node_a->back != node_b->back) {
      node_a = node_a->back;
      node_b = node_b->back;
    }
    if (node_a->back->left == node_a) {
      return 1;
    }

  }
  return 0;
}



long p(long q)
{ return( (1247*q +104729) % 300007 );
}

int main()
{  long i; o_t *o; 
   printf("starting \n");
   o = create_order();
   for(i=100000; i>=0; i-- )
      insert_bottom( o, p(i) );
   for(i=100001; i< 300007; i+=2 )
   {  insert_after(o, p(i+1), p(i-1) );
      insert_before( o, p(i), p(i+1) );
   }
   printf("inserted 300000 elements. ");
   for(i = 250000; i < 300007; i++ )
      delete_o( o, p(i) );
   printf("deleted 50000 elements. ");
   insert_top( o, p(300006) );
   for(i = 250000; i < 300006; i++ )
      insert_before( o, p(i) , p(300006) );
   printf("reinserted. now testing order\n");
   for( i=0; i < 299000; i +=42 )
   {  if( is_before( o, p(i), p(i+23) ) != 1 )
      {  printf(" found error (1) \n"); exit(0);
      }
   }
   for( i=300006; i >57; i -=119 )
   {  if( is_before( o, p(i), p(i-57) ) != 0 )
      {  printf(" found error (0) \n"); exit(0);
      }
   }
   printf("finished. no problem found.\n");
} 
