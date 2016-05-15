#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef int object_t;
typedef int key_t;

typedef struct tr_n_t { key_t             key;
                     struct tr_n_t      *left;
                     struct tr_n_t     *right;
                     enum {red, black}  color; 
                /* possibly other information */ } tree_node_t;




tree_node_t *currentblock = NULL;
int    size_left;
tree_node_t *free_list = NULL;

tree_node_t *get_node()
{ tree_node_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> left;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = 
                (tree_node_t *) malloc( BLOCKSIZE * sizeof(tree_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}


void return_node(tree_node_t *node)
{  node->left = free_list;
   free_list = node;
}


tree_node_t *create_tree(void)
{  tree_node_t *tmp_node;
   tmp_node = get_node();
   tmp_node->left = NULL;
   tmp_node->color = black;
   return( tmp_node );
}

void left_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
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

void right_rotation(tree_node_t *n)
{  tree_node_t *tmp_node;
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

object_t *find(tree_node_t *tree, key_t query_key)
{  tree_node_t *tmp_node;
   if( tree->left == NULL )
     return(NULL);
   else
   {  tmp_node = tree;
      while( tmp_node->right != NULL )
      {   if( tmp_node->color == red ) printf("r"); else printf("b");
          if( query_key < tmp_node->key )
               tmp_node = tmp_node->left;
          else
               tmp_node = tmp_node->right;
      }
      if( tmp_node->color == red ) printf("r\n"); else printf("b\n");
      if( tmp_node->key == query_key )
         return( (object_t *) tmp_node->left );
      else
         return( NULL );
   }
}


int insert(tree_node_t *tree, key_t new_key, object_t *new_object)
{  if( tree->left == NULL )
   {  tree->left = (tree_node_t *) new_object;
      tree->key  = new_key;
      tree->color = black; /* root is always black */
      tree->right  = NULL; 
   }
   else
   {  tree_node_t *current, *next_node, *upper;
      current = tree;
      upper = NULL; 
      while( current->right != NULL )
      {  if( new_key < current->key )
            next_node = current->left;
         else
            next_node = current->right;
         
         if( current->color == black )
	      {  if( current->left->color == black || current->right->color == black )
	         {  
               upper = current; current = next_node;
            }
            else /* current->left and current->right red */ 
	         {  /* need rebalance */
	            if( upper == NULL ) /* current is root */
	            {  
                  current->left->color = black;
                  current->right->color = black;
                  upper = current;
               }
               else if (current->key < upper->key )
	            {  /* current left of upper */
                  if( current == upper->left )
	               {  
                     current->left->color = black;
                     current->right->color = black;
                     current->color = red;
                  }
                  else if( current == upper->left->left )
                  {  right_rotation( upper );
                     upper->left->color = red;
                     upper->right->color = red;
                     upper->left->left->color = black;
                     upper->left->right->color = black;
                  }
                  else /* current == upper->left->right */
                  {  left_rotation( upper->left );
                     right_rotation( upper );
                     upper->left->color = red;
                     upper->right->color = red;
                     upper->right->left->color = black;
                     upper->left->right->color = black;
                  }
               }
	            else /* current->key >= upper->key */
	            {  /* current right of upper */
                  if( current == upper->right )
	               {  
                     current->left->color = black;
                     current->right->color = black;
                     current->color = red;
                  }
                  else if( current == upper->right->right )
                  {  left_rotation( upper );
                     upper->left->color = red;
                     upper->right->color = red;
                     upper->right->left->color = black;
                     upper->right->right->color = black;
                  }
                  else /* current == upper->right->left */
                  {  right_rotation( upper->right );
                     left_rotation( upper );
                     upper->left->color = red;
                     upper->right->color = red;
                     upper->right->left->color = black;
                     upper->left->right->color = black;
                  }
               } /* end rebalancing */
               current = next_node;
	            upper = current; /*two black lower neighbors*/
            }
         }   
         else /* current red */
         {  
            current = next_node; /*move down */
         }
      } /* end while; reached leaf. always arrive on black leaf*/
      /* found the candidate leaf. Test whether key distinct */
      if( current->key == new_key )
         return( -1 );
      /* key is distinct, now perform the insert */ 
      {  tree_node_t *old_leaf, *new_leaf;
         old_leaf = get_node();
         old_leaf->left = current->left; 
         old_leaf->key = current->key;
         old_leaf->right  = NULL;
         old_leaf->color = red;
         new_leaf = get_node();
         new_leaf->left = (tree_node_t *) new_object; 
         new_leaf->key = new_key;
         new_leaf->right  = NULL;
         new_leaf->color = red;
         if( current->key < new_key )
         {   current->left  = old_leaf;
             current->right = new_leaf;
             current->key = new_key;
         } 
         else
         {   current->left  = new_leaf;
             current->right = old_leaf;
         } 
      } 
   }
   return( 0 );
}



object_t *delete(tree_node_t *tree, key_t delete_key)
{  object_t *deleted_object; int finished = 0;
   if( tree->left == NULL )
      return( NULL );
   else if( tree->right == NULL )
   {  if(  tree->key == delete_key )
      {  deleted_object = (object_t *) tree->left;
         tree->left = NULL;
         return( deleted_object );
      }
      else
         return( NULL );
   }
   else
   {  tree_node_t *current, *upper;
      tree->color = black; /* root is always black*/
      upper = tree;
      if( upper->left->color == black && upper->right->color == black )
      {  /* need to give upper ared lower neighbor */
	      if( delete_key < upper->key )
	      {  if( upper->left->right == NULL )  
	           {  if( upper->right->right == NULL )
	               {  
                     upper->left->color  = red;
	                  upper->right->color = red;
	               } 
	              else
      	        {  
                     upper->right->left->color  = black;
      	            upper->right->right->color = black;
                     upper->right->color = red;
      	        } 

	           }
	           else
	           {  if( upper->left->left->color == red || upper->left->right->color == red )
	                  upper = upper->left;
	              else if ( upper->right->right->color == red )
	              {   left_rotation( upper );
		               upper->right->color = black;
                     upper->left->color  = black;
                     upper->left->left->color = red;
                     upper = upper->left;
                  }
	              else if ( upper->right->left->color == red )
	              {   right_rotation( upper->right );
                     left_rotation( upper );               
		               upper->right->color = black;
                     upper->left->color  = black;
                     upper->left->left->color = red;
                     upper = upper->left;
                  }
	              else /* left and right have only black lower neighbors */
	              {  upper->left->color = red;
		              upper->right->color = red; 
                 }
               }
         }
         else /* delete_key >= upper->key */
	     {  
            if (upper->right->right == NULL) {
               if (upper->right->right == NULL) {
                  upper->left->color  = red;
                  upper->right->color = red;
               }
            }else if( upper->right->left == NULL )  
	        {  if( upper->left->right == NULL )
	           {  upper->left->color  = red;
	              upper->right->color = red;
	           } 
	           else
	           {  upper->left->left->color  = black;
	              upper->left->right->color = black;
                 upper->left->color = red;
	           } 

	        }
	        else
	        {  if( upper->right->right->color == red || upper->right->left->color == red )
	               upper = upper->right;
	           else if ( upper->left->left->color == red )
	           {   right_rotation( upper );
		            upper->right->color = black;
                  upper->left->color  = black;
                  upper->right->right->color = red;
                  upper = upper->right;
               }
	            else if ( upper->left->right->color == red )
	            {  left_rotation( upper->left );
                  right_rotation( upper );               
		            upper->right->color = black;
                  upper->left->color  = black;
                  upper->right->right->color = red;
                  upper = upper->right;
               }
	           else /* left and right have only black lower neighbors */
	           {  upper->left->color = red;
		           upper->right->color = red; 
               }
            }
         }
      }  /* upper has at least one red lower neighbor */
      current = upper;
      while( current->right != NULL )
      {  if( delete_key < current->key )
            current = current->left;
         else
            current = current->right;
         if( current->color == red || current->right == NULL )
            continue; /* go on down, or stop if leaf found */
         else /* current->color == black, not leaf, rebalance */
	      {  if( current->left->color  == red || current->right->color == red )
	         {  upper = current; /* condition satisfied */
            }
            else /* current->left and current->right black */ 
	         {  /* need rebalance */
               if (current->key < upper->key )
	            {  /* Case 2: current left of upper */
                  if( current == upper->left )
		            {  if( upper->right->left->left->color == black &&
			               upper->right->left->right->color == black )
 		               {  /* Case 2.1.1 */
                        left_rotation( upper );
		                  upper->left->color = black;
		                  upper->left->left->color = red;
		                  upper->left->right->color = red;
                        current = upper = upper->left;
                     }
		               else if( upper->right->left->left->color == red )
		               {  /* Case 2.1.2 */
                        right_rotation( upper->right->left );
		                  right_rotation( upper->right );
                        left_rotation( upper );
         		         upper->left->color = black;
         		         upper->right->left->color = black;
         		         upper->right->color = red;
         		         upper->left->left->color = red;
                        current = upper = upper->left;
                     }
		               else /* upper->right->left->left->color == black &&
			            upper->right->left->right->color == red */
		               {  /* Case 2.1.3 */
		                  right_rotation( upper->right );
                        left_rotation( upper );
		                  upper->left->color = black;
		                  upper->right->left->color = black;
		                  upper->right->color = red;
		                  upper->left->left->color = red;
                        current = upper = upper->left;
                     }
                  } /*end Case 2.1: current==upper->left */
                  else if( current == upper->left->left )
		            {  if( upper->left->right->left->color == black &&
			               upper->left->right->right->color == black )
 		               {  /* Case 2.2.1 */
		                  upper->left->color = black;
		                  upper->left->left->color = red;
		                  upper->left->right->color = red;
                        current = upper = upper->left;
                     }
		               else if( upper->left->right->right->color == red )
		               {  /* Case 2.2.2 */
                        left_rotation( upper->left );
		                  upper->left->left->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->left->left->left->color = red;
                        current = upper = upper->left->left;
                     }
		               else /* upper->left->right->left->color == red &&
			            upper->left->right->right->color == black */
		               {  /* Case 2.2.3 */
		                  right_rotation( upper->left->right );
                        left_rotation( upper->left );
		                  upper->left->left->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->left->left->left->color = red;
                        current = upper = upper->left->left;
                     }
                  } /*end Case 2.2: current==upper->left->left */
                  else /* current == upper->left->right */
		            {  if( upper->left->left->left->color == black &&
			               upper->left->left->right->color == black )
 		               {  /* Case 2.3.1 */
		                  upper->left->color = black;
		                  upper->left->left->color = red;
		                  upper->left->right->color = red;
                        current = upper = upper->left;
                     }
		               else if( upper->left->left->left->color == red )
		               {  /* Case 2.3.2 */
                        right_rotation( upper->left );
		                  upper->left->left->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->left->right->right->color = red;
                        current = upper = upper->left->right;
                     }
		               else /* upper->left->left->left->color == black &&
			              upper->left->left->right->color == red */
		               {  /* Case 2.3.3 */
                        left_rotation( upper->left->left );
		                  right_rotation( upper->left );
		                  upper->left->left->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->left->right->right->color = red;
                        current = upper = upper->left->right;
                     }
                  } /*end Case 2.3: current==upper->left->right */
               } /* end Case 2: current->key < upper-> key */
               else 
	           {  /* Case 3: current->key >= upper->key */
                  if( current == upper->right )
		            {  if( upper->left->right->right->color == black &&
			               upper->left->right->left->color == black )
 		               {  /* Case 3.1.1 */
                        right_rotation( upper );
		                  upper->right->color = black;
		                  upper->right->right->color = red;
		                  upper->right->left->color = red;
                        current = upper = upper->right;
                     }
		               else if( upper->left->right->right->color == red )
		               {  /* Case 3.1.2 */
                        left_rotation( upper->left->right );
		                  left_rotation( upper->left );
                        right_rotation( upper );
		                  upper->right->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->right->right->color = red;
                        current = upper = upper->right;
                     }
		               else /* upper->left->right->right->color == black &&
			              upper->left->right->left->color == red */
		               {  /* Case 3.1.3 */
		                  left_rotation( upper->left );
                        right_rotation( upper );
		                  upper->right->color = black;
		                  upper->left->right->color = black;
		                  upper->left->color = red;
		                  upper->right->right->color = red;
                        current = upper = upper->right;

                     }
                  } /*end Case 3.1: current==upper->left */
                  else if( current == upper->right->right )
		            {  if( upper->right->left->right->color == black &&
			               upper->right->left->left->color == black )
 		               {  /* Case 3.2.1 */
		                  upper->right->color = black;
		                  upper->right->left->color = red;
		                  upper->right->right->color = red;
                        current = upper = upper->right;
                     }
		               else if( upper->right->left->left->color == red )
		               {  /* Case 3.2.2 */
                        right_rotation( upper->right );
		                  upper->right->left->color = black;
		                  upper->right->right->color = black;
		                  upper->right->color = red;
		                  upper->right->right->right->color = red;
                        current = upper = upper->right->right;
                     }
		               else /* upper->right->left->right->color == red &&
			              upper->right->left->left->color == black */
		               {  /* Case 3.2.3 */
		                  left_rotation( upper->right->left );
                        right_rotation( upper->right );
		                  upper->right->right->color = black;
		                  upper->right->left->color = black;
		                  upper->right->color = red;
		                  upper->right->right->right->color = red;
                        current = upper = upper->right->right;
                     }
                  } /*end Case 3.2: current==upper->right->right */
                  else /* current == upper->right->left */
		            {  if( upper->right->right->right->color == black &&
			               upper->right->right->left->color == black )
 		               {  /* Case 3.3.1 */
		                  upper->right->color = black;
		                  upper->right->left->color = red;
		                  upper->right->right->color = red;
                        current = upper = upper->right;
                     }
		               else if( upper->right->right->right->color == red )
		               {  /* Case 3.3.2 */
                        left_rotation( upper->right );
		                  upper->right->left->color = black;
		                  upper->right->right->color = black;
		                  upper->right->color = red;
		                  upper->right->left->left->color = red;
                        current = upper = upper->right->left;
                     }
		               else /* upper->right->right->right->color == black &&
			              upper->right->right->left->color == red */
		               {  /* Case 3.3.3 */
                        right_rotation( upper->right->right );
		                  left_rotation( upper->right );
		                  upper->right->left->color = black;
		                  upper->right->right->color = black;
		                  upper->right->color = red;
		                  upper->right->left->left->color = red;
                        current = upper = upper->right->left;
                     }
                  } /*end Case 3.3: current==upper->right->left */
               } /* end Case 3: current->key >= upper-> key */
	    } /* end rebalance, upper has a red lower neighbor */
         }
      }  /* end while */
      /* found the candidate leaf. Test whether key correct */
      if( current->key != delete_key )
         return( NULL );
      else /* want to delete node current */
      {  tree_node_t *tmp_node;
         deleted_object = (object_t *) current->left;
         if( current->key < upper->key )         
	      {  if( current == upper->left )
	         {  /* upper->right is red */
               tmp_node = upper->right;            
               upper->key   = tmp_node->key;
               upper->left  = tmp_node->left;
               upper->right = tmp_node->right;
            }
	         else if ( current == upper->left->left )
	         {  /* upper->left is red */
               tmp_node = upper->left;            
               upper->left  = tmp_node->right;
            }
 	         else /* current == upper->left->right */
	         {  /* upper->left is red */
               tmp_node = upper->left;            
               upper->left  = tmp_node->left;
            }
         } /* end current->key < upper->key */
         else /* leaf to the right, current->key >= upper->key */
	      {  if( current == upper->right )
	         {  /* upper->left is red */
               tmp_node = upper->left;            
               upper->key   = tmp_node->key;
               upper->left  = tmp_node->left;
               upper->right = tmp_node->right;
            }
	         else if ( current == upper->right->right )
	         {  /* upper->right is red */
               tmp_node = upper->right;            
               upper->right  = tmp_node->left;
            }
 	         else /* current == upper->right->left */
	         {  /* upper->right is red */
               tmp_node = upper->right;            
               upper->right  = tmp_node->right;
            }
         } /* end current->key >= upper->key */
         return_node( tmp_node );
         return_node( current );
      }
      return( deleted_object );
   }
}


void check_tree( tree_node_t *tr, int count, int lower, int upper )
{  if( tr->left == NULL )
   {  printf("Tree Empty\n"); return; }
   if( tr->key < lower || tr->key >= upper )
   {      printf("Wrong Key Order: node key %d, lower %d, upper %d \n",
               tr->key, lower, upper);
   }
   if( tr->right == NULL ) /* leaf */
   {  //if( *( (int *) tr->left) == 10*tr->key + 2 )
         printf("%d(%d)  ", tr->key, count+(tr->color==black) );
      // else
      //    printf("Wrong Object \n");
   }
   else /* not leaf */
   {  if( tr->color == red )
      {  if(  tr->left->color == red ) 
	   printf("wrong color below red key %d on the left\n", tr->key );
         if(  tr->right->color == red ) 
	   printf("wrong color below red key %d on the right\n", tr->key );
      }
      check_tree(tr->left, count + (tr->color==black), lower, tr->key ); 
      check_tree(tr->right, count+ (tr->color==black), tr->key, upper ); 
   }
}

key_t find_min(tree_node_t *tree) {
   tree_node_t *tmp_node = tree;
   while (tmp_node->right != NULL) {
      tmp_node = tmp_node->left;
   }
   return tmp_node->key;
}

key_t find_max(tree_node_t *tree) {
   tree_node_t *tmp_node = tree;
   while (tmp_node->right != NULL) {
      tmp_node = tmp_node->right;
   }
   return tmp_node->key;
}

int check_height(tree_node_t *tree) {
   int count = 0;
   if (tree->left == NULL) {
      return 0;
   }

   while (tree->right != NULL) {
      if (tree->color == black) {
         count++;
      }
      tree = tree->right;
   }
   if (tree->color == black) {
      count++;
   }
   return count;
}


void join(tree_node_t *tree1, tree_node_t *tree2) {
   if (tree2->left == NULL) {
      return;
   }

   if (tree1->left == NULL) {
      tree1->key = tree2->key;
      tree1->left = tree2->left;
      tree1->right = tree2->right;
      //tree1 = tree2;
      tree_node_t *empty_tree = create_tree();
      tree2->key = empty_tree->key;
      tree2->left = empty_tree->left;
      tree2->right = empty_tree->right;
      //tree2 = create_tree();
      return;
   }

   if (tree1->key > tree2->key) {
      tree_node_t *t = create_tree();
      t->key = tree2->key;
      t->left = tree2->left;
      t->right = tree2->right;
      t->color = tree2->color;

      tree2->key = tree1->key;
      tree2->left = tree1->left;
      tree2->right = tree1->right;
      tree2->color = tree1->color;

      //tree1 = t;
      tree1->key = t->key;
      tree1->left = t->left;
      tree1->right = t->right;
      tree1->color = t->color;
   }
   // each value in tree1 is less then each vaule in tree2
   int h1 = check_height(tree1);
   int h2 = check_height(tree2);
   tree_node_t *tree = create_tree();
   if (h1 == h2) {
      int *insobj1, *insobj2;
      insobj1 = (int *) malloc(sizeof(int));
      *insobj1 = 0;
      insert(tree,0, insobj1);
      insobj2 = (int *) malloc(sizeof(int));
      *insobj2 = 1;
      insert(tree,1, insobj2);
      tree->key = find_min(tree2);
      tree->left->key = tree1->key;
      tree->left->left = tree1->left;
      tree->left->right = tree1->right;
      tree->left->color = black;
      tree->right->key = tree2->key;
      tree->right->left = tree2->left;
      tree->right->right = tree2->right;
      tree->right->color = black;

      //tree1 = create_tree();
      tree1->key = tree->key;
      tree1->left = tree->left;
      tree1->left->color = tree->left->color;
      tree1->right = tree->right;
      tree1->right->color = tree->right->color;

      tree_node_t *empty_tree = create_tree();
      tree2->key = empty_tree->key;
      tree2->left = empty_tree->left;
      tree2->right = empty_tree->right;
      return;
   }else if (h1 > h2) {
      tree_node_t *tmp_node = tree1;
      tree_node_t *p = tmp_node;
      tree_node_t *g = p;
      // tmp_node = tmp_node->right;
      while (check_height(tmp_node) != h2) {
         g = p;
         p = tmp_node;
         tmp_node = tmp_node->right;
      }

      while (tmp_node->color != black) {
         g = p;
         p = tmp_node;
         tmp_node = tmp_node->right;
      }
      tree_node_t *tmp_tree = create_tree();
      int *insobj1, *insobj2;
      insobj1 = (int *) malloc(sizeof(int));
      *insobj1 = 0;
      insert(tmp_tree,0, insobj1);
      insobj2 = (int *) malloc(sizeof(int));
      *insobj2 = 1;
      insert(tmp_tree,1, insobj2);

      tmp_tree->key = find_min(tree2);
      tmp_tree->color = red;
      //tmp_tree->left = tmp_node;
      tmp_tree->left->key = tmp_node->key;
      tmp_tree->left->left = tmp_node->left;
      tmp_tree->left->right = tmp_node->right;
      tmp_tree->left->color = black;
      //tmp_node->right = tree2;
      tmp_tree->right->key = tree2->key;
      tmp_tree->right->left = tree2->left;
      tmp_tree->right->right = tree2->right;
      tmp_tree->right->color = black;

      //p->right = tmp_tree;
      p->right->key = tmp_tree->key;
      p->right->left = tmp_tree->left;
      p->right->right = tmp_tree->right;
      p->right->color = tmp_tree->color;
      //tree = tree1;
      tree_node_t *empty_tree = create_tree();
      tree2->key = empty_tree->key;
      tree2->left = empty_tree->left;
      tree2->right = empty_tree->right;
      //fix color 
      if (p->color == red) {
         printf("after rebalance\n");
         if (g->left->color == black) {
            left_rotation(g);
            g->color = black;
            g->left->color = red;
            g->right->color = red;
         }else {
            right_rotation(g);
            g->color = red;
            g->left->color = black;
            g->right->color = red;
            if (g == tree1)
               g->color = black;
         }

      }

   }else {
      // h1 < h2
      tree_node_t *tmp_node = tree2;
      tree_node_t *p = tmp_node;
      tree_node_t *g = p;
      while (check_height(tmp_node) != h1) {
         g = p;
         p = tmp_node;
         tmp_node = tmp_node->left;
      }

      while (tmp_node->color != black) {
         g = p;
         p = tmp_node;
         tmp_node = tmp_node->left;
      }
      tree_node_t *tmp_tree = create_tree();
      int *insobj1, *insobj2;
      insobj1 = (int *) malloc(sizeof(int));
      *insobj1 = 0;
      insert(tmp_tree,0, insobj1);
      insobj2 = (int *) malloc(sizeof(int));
      *insobj2 = 1;
      insert(tmp_tree,1, insobj2);

      tmp_tree->key = find_min(tree2);
      tmp_tree->color = red;
      //tmp_tree->left = tmp_node;
      tmp_tree->right->key = tmp_node->key;
      tmp_tree->right->left = tmp_node->left;
      tmp_tree->right->right = tmp_node->right;
      tmp_tree->right->color = black;
      //tmp_node->right = tree2;
      tmp_tree->left->key = tree1->key;
      tmp_tree->left->left = tree1->left;
      tmp_tree->left->right = tree1->right;
      tmp_tree->left->color = black;

      //p->left = tmp_tree;
      p->left->key = tmp_tree->key;
      p->left->left = tmp_tree->left;
      p->left->right = tmp_tree->right;
      p->left->color = tmp_tree->color;
      //tree = tree2;
      tree1->key = tree2->key;
      tree1->left = tree2->left;
      tree1->right = tree2->right;

      tree_node_t *empty_tree = create_tree();
      tree2->key = empty_tree->key;
      tree2->left = empty_tree->left;
      tree2->right = empty_tree->right;
      //fix color 
      if (p->color == red) {
         printf("after rebalance\n");
         if (g->right->color == black) {
            right_rotation(g);
            g->color = black;
            g->left->color = red;
            g->right->color = red;
         }else {
            left_rotation(g);
            g->color = red;
            g->left->color = black;
            g->right->color = black;
            if (g == tree1)
               g->color = black;
         }

      }
   }
   return;
}


tree_node_t *split(tree_node_t *tree, key_t x) {
   // x don't need to be in tree
   tree_node_t *tree1, *tree2;
   tree1 = create_tree();
   tree2 = create_tree();

   if (tree->left == NULL) {
      return tree1;
   }
   if (x <= find_min(tree)) {
      return tree1;
   }

   if (x > find_max(tree)) {
      //tree1 = tree;
      tree1->key = tree->key;
      tree1->left = tree->left;
      tree1->right = tree->right;

      tree->key = tree2->key;
      tree->left = tree2->left;
      tree->right = tree2->right;
      tree->color = tree2->color;
      return tree1;
   }
   if (x == find_max(tree)) {
      tree1->key = tree->key;
      tree1->left = tree->left;
      tree1->right = tree->right;
      delete(tree1,x);
      int *insobj = (int *) malloc(sizeof(int));
      *insobj = 10*x+2;
      insert(tree2,x,insobj);
      tree->key = tree2->key;
      tree->left = tree2->left;
      tree->right = tree2->right;
      tree->color = tree2->color;
      return tree1;
   }
   tree_node_t *tmp_node = tree;
   while (tmp_node->right != NULL) {
      if (x < tmp_node->key) {
         tmp_node->right->color = black;
         join(tree2, tmp_node->right);
         tmp_node = tmp_node->left;
      }else {
         tmp_node->left->color = black;
         join(tree1, tmp_node->left);
         tmp_node = tmp_node->right;
      }
   }

   if (tmp_node->key < x) {
      insert(tree1,tmp_node->key,(object_t *)tmp_node->left);
   }else {
      insert(tree2,tmp_node->key,(object_t *)tmp_node->left);
   }

   tree->key = tree2->key;
   tree->left = tree2->left;
   tree->right = tree2->right;
   tree->color = tree2->color;   
   return tree1;
}


int main()
{  tree_node_t *searchtree;
   char nextop;

   // tree_node_t *new_tree= create_tree();
   // searchtree = create_tree();
   // for (int i = 0; i < 12; i++) {
   //    int *insobj = (int *) malloc(sizeof(int));
   //    *insobj = 10*i+2;
   //    insert(searchtree,i, insobj);
   // }

   tree_node_t *tree1;
   //tree_node_t *tree2;
   tree1 = create_tree();
   //tree2 = create_tree();

   for (int i = 0; i < 19; i++) {
      int *insobj = (int *) malloc(sizeof(int));
      *insobj = 10*i+2;
      insert(tree1,i,insobj);
   }

   printf("before split\n");
   printf("tree1: ");
   check_tree(tree1,0,-100000,100000);
   printf("\n");
   // printf("tree2: ");
   // check_tree(tree2,0,-100000,100000);
   // printf("\nafter join.........\n");

   tree_node_t *tree2 = split(tree1,8);

   printf("\nafter split\n");
   printf("tree2: ");
   check_tree(tree2,0,-100000,100000);
   printf("\n");
   printf("tree1: ");
   check_tree(tree1,0,-100000,100000);
   printf("\n......\n");
   // for (int i = 0; i < 30; i++) {
   //    int *insobj = (int *) malloc(sizeof(int));
   //    *insobj = 10*i+2;
   //    insert(tree1,i, insobj);
   // }
   // for (int i = 31; i < 42; i++) {
   //    int *insobj = (int *) malloc(sizeof(int));
   //    *insobj = 10*i+2;
   //    insert(tree2,i, insobj);
   // }
   // printf("before check\n");
   // printf("tree1: ");
   // check_tree(tree1,0,-100000,100000);
   // printf("\n");
   // printf("tree2: ");
   // check_tree(tree2,0,-100000,100000);
   // printf("\nafter join.........\n");

   // join(tree1,tree2);

   // printf("tree1: ");
   // check_tree(tree1,0,-100000,100000);
   // printf("\n");
   // printf("tree2: ");
   // check_tree(tree2,0,-100000,100000);
   // printf("\n");

   // check_tree(searchtree,0,-100000,100000);
   // printf("Made Tree: Red-Black Tree with Top-Down Rebalancing\n");
   // while( (nextop = getchar())!= 'q' )
   // { if( nextop == 'i' )
   //   { int inskey,  *insobj, success;
   //     insobj = (int *) malloc(sizeof(int));
   //     scanf(" %d", &inskey);
   //     *insobj = 10*inskey+2;
   //     success = insert( searchtree, inskey, insobj );
   //     if ( success == 0 )
   //       printf("  insert successful, key = %d, object value = %d,\n",
   //      	  inskey, *insobj );
   //     else
   //         printf("  insert failed, success = %d\n", success);
   //   }  
   //   if( nextop == 'f' )
   //   { int findkey, *findobj;
   //     scanf(" %d", &findkey);
   //     findobj = find( searchtree, findkey);
   //     if( findobj == NULL )
   //       printf("  find failed, for key %d\n", findkey);
   //     else
   //       printf("  find successful, found object %d\n", *findobj);
   //   }
   //   if( nextop == 'd' )
   //   { int delkey, *delobj;
   //     scanf(" %d", &delkey);
   //     delobj = delete( searchtree, delkey);
   //     if( delobj == NULL )
   //       printf("  delete failed for key %d\n", delkey);
   //     else
   //       printf("  delete successful, deleted object %d\n", 
   //           *delobj);
   //   }
   //   if (nextop == 's') {
   //       int splitting_key;
   //       scanf(" %d", &splitting_key);
   //       new_tree = split(searchtree,splitting_key);
   //       printf("key in root is %d,\n", searchtree->key);
   //       check_tree(searchtree,0,-100000,100000);
   //       printf("\n............\n");
   //       if (new_tree != NULL){
   //          printf("\nkey in new_tree root is %d,\n", new_tree->key);
   //          check_tree(new_tree,0,-100000,100000);
   //       }
   //       printf("\n  Finished Splitting tree\n"); 
   //   }
   //   if( nextop == '?' )
   //   {  printf("  Checking tree\n"); 
   //      check_tree(searchtree,0,-100000,100000);
   //      printf("\n");
   //      if( searchtree->left != NULL )
	  // printf("key in root is %d,\n", searchtree->key);
   //      printf("  Finished Checking tree\n"); 
   //   }
   // }
   // return(0);
}
