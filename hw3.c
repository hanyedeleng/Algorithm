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
{  o_t *tmp_node;
   o_t *tmp_back;
   tmp_node = n->left; 
   tmp_back = n->back;

   n->left  = n->right;        
   n->key   = 0;
   n->right = n->left->right;  
   n->left->right = n->left->left;
   n->left->left  = tmp_node;
   n->left->key   = 0;

   n->back = tmp_back;
   n->left->back = n;
   n->right->back = n;
   n->left->left->back = n->left;
   n->left->right->back = n->left;
}

void right_rotation2(o_t *n)
{  o_t *tmp_node;
   o_t *tmp_back;
   tmp_node = n->right;
   tmp_back = n->back;

   n->right = n->left;        
   n->key   = 0;
   n->left  = n->right->left;  
   n->right->left = n->right->right;
   n->right->right  = tmp_node;
   n->right->key   = 0;

   n->back = tmp_back;
   n->left->back = n;
   n->right->back = n;
   n->right->left->back = n->right;
   n->right->right->back = n->left;
}


o_t *create_order() {
	o_t *root, *root2;
	root = get_node();
	root2 = get_node();

	root2->left = NULL;
	root2->key = 0;

	root->left = NULL;
	root->back = root2;
	return root;
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

void insert_after(o_t *ord, key_t a, key_t b) {
	// if can't find b, return
	if (ord->left == NULL) {
		return;
	}

	// find b
	int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

	o_t *root, *root2;
	root = ord;

	//find b in tree1
	o_t *tmp_node;
	tmp_node = ord;
	while (tmp_node->right != NULL) {
		if (b < tmp_node->key)
			tmp_node = tmp_node->left;
		else
			tmp_node = tmp_node->right;
	}
	if (tmp_node->key == b) {
		if (find(ord, a) != NULL) {
			return;
		}
		//found b in tree1;
		o_t *insert_position2 = tmp_node->back;

		//insert a in tree 2;
		o_t *old_leaf2, *new_leaf2;
		old_leaf2 = get_node();
		old_leaf2->left = insert_position2->left;
		old_leaf2->key = 0;
		old_leaf2->right = NULL;
		old_leaf2->height = 0;
		new_leaf2 = get_node();
		new_leaf2->left = (o_t *) insobj2;
		new_leaf2->key = 0;
		new_leaf2->right = NULL;
		new_leaf2->height = 0;

		insert_position2->left = old_leaf2;
		insert_position2->right = new_leaf2;
		old_leaf2->back = insert_position2;
		new_leaf2->back = insert_position2;
		insert_position2->height = 1;

		o_t *path_stack2[100], *tmp_stack[100];
		int path_st_p2 = 0, tmp_st_p = 0;
		while (insert_position2 != NULL) {
			tmp_stack[tmp_st_p++] = insert_position2;
			insert_position2 = insert_position2->back;
		}
		//reverse stack;
		while (tmp_st_p > 0) {
			path_stack2[path_st_p2++] = tmp_stack[--tmp_st_p];
		}

		//insert a in tree1
		o_t *insert_position = ord;
		o_t *path_stack[100];
		int finished, finished2;
		int path_st_p = 0;
		while (insert_position->right != NULL) {
			path_stack[path_st_p++] = insert_position;
			if (a < insert_position->key)
				insert_position = insert_position->left;
			else
				insert_position = insert_position->right;
		}
		if (insert_position->key == a) {
			return;
		}

		o_t *old_leaf, *new_leaf;
		old_leaf = get_node();
		old_leaf->left = insert_position->left;
		old_leaf->key = insert_position->key;
		old_leaf->right = NULL;
		old_leaf->height = 0;
		new_leaf = get_node();
		new_leaf->left = (o_t *) insobj;
		new_leaf->key = a;
		new_leaf->right = NULL;
		new_leaf->height = 0;

		if(insert_position->key < a) {
			insert_position->left = old_leaf;
			insert_position->right = new_leaf;
			insert_position->key = a;
		}else {
			insert_position->left = new_leaf;
			insert_position->right = old_leaf;
		}
		insert_position->height = 1;
		// linked tree 1 and tree2;
		insert_position->back = NULL;
		old_leaf->back = old_leaf2;
		new_leaf->back = new_leaf2;

		//rebalance
		/*rebalance */
		// rebalance tree1;
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
      }//finished rebalance tree1;

      //rebalance tree2
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
      //finished rebalance tree2
		//finished rebalance
	}else {
		return;
	}
	return;
}

void insert_before(o_t *ord, key_t a, key_t b) {
	// if can't find b, return
	if (ord->left == NULL) {
		return;
	}

	// find b
	int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

	o_t *root, *root2;
	root = ord;

	//find b in tree1
	o_t *tmp_node;
	tmp_node = ord;
	while (tmp_node->right != NULL) {
		if (b < tmp_node->key)
			tmp_node = tmp_node->left;
		else
			tmp_node = tmp_node->right;
	}
	if (tmp_node->key == b) {
		if (find(ord, a) != NULL) {
			return;
		}
		//found b in tree1;
		o_t *insert_position2 = tmp_node->back;

		//insert a in tree 2;
		o_t *old_leaf2, *new_leaf2;
		old_leaf2 = get_node();
		old_leaf2->left = insert_position2->left;
		old_leaf2->key = 0;
		old_leaf2->right = NULL;
		old_leaf2->height = 0;
		new_leaf2 = get_node();
		new_leaf2->left = (o_t *) insobj2;
		new_leaf2->key = 0;
		new_leaf2->right = NULL;
		new_leaf2->height = 0;

		insert_position2->left = new_leaf2;
		insert_position2->right = old_leaf2;
		old_leaf2->back = insert_position2;
		new_leaf2->back = insert_position2;
		insert_position2->height = 1;

		o_t *path_stack2[100], *tmp_stack[100];
		int path_st_p2 = 0, tmp_st_p = 0;
		while (insert_position2 != NULL) {
			tmp_stack[tmp_st_p++] = insert_position2;
			insert_position2 = insert_position2->back;
		}
		//reverse stack;
		while (tmp_st_p > 0) {
			path_stack2[path_st_p2++] = tmp_stack[--tmp_st_p];
		}

		//insert a in tree1
		o_t *insert_position = root;
		o_t *path_stack[100];
		int finished, finished2;
		int path_st_p = 0;
		while (insert_position->right != NULL) {
			path_stack[path_st_p++] = insert_position;
			if (a < insert_position->key)
				insert_position = insert_position->left;
			else
				insert_position = insert_position->right;
		}

		o_t *old_leaf, *new_leaf;
		old_leaf = get_node();
		old_leaf->left = insert_position->left;
		old_leaf->key = insert_position->key;
		old_leaf->right = NULL;
		old_leaf->height = 0;
		new_leaf = get_node();
		new_leaf->left = (o_t *) insobj;
		new_leaf->key = a;
		new_leaf->right = NULL;
		new_leaf->height = 0;

		if(insert_position->key < a) {
			insert_position->left = old_leaf;
			insert_position->right = new_leaf;
			insert_position->key = a;
		}else {
			insert_position->left = new_leaf;
			insert_position->right = old_leaf;
		}
		insert_position->height = 1;
		// linked tree 1 and tree2;
		insert_position->back = NULL;
		old_leaf->back = old_leaf2;
		new_leaf->back = new_leaf2;

		//rebalance
		/*rebalance */
		// rebalance tree1;
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
      }//finished rebalance tree1;

      //rebalance tree2
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
      //finished rebalance tree2
		//finished rebalance
	}else {
		return;
	}
	return;
}


void insert_top(o_t *ord, key_t a) {
	int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

	o_t *root, *root2;

	root = ord;
	root2 = ord;
	while (root2->right != NULL) {
		root2 = root2->right;
	}
	root2 = root2->back;
	while (root2->back != NULL) {
		root2 = root2->back;
	}
	// insert tree2 first, then insert tree1, then linked together
	// empty tree
	if (root->left == NULL) {
		// insert tree2 first
		root2->left = (o_t *) insobj2;
		root2->key = 0;
		root2->height = 0;
		root2->right = NULL;

		root->left = (o_t *) insobj;
		root->key = a;
		root->height = 0;
		root->right = NULL;

		root->back = root2;
	}// end empty tree
	else {
		//check contains a or not
		if (find(ord, a) != NULL) {
			return;
		}
		// find insert position first, then insert tree2, insert tree1, then link
		o_t *path_stack[100], *path_stack2[100];
		int path_st_p = 0, path_st_p2 = 0;
		int finished, finished2;

		o_t *insert_position2 = root2;
		while (insert_position2->right != NULL) {
			path_stack2[path_st_p2++] = insert_position2;
			insert_position2 = insert_position2->left;
		}

		{//insert a to tree2
			o_t *old_leaf2, *new_leaf2;
			old_leaf2 = get_node();
			old_leaf2->left = insert_position2->left;
			old_leaf2->key = 0;
			old_leaf2->right = NULL;
			old_leaf2->height = 0;
			new_leaf2 = get_node();
			new_leaf2->left = (o_t *) insobj2;
			new_leaf2->key = 0;
			new_leaf2->right = NULL;
			new_leaf2->height = 0;

			insert_position2->left = new_leaf2;
			insert_position2->right = old_leaf2;
			old_leaf2->back = insert_position2;
			new_leaf2->back = insert_position2;
			insert_position2->height = 1;

			//find insert position in tree1
			o_t *insert_position = root;
			while (insert_position->right != NULL) {
				path_stack[path_st_p++] = insert_position;
				if (a < insert_position->key)
					insert_position = insert_position->left;
				else
					insert_position = insert_position->right; 
			}

			o_t *old_leaf, *new_leaf;
			old_leaf = get_node();
			old_leaf->left = insert_position->left;
			old_leaf->key = insert_position->key;
			old_leaf->right = NULL;
			old_leaf->height = 0;
			new_leaf = get_node();
			new_leaf->left = (o_t *) insobj;
			new_leaf->key = a;
			new_leaf->right = NULL;
			new_leaf->height = 0;

			if(insert_position->key < a) {
				insert_position->left = old_leaf;
				insert_position->right = new_leaf;
				insert_position->key = a;
			}else {
				insert_position->left = new_leaf;
				insert_position->right = old_leaf;
			}
			insert_position->height = 1;

			// linked tree 1 and tree2;
			insert_position->back = NULL;
			old_leaf->back = old_leaf2;
			new_leaf->back = new_leaf2;

		}// end of insert a to tree2
		/*rebalance */
		// rebalance tree1;
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
      }//finished rebalance tree1;

      //rebalance tree2
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
      //finished rebalance tree2
	}// end non empty tree insert
	return;
}


void insert_bottom(o_t *ord, key_t a) {
	int *insobj = (int *) malloc(sizeof(int));
    *insobj = a;
    int *insobj2 = (int *) malloc(sizeof(int));
    *insobj2 = a;

	o_t *root, *root2;

	root = ord;
	root2 = ord;
	while (root2->right != NULL) {
		root2 = root2->right;
	}
	root2 = root2->back;
	while (root2->back != NULL) {
		root2 = root2->back;
	}
	// insert tree2 first, then insert tree1, then linked together
	// empty tree
	if (root->left == NULL) {
		// insert tree2 first
		root2->left = (o_t *) insobj2;
		root2->key = 0;
		root2->height = 0;
		root2->right = NULL;

		root->left = (o_t *) insobj;
		root->key = a;
		root->height = 0;
		root->right = NULL;

		root->back = root2;
	}// end empty tree
	else {
		// check contains a or not
		if (find(ord, a) != NULL) {
			return;
		}
		// find insert position first, then insert tree2, insert tree1, then link
		o_t *path_stack[100], *path_stack2[100];
		int path_st_p = 0, path_st_p2 = 0;
		int finished, finished2;

		o_t *insert_position2 = root2;
		while (insert_position2->right != NULL) {
			path_stack2[path_st_p2++] = insert_position2;
			insert_position2 = insert_position2->right;
		}

		{//insert a to tree2
			o_t *old_leaf2, *new_leaf2;
			old_leaf2 = get_node();
			old_leaf2->left = insert_position2->left;
			old_leaf2->key = 0;
			old_leaf2->right = NULL;
			old_leaf2->height = 0;
			new_leaf2 = get_node();
			new_leaf2->left = (o_t *) insobj2;
			new_leaf2->key = 0;
			new_leaf2->right = NULL;
			new_leaf2->height = 0;

			insert_position2->left = old_leaf2;
			insert_position2->right = new_leaf2;
			old_leaf2->back = insert_position2;
			new_leaf2->back = insert_position2;
			insert_position2->height = 1;

			//find insert position in tree1
			o_t *insert_position = root;
			while (insert_position->right != NULL) {
				path_stack[path_st_p++] = insert_position;
				if (a < insert_position->key)
					insert_position = insert_position->left;
				else
					insert_position = insert_position->right; 
			}

			o_t *old_leaf, *new_leaf;
			old_leaf = get_node();
			old_leaf->left = insert_position->left;
			old_leaf->key = insert_position->key;
			old_leaf->right = NULL;
			old_leaf->height = 0;
			new_leaf = get_node();
			new_leaf->left = (o_t *) insobj;
			new_leaf->key = a;
			new_leaf->right = NULL;
			new_leaf->height = 0;

			if(insert_position->key < a) {
				insert_position->left = old_leaf;
				insert_position->right = new_leaf;
				insert_position->key = a;
			}else {
				insert_position->left = new_leaf;
				insert_position->right = old_leaf;
			}
			insert_position->height = 1;

			// linked tree 1 and tree2;
			insert_position->back = NULL;
			old_leaf->back = old_leaf2;
			new_leaf->back = new_leaf2;

		}// end of insert a to tree2
		/*rebalance */
		// rebalance tree1;
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
      }//finished rebalance tree1;

      //rebalance tree2
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
      //finished rebalance tree2
	}// end non empty tree insert
	return;
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
			return;
		}else {
			return;
		}
	}else {
		o_t *path_stack[100], *path_stack2[100];
		int path_st_p = 0;
		int path_st_p2 = 0;

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

			if (tmp_node2 == upper_node2->left)
				other_node2 = upper_node2->right;
			else
				other_node2 = upper_node2->left;


			upper_node2->left = other_node2->left;
			upper_node2->right = other_node2->right;
			other_node2->back = upper_node2->back;
			upper_node2->height = other_node2->height;

			//delete a in tree 1
			upper_node->key = other_node->key;
			upper_node->left = other_node->left;
			upper_node->right = other_node->right;
			upper_node->height = other_node->height;


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
