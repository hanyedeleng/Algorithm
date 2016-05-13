#include <stdio.h>
#include <stdlib.h>
#define BLOCKSIZE 256
typedef int object_t;
typedef int key_t;
typedef struct tr_n_t { key_t        key;
                    struct tr_n_t  *left;
                    struct tr_n_t *right;
                    struct tr_n_t *back;
                    int           height; 
                      } o_t;


o_t *currentblock = NULL;
int    size_left;
o_t *free_list = NULL;

void check_tree( o_t *tr, int depth, int lower, int upper );

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

void left_rotation(o_t *n)
{  o_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->left; 
   tmp_key  = n->key;
   n->left  = n->right;        
   n->key   = n->right->key;
   n->right = n->left->right;  
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   n->left->key   = tmp_key;
}

void right_rotation(o_t *n)
{  o_t *tmp_node;
   key_t        tmp_key;
   tmp_node = n->right; 
   tmp_key  = n->key;
   n->right = n->left;        
   n->key   = n->left->key;
   n->left  = n->right->left;  
   n->right->left = n->right->right;
   n->right->right  = tmp_node;
   n->right->key   = tmp_key;
}

void left_rotation2(o_t *n)
{ o_t *tmp_node;
  tmp_node = n->left;
  n->left = n->right;
  n->right = n->left->right;
  n->right->back = n;
  n->left->right = n->left->left;
  n->left->left = tmp_node;
  n->left->left->back = n->left;
}

void right_rotation2(o_t *n)
{ o_t *tmp_node;
  tmp_node = n->right;
  n->right = n->left;
  n->left = n->right->left;
  n->left->back = n;
  n->right->left = n->right->right;
  n->right->right = tmp_node;
  n->right->right->back = n->right;
}


o_t *create_order() {
	o_t *root, *root2;
	root = get_node();
	root2 = get_node();

	root2->left = NULL;
	root->left = NULL;
  
	root->back = root2;
	return root;
}

int find(o_t *tree, key_t query_key)
{  o_t *tmp_node;
   if( tree->left == NULL )
     return 0;
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->key == query_key )
         return 1;
      else
         return 0;
   }
}

void insert_node (o_t *tmp_node, key_t a) {
  o_t *old_leaf, *new_leaf;
  int *new_object = (int *) malloc(sizeof(int));
    *new_object = a;

    old_leaf = get_node();
    old_leaf->left = tmp_node->left; 
    old_leaf->key = tmp_node->key;
    old_leaf->right  = NULL;
    old_leaf->height = 0;
    new_leaf = get_node();
    new_leaf->left = (o_t *) new_object; 
    new_leaf->key = a;
    new_leaf->right  = NULL;
    new_leaf->height = 0; 
    if( tmp_node->key < a )
    {   tmp_node->left  = old_leaf;
        tmp_node->right = new_leaf;
        tmp_node->key = a;
    } 
    else
    {   tmp_node->left  = new_leaf;
        tmp_node->right = old_leaf;
    } 
    tmp_node->height = 1;
}

void insert_node2 (o_t *tmp_node, key_t a, int before) {
  o_t *old_leaf, *new_leaf;
  int *new_object = (int *) malloc(sizeof(int));
    *new_object = a;

    old_leaf = get_node();
    old_leaf->left = tmp_node->left; 
    old_leaf->key = 0;
    old_leaf->right  = NULL;
    old_leaf->height = 0;
    new_leaf = get_node();
    new_leaf->left = (o_t *) new_object; 
    new_leaf->key = 0;
    new_leaf->right  = NULL;
    new_leaf->height = 0; 
    if( before != 1 )
    {   tmp_node->left  = old_leaf;
        tmp_node->right = new_leaf;
    } 
    else
    {   tmp_node->left  = new_leaf;
        tmp_node->right = old_leaf;
    }
    old_leaf->back = tmp_node;
    new_leaf->back = tmp_node; 
    tmp_node->height = 1;
}

void insert_before(o_t *ord, key_t a, key_t b) {
  /* insert a before b*/
  /*
   * find b first
     insert a before b in tree 2, saved tree2 insert position, update back pointer to it later
     insert a in tree 1, saved insert position, update back pointer.
   */
     if (find(ord, b) == 0) {
      return;
     }

    int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

    // find b in tree 1
    o_t *b_position = ord;
    while (b_position->right != NULL) {
      if (b < b_position->key)
        b_position = b_position->left;
      else
        b_position = b_position->right;
    }
    o_t *b_position_in_tree1 = b_position;
    b_position = b_position->back;
    insert_node2(b_position, a, 1);

    b_position_in_tree1->back = b_position->right;

    o_t *tree2_rotation_start_node = b_position;

    //insert a in tree 1;
    o_t *tree1_position = ord;
    o_t *tree1_saved_position;
    o_t *path_stack[100];
    int path_st_p = 0;

    while (tree1_position->right != NULL) {
      path_stack[path_st_p++] = tree1_position;
      if (a < tree1_position->key)
        tree1_position = tree1_position->left;
      else
        tree1_position = tree1_position->right;
    }
    insert_node(tree1_position, a);
    //linked tree node;
     if (tree1_position->left->key == a) {
      tree1_position->right->back = tree1_position->back;
      tree1_position->left->back = b_position->left;
     }else{
      tree1_position->left->back = tree1_position->back;
      tree1_position->right->back = b_position->left;
     }
     tree1_position->back = NULL;

     /* rebalance */
    // tree 1
   int finished = 0;
   o_t *tmp_node;
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
      }//finished rebalance tree1;
   // end tree 1

      // rebalance tree 2
      tmp_node = tree2_rotation_start_node;
      //printf("insert position value:%d\n",tmp_node->left->left->key);
      while( tmp_node != NULL )
      {  int tmp_height, old_height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation2( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation2( tmp_node->left );
               right_rotation2( tmp_node );
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
            {  left_rotation2( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation2( tmp_node->right );
               left_rotation2( tmp_node );
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
         // if( tmp_node->height == old_height )
         //    finished2 = 1;
         tmp_node = tmp_node->back;
      }
      return;

}


void insert_after(o_t *ord, key_t a, key_t b) {
  /* insert a before b*/
  /*
   * find b first
     insert a after b in tree 2, saved tree2 insert position, update back pointer to it later
     insert a in tree 1, saved insert position, update back pointer.
   */
     if (find(ord, b) == 0) {
      return;
     }

    int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

    // find b in tree 1
    o_t *b_position = ord;
    while (b_position->right != NULL) {
      if (b < b_position->key)
        b_position = b_position->left;
      else
        b_position = b_position->right;
    }
    o_t *b_position_in_tree1 = b_position;
    b_position = b_position->back;
    insert_node2(b_position, a, 0);

    b_position_in_tree1->back = b_position->left;

    o_t *tree2_rotation_start_node = b_position;

    //insert a in tree 1;
    o_t *tree1_position = ord;
    o_t *tree1_saved_position;
    o_t *path_stack[100];
    int path_st_p = 0;

    while (tree1_position->right != NULL) {
      path_stack[path_st_p++] = tree1_position;
      if (a < tree1_position->key)
        tree1_position = tree1_position->left;
      else
        tree1_position = tree1_position->right;
    }
    insert_node(tree1_position, a);
    //linked tree node;
     if (tree1_position->left->key == a) {
      tree1_position->right->back = tree1_position->back;
      tree1_position->left->back = b_position->right;
     }else{
      tree1_position->left->back = tree1_position->back;
      tree1_position->right->back = b_position->right;
     }
     tree1_position->back = NULL;

     /* rebalance */
    // tree 1
   int finished = 0;
   o_t *tmp_node;
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
      }//finished rebalance tree1;
   // end tree 1

      // rebalance tree 2
      tmp_node = tree2_rotation_start_node;
      //printf("insert position value:%d\n",tmp_node->left->left->key);
      while( tmp_node != NULL )
      {  int tmp_height, old_height;
         //o_t *tmp_node;
         //tmp_node = path_stack2[--path_st_p2];
         //old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation2( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation2( tmp_node->left );
               right_rotation2( tmp_node );
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
            {  left_rotation2( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation2( tmp_node->right );
               left_rotation2( tmp_node );
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
         // if( tmp_node->height == old_height )
         //    finished2 = 1;
         tmp_node = tmp_node->back;
      }
      return;

}


void insert_bottom(o_t *ord, key_t a) {
    int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;
    // insert bottom, add a to the beginning of tree 2
    //situation 1, tree empty;
    if (find(ord, a) == 1)
      return;
    if (ord->left == NULL) {
      //add a to tree 2 and add a to tree 1
      ord->back->left = (o_t *) insobj2;
      ord->left = (o_t *) insobj;
      ord->key = a;
      return;
    }

    //situation 2, tree not empty
    /*
   * 1. find insert position in tree 2
        saved the value in position p2, update the back pointer point to p2
   * 2. insert a in tree 2;
   * 3. insert a in tree 1, back pointer -> tree 2,
   * 4. find saved value in tree 1, update pointer position in tree 2
   * 
    */
   o_t *tmp_node = ord;
   o_t *tree1_position = ord;
   o_t *tree1_saved_position = ord;
   o_t *path_stack[100];
   int path_st_p = 0;
   int saved_value;
   while (tmp_node->right != NULL) {
    tmp_node = tmp_node->left;
   }
   tmp_node = tmp_node->back;
   while (tmp_node->back != NULL) {
    tmp_node = tmp_node->back;
   }
   //find the insert position
   while (tmp_node->right != NULL) {
    tmp_node = tmp_node->left;
   }
   saved_value = tmp_node->left->key;
   // if insert node is before a, third parameter value is 1, else 0
   insert_node2(tmp_node, a, 1);

   while (tree1_position->right != NULL) {
    path_stack[path_st_p++] = tree1_position;
    if (a < tree1_position->key)
      tree1_position = tree1_position->left;
    else
      tree1_position = tree1_position->right;
   }
   insert_node(tree1_position, a);
   //linked tree node;
   if (tree1_position->left->key == a) {
    tree1_position->right->back = tree1_position->back;
    tree1_position->left->back = tmp_node->left;
   }else{
    tree1_position->left->back = tree1_position->back;
    tree1_position->right->back = tmp_node->left;
   }
   tree1_position->back = NULL;

   //find saved value position in tree 1, update back pointer
   while (tree1_saved_position->right != NULL) {
    if (saved_value < tree1_saved_position->key) 
      tree1_saved_position = tree1_saved_position->left;
    else
      tree1_saved_position = tree1_saved_position->right;
   }
   tree1_saved_position->back = tmp_node->right;

   o_t *tree2_rotation_start_node = tmp_node;
   /*rebalance*/
   // tree 1
   int finished = 0;
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
      }//finished rebalance tree1;
   // end tree 1

      // rebalance tree 2
      tmp_node = tree2_rotation_start_node;
      //printf("insert position value:%d\n",tmp_node->left->left->key);
      while( tmp_node != NULL )
      {  int tmp_height, old_height;
         //o_t *tmp_node;
         //tmp_node = path_stack2[--path_st_p2];
         //old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation2( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation2( tmp_node->left );
               right_rotation2( tmp_node );
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
            {  left_rotation2( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation2( tmp_node->right );
               left_rotation2( tmp_node );
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
         // if( tmp_node->height == old_height )
         //    finished2 = 1;
         tmp_node = tmp_node->back;
      }
      return;
}


void insert_top(o_t *ord, key_t a) {
  int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;
    // insert bottom, add a to the beginning of tree 2
    //situation 1, tree empty;
    if (find(ord, a) == 1)
      return;
    if (ord->left == NULL) {
      //add a to tree 2 and add a to tree 1
      ord->back->left = (o_t *) insobj2;
      ord->left = (o_t *) insobj;
      ord->key = a;
      return;
    }

    //situation 2, tree not empty
    /*
   * 1. find insert position in tree 2
        saved the value in position p2, update the back pointer point to p2
   * 2. insert a in tree 2;
   * 3. insert a in tree 1, back pointer -> tree 2,
   * 4. find saved value in tree 1, update pointer position in tree 2
   * 
    */
   o_t *tmp_node = ord;
   o_t *tree1_position = ord;
   o_t *tree1_saved_position = ord;
   o_t *path_stack[100];
   int path_st_p = 0;
   int saved_value;
   while (tmp_node->right != NULL) {
    tmp_node = tmp_node->left;
   }
   tmp_node = tmp_node->back;
   while (tmp_node->back != NULL) {
    tmp_node = tmp_node->back;
   }
   //find the insert position
   while (tmp_node->right != NULL) {
    tmp_node = tmp_node->right;
   }
   saved_value = tmp_node->left->key;
   // if insert node is before a, third parameter value is 1, else 0
   insert_node2(tmp_node, a, 0);

   while (tree1_position->right != NULL) {
    path_stack[path_st_p++] = tree1_position;
    if (a < tree1_position->key)
      tree1_position = tree1_position->left;
    else
      tree1_position = tree1_position->right;
   }
   insert_node(tree1_position, a);
   //linked tree node;
   if (tree1_position->left->key == a) {
    tree1_position->right->back = tree1_position->back;
    tree1_position->left->back = tmp_node->left;
   }else{
    tree1_position->left->back = tree1_position->back;
    tree1_position->right->back = tmp_node->left;
   }
   tree1_position->back = NULL;

   //find saved value position in tree 1, update back pointer
   while (tree1_saved_position->right != NULL) {
    if (saved_value < tree1_saved_position->key) 
      tree1_saved_position = tree1_saved_position->left;
    else
      tree1_saved_position = tree1_saved_position->right;
   }
   tree1_saved_position->back = tmp_node->right;

   o_t *tree2_rotation_start_node = tmp_node;
   /*rebalance*/
   // tree 1
   int finished = 0;
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
      }//finished rebalance tree1;
   // end tree 1

      // rebalance tree 2
      int finished2 = 0;
      tmp_node = tree2_rotation_start_node;
      //printf("insert position value:%d\n",tmp_node->left->left->key);
      while( tmp_node != NULL )
      {  int tmp_height, old_height;
         //o_t *tmp_node;
         //tmp_node = path_stack2[--path_st_p2];
         //old_height= tmp_node->height;
         if( tmp_node->left->height - 
                                 tmp_node->right->height == 2 )
         {  if( tmp_node->left->left->height - 
                                 tmp_node->right->height == 1 )
            {  right_rotation2( tmp_node );
               tmp_node->right->height = 
                            tmp_node->right->left->height + 1;
               tmp_node->height = tmp_node->right->height + 1;
            }
            else
            {  left_rotation2( tmp_node->left );
               right_rotation2( tmp_node );
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
            {  left_rotation2( tmp_node );
               tmp_node->left->height = 
                           tmp_node->left->right->height + 1;
               tmp_node->height = tmp_node->left->height + 1;
            }
            else
            {  right_rotation2( tmp_node->right );
               left_rotation2( tmp_node );
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
         // if( tmp_node->height == old_height )
         //    finished2 = 1;
         tmp_node = tmp_node->back;
      }
      return;
}



void check_tree( o_t *tr, int depth, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
         printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  //if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)(%d)  ", tr->key, depth, tr->back->left->key);
      // else
      //    printf("Wrong Object \n");
   }
   else
   {  check_tree(tr->left, depth+1, lower, tr->key ); 
      check_tree(tr->right, depth+1, tr->key, upper ); 
   }
}
void check_tree2( o_t *tr, int depth, int lower, int upper )
{
  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   // if( tr->key < lower || tr->key >= upper )
   //       printf("Wrong Key Order \n");
   if( tr->right == NULL )
   {  //if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)(%d)  ", tr->key, depth, tr->left->key);
      // else
      //    printf("Wrong Object \n");
   }
   else
   {  check_tree2(tr->left, depth+1, lower, tr->key ); 
      check_tree2(tr->right, depth+1, tr->key, upper ); 
   }

}



void delete_o(o_t *ord, key_t a) {
  o_t *tmp_node, *upper_node, *other_node;
  o_t *tmp_node2, *upper_node2, *other_node2;
  object_t *deleted_object, *deleted_object2;
  int finished, finished2;

  if (ord->left == NULL) {
    return;
  }else if (ord->right == NULL) {
    if (ord->key == a) {
      ord->left = NULL;
      ord->back->left = NULL;
      ord->back->right = NULL;
      return;
    }else {
      return;
    }
  }else {
    o_t *path_stack[100], *path_stack2[100];
    int path_st_p = 0;
    int path_st_p2 = 0;
    if (find(ord, a) == 0) {
      return;
    }
    // find a in tree1
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
      return;
    }else {
      tmp_node2 = tmp_node->back;
      // delete a in tree 2
      upper_node2 = tmp_node2->back;
      if (tmp_node2 == upper_node2->left)
        other_node2 = upper_node2->right;
      else
        other_node2 = upper_node2->left;
      // saved other node key value, wait for update
      int saved_value = other_node2->left->key;

      o_t *node4stack = upper_node2;
      o_t *tmp_stack[100];
      int tmp_st_p = 0;
      while (node4stack != NULL) {
        tmp_stack[tmp_st_p++] = node4stack;
        node4stack = node4stack->back;
      }
      while (tmp_st_p > 0) {
        path_stack2[path_st_p2++] = tmp_stack[--tmp_st_p];
      }

      // 
      upper_node2->key = other_node2->key;
      upper_node2->left = other_node2->left;
      upper_node2->right = other_node2->right;
      upper_node2->height = other_node2->height;

      o_t *tmp_node = ord;
      while (tmp_node->right != NULL) {
        if (saved_value < tmp_node->key)
          tmp_node = tmp_node->left;
        else
          tmp_node = tmp_node->right;
      }
      tmp_node->back = upper_node2;
      //delete a in tree 1, remember to updat the other node back pointer
      upper_node->key = other_node->key;
      upper_node->left = other_node->left;
      upper_node->right = other_node->right;
      upper_node->height = other_node->height;
      upper_node->back = other_node->back;
      other_node->back = NULL;


    }
          //rebalance
          /*start rebalance*/  
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

          //rebalance tree 2
              /*start rebalance*/  
          finished2 = 0; path_st_p2 -= 1;
          while( path_st_p2 > 0 && !finished2 )
          {  int tmp_height, old_height;
             tmp_node = path_stack2[--path_st_p2];
             old_height= tmp_node->height;
             if( tmp_node->left->height - 
                                     tmp_node->right->height == 2 )
             {  if( tmp_node->left->left->height - 
                                     tmp_node->right->height == 1 )
          {  right_rotation2( tmp_node ); 
                   tmp_node->right->height = 
                                tmp_node->right->left->height + 1;
                   tmp_node->height = tmp_node->right->height + 1;
                }
                else
          {  left_rotation2( tmp_node->left ); 
                   right_rotation2( tmp_node );
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
          {  left_rotation2( tmp_node ); 
                   tmp_node->left->height = 
                               tmp_node->left->right->height + 1;
                   tmp_node->height = tmp_node->left->height + 1;
                }
                else
                {  right_rotation2( tmp_node->right );
                   left_rotation2( tmp_node );
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

      //end rebalance 
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
    while (tmp_node_a != NULL) {
      height_a++;
      tmp_node_a = tmp_node_a->back;
    }
    while (tmp_node_b != NULL) {
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
    }else {
      return 0;
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
