/* this program use Professor Peter Brass's heignt balance tree code as example
* author: Xue Gong
* date: March 1 2016
*/

#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef char object_t;
typedef struct text_s {
	int key;
	struct text_s *left;
	struct text_s *right;
	int height;
} text_t;

text_t *currentblock = NULL;
int size_left;
text_t *free_list = NULL;

text_t *get_node(void) {
	text_t *tmp;
	if( free_list != NULL ) {
		tmp = free_list;
		free_list = free_list->left;
	} else{
		if(currentblock == NULL || size_left == 0) {
			currentblock = (text_t *) malloc( BLOCKSIZE * sizeof(text_t));
			size_left = BLOCKSIZE;
		}
		tmp = currentblock++;
		size_left -= 1;
	}
	return tmp;
}

void return_node(text_t *txt) {
	txt->left = free_list;
	free_list = txt;
}

text_t *create_text() {
	text_t *tmp_node;
	tmp_node = get_node();
	tmp_node->left = NULL;
	tmp_node->key = 0;
	return tmp_node;
}

void left_rotation(text_t *txt) {
	text_t *tmp_node;
	int tmp_key;
	tmp_node = txt->left;
	tmp_key = txt->key;
	txt->left = txt->right;
	txt->key = txt->right->key;
	txt->right = txt->left->right;
	txt->left->right = txt->left->left;
	txt->left->left = tmp_node;
	txt->left->key = tmp_key;
}

void right_rotation(text_t *txt) {
	text_t *tmp_node;
	int tmp_key;
	tmp_node = txt->right;
	tmp_key = txt->key;
	txt->right = txt->left;
	txt->key = txt->left->key;
	txt->left = txt->right->left;
	txt->right->left = txt->right->right;
	txt->right->right = tmp_node;
	txt->right->key = tmp_key;
}

int length_text(text_t *txt) {
	if(txt == NULL) {
		return 0;
	}else {
		return txt->key;
	}
}

object_t *get_line(text_t *txt, int index) {
	text_t *tmp_node = txt;
	if(txt == NULL) {
		return NULL;
	}
	if(txt->key < index) {
		return NULL;
	}
	//search left or right until null, null means find
	while(tmp_node->right != NULL) {
		if(tmp_node->left->key >= index) {
			tmp_node = tmp_node->left;
		}else {
			index -= tmp_node->left->key;
			tmp_node = tmp_node->right;
		}
	}
	return (object_t *) tmp_node->left;

}


object_t *set_line(text_t *txt, int index, object_t *new_line) {
	text_t *tmp_node = txt;
	int new_index = index - 1;
	if(txt == NULL) {
		return NULL;
	}
	if(txt->key < index) {
		return NULL;
	}

	while(tmp_node->right != NULL) {
		if(tmp_node->left->key >= index) {
			tmp_node = tmp_node->left;
		}else {
			index -= tmp_node->left->key;
			tmp_node = tmp_node->right;
		}
	}
	tmp_node->left = (text_t *) new_line;
	return get_line(txt, new_index);

}


void append_line(text_t *txt, char *new_line){
	text_t *tmp_node;
	text_t *old_leaf, *new_leaf;

	int finished;
	text_t * path_stack[100];
	int  path_st_p;

	if(txt->left == NULL){
		txt->left = (text_t *)new_line;
		txt->key = 1;
		txt->height = 0;
		txt->right = NULL;
	}else{
		path_st_p = 0;
		tmp_node = txt;
		while(tmp_node->right != NULL){
			path_stack[path_st_p++] = tmp_node;
			tmp_node = tmp_node->right;
		}

		{
         old_leaf = get_node();
         old_leaf->left = tmp_node->left;
         old_leaf->key = tmp_node->key;
         old_leaf->right = NULL;
         old_leaf->height = 0;

         new_leaf = get_node();
         new_leaf->left = (text_t *)new_line;
         new_leaf->key = 1;
         new_leaf->right = NULL;
		 new_leaf->height = 0;

		 tmp_node->left = old_leaf;
		 tmp_node->right = new_leaf;
         tmp_node->key = old_leaf->key + new_leaf->key;
         tmp_node->height = 1;
		}

		// rebalance the tree
		finished = 0;
		while( path_st_p > 0 && !finished ){
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			// add new line update key
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;

			old_height= tmp_node->height;
			if( tmp_node->left->height - tmp_node->right->height == 2 ){
				if( tmp_node->left->left->height - tmp_node->right->height == 1 ) {
					right_rotation( tmp_node );
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				}else{
					left_rotation( tmp_node->left );
					right_rotation( tmp_node );
					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else if ( tmp_node->left->height - tmp_node->right->height == -2 ){
				if( tmp_node->right->right->height - tmp_node->left->height == 1 ){
					left_rotation( tmp_node );
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
					//add line to update key.
					tmp_node->key = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
				}else{
					right_rotation( tmp_node->right );
					left_rotation( tmp_node );
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else{
				if( tmp_node->left->height > tmp_node->right->height ){
					tmp_node->height = tmp_node->left->height + 1;
				}else{
					tmp_node->height = tmp_node->right->height + 1;
				}
			}
			if( tmp_node->height == old_height ){
				finished = 1;
			}
		}
		while( path_st_p > 0){
			tmp_node = path_stack[--path_st_p];
			// add new line update key
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;
		}
	}
}

void insert_line(text_t *txt, int index, char * new_line) {
	text_t *tmp_node;
	int finished;

	text_t * path_stack[100]; 
	int  path_st_p;	

	text_t *old_leaf, *new_leaf;
	int tmp_height, old_height;

	if( txt->left == NULL ){
		txt->left = (text_t *)new_line;
		txt->key  = 1;
		txt->height = 0;
		txt->right = NULL;
		return;
	}else{
		path_st_p = 0;
		tmp_node = txt;

		//append or not
		if(index > txt->key) {
			append_line(txt, new_line);
			return;
		}

		while( tmp_node->right != NULL ){
			path_stack[path_st_p++] = tmp_node;
			if(tmp_node->left->key >= index) {
			    tmp_node = tmp_node->left;
			}else {
			    index -= tmp_node->left->key;
			    tmp_node = tmp_node->right;
		    }
		}

		{
			old_leaf = get_node();
			old_leaf->left = tmp_node->left;
			old_leaf->key = tmp_node->key;
			old_leaf->right  = NULL;
			old_leaf->height = 0;

			new_leaf = get_node();
			new_leaf->left = (text_t *)new_line;
			new_leaf->key = 1;
			new_leaf->right  = NULL;
			new_leaf->height = 0;

			tmp_node->left  = new_leaf;
            tmp_node->right = old_leaf;
			tmp_node->key = old_leaf->key + new_leaf->key;
			tmp_node->height = 1;
		}

		//rebalance */
		finished = 0;
		while( path_st_p > 0 && !finished ){
			tmp_node = path_stack[--path_st_p];
			//update key here;
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;
			old_height = tmp_node->height;
			if( tmp_node->left->height - tmp_node->right->height == 2 ){
				if( tmp_node->left->left->height - tmp_node->right->height == 1 ){
					right_rotation( tmp_node );
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
					// update key
					tmp_node->key = tmp_node->right->key;
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
				}else{   // need change here !!!!!! or not
					left_rotation( tmp_node->left );
					int switch_tmp = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key;
					tmp_node->left->left->key = switch_tmp;

					right_rotation( tmp_node );
					tmp_node->key = tmp_node->right->key;
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;

					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else if ( tmp_node->left->height - tmp_node->right->height == -2 ){
				if( tmp_node->right->right->height - tmp_node->left->height == 1 ){
					left_rotation( tmp_node );
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
					//add line to update key.
					tmp_node->key = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
				}else{
					right_rotation( tmp_node->right );
					int switch_tmp = tmp_node->right->key;
					tmp_node->right->key = tmp_node->right->right->key;
					tmp_node->right->right->key = switch_tmp;
					left_rotation( tmp_node );
					tmp_node->key = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height  = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else{
				if( tmp_node->left->height > tmp_node->right->height )
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if( tmp_node->height == old_height )
				finished = 1;
		}

		while( path_st_p > 0){
			tmp_node = path_stack[--path_st_p];
			// add new line update key
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;
		}

	}
}


char *delete_line(text_t *txt, int index) {
	text_t *tmp_node, *upper_node, *other_node;
	object_t *deleted_object;
	int finished;
	text_t * path_stack[100];
	int path_st_p;
	int tmp_height, old_height;

	if(txt == NULL) {
		return NULL;
	}

	if(index > txt->key) {
		return NULL;
	}

	if(txt->right == NULL) {
		if(txt->key == index){
			deleted_object = (object_t *)txt->left;
			txt->left = NULL;
			txt->key = 0;
			return (deleted_object);
		}
	}else{
		path_st_p = 0;
		tmp_node = txt;

		while(tmp_node->right != NULL){
			path_stack[path_st_p++] = tmp_node;
			upper_node = tmp_node;
			if(tmp_node->left->key >= index) {
			    tmp_node = tmp_node->left;
			    other_node = upper_node->right;

			}else {
			    index -= tmp_node->left->key;
			    tmp_node = tmp_node->right;
			    other_node = upper_node->left;

		    }
		}

		{
			upper_node->key = other_node->key;
			upper_node->left = other_node->left;
			upper_node->right = other_node->right;
			upper_node->height = other_node->height;

			deleted_object = (object_t *) tmp_node->left;
			return_node(tmp_node);
			return_node(other_node);
		}

		//rebalance
		finished = 0;
		path_st_p -= 1;
		while(path_st_p > 0 && !finished){
			tmp_node = path_stack[--path_st_p];
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;

			old_height = tmp_node->height;
			if(tmp_node->left->height - tmp_node->right->height == 2 ){
				if(tmp_node->left->left->height - tmp_node->right->height == 1 ){
					right_rotation(tmp_node);
					tmp_node->key = tmp_node->right->key;
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				}else{
					left_rotation(tmp_node->left);
					right_rotation(tmp_node);
					tmp_node->key = tmp_node->right->key;
					tmp_node->right->key = tmp_node->right->left->key + tmp_node->right->right->key;

					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else if (tmp_node->left->height - tmp_node->right->height == -2 ){
				if( tmp_node->right->right->height - tmp_node->left->height == 1 ){
					left_rotation(tmp_node);
					//add line to update key.
					tmp_node->key = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;

					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				}else{
					right_rotation(tmp_node->right);
					left_rotation(tmp_node);
					tmp_node->key = tmp_node->left->key;
					tmp_node->left->key = tmp_node->left->left->key + tmp_node->left->right->key;
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			}else{
				if(tmp_node->left->height > tmp_node->right->height)
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if(tmp_node->height == old_height)
				finished = 1;
		}
		while( path_st_p > 0){
			tmp_node = path_stack[--path_st_p];
			// add new line update key
			tmp_node->key = tmp_node->left->key + tmp_node->right->key;
		}
		return (deleted_object);
   }
   return NULL;
}



int main()
{
}