#include <stdio.h>
#include <stdlib.h>

#define BLOCKSIZE 256

typedef char object_t;

//#define MAXP 46337  /* prime, and 46337*46337 < 2147483647 */
#define MAXP 15485863

typedef struct l_node {  char      *key;
                         object_t  *obj;
                   struct l_node  *next; } list_node_t;

typedef struct htp_l_node { int a; 
                   struct htp_l_node *next; } htp_l_node_t; 

typedef struct { int b;   int size; 
                 struct htp_l_node *a_list;} hf_param_t;

char b_array_0[250000];
char b_array_1[250000];
char b_array_2[250000];
char b_array_3[250000];
char b_array_4[250000];
char b_array_5[250000];
char b_array_6[250000];
char b_array_7[250000];

typedef struct bloom {
  hf_param_t param_0;
  hf_param_t param_1;
  hf_param_t param_2;
  hf_param_t param_3;
  hf_param_t param_4;
  hf_param_t param_5;
  hf_param_t param_6;
  hf_param_t param_7;
} bf_t;

bf_t new_bloom;


list_node_t *currentblock = NULL;
int    size_left;
list_node_t *free_list = NULL;

list_node_t *get_node()
{ list_node_t *tmp;
  if( free_list != NULL )
  {  tmp = free_list;
     free_list = free_list -> next;
  }
  else
  {  if( currentblock == NULL || size_left == 0)
     {  currentblock = 
                (list_node_t *) malloc( BLOCKSIZE * sizeof(list_node_t) );
        size_left = BLOCKSIZE;
     }
     tmp = currentblock++;
     size_left -= 1;
  }
  return( tmp );
}

void return_node(list_node_t *node)
{  node->next = free_list;
   free_list = node;
}

int universalhashfunction(char *key, hf_param_t hfp)
{  int sum;
   htp_l_node_t *al;
   sum = hfp.b;
   al = hfp.a_list;
   while( *key != '\0' )
   {  if( al->next == NULL )
      {   al->next = (htp_l_node_t *) get_node();
          al->next->next = NULL;
          al->a = rand()%MAXP;
      }
      sum += ( (al->a)*((int) *key))%MAXP;
      key += 1;
      al = al->next;
   }
   return ( abs(sum)%hfp.size );
}

bf_t * create_bf() {
  // create 8 bit arrays, each size is 250,000 char
  //bf_t *new_bloom;
  int size = 8;
  int i,j;
  for (j=0;j<250000;j++){
    b_array_0[j] = 0;
    b_array_1[j] = 0;
    b_array_2[j] = 0;
    b_array_3[j] = 0;
    b_array_4[j] = 0;
    b_array_5[j] = 0;
    b_array_6[j] = 0;
    b_array_7[j] = 0;
  }
  // for(i=0; i < size; i++) {
  //   new_bloom->param[i].b = rand()%MAXP;
  //   new_bloom->param[i].size = 2000000;
  //   new_bloom->param[i].a_list = (htp_l_node_t *) get_node();
  //   new_bloom->param[i].a_list->next = NULL;
  // }
  new_bloom.param_0.b = rand()%MAXP;
  new_bloom.param_0.size = 2000000;
  new_bloom.param_0.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_0.a_list->next = NULL;

  new_bloom.param_1.b = rand()%MAXP;
  new_bloom.param_1.size = 2000000;
  new_bloom.param_1.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_1.a_list->next = NULL;

  new_bloom.param_2.b = rand()%MAXP;
  new_bloom.param_2.size = 2000000;
  new_bloom.param_2.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_2.a_list->next = NULL;

  new_bloom.param_3.b = rand()%MAXP;
  new_bloom.param_3.size = 2000000;
  new_bloom.param_3.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_3.a_list->next = NULL;

  new_bloom.param_4.b = rand()%MAXP;
  new_bloom.param_4.size = 2000000;
  new_bloom.param_4.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_4.a_list->next = NULL;

  new_bloom.param_5.b = rand()%MAXP;
  new_bloom.param_5.size = 2000000;
  new_bloom.param_5.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_5.a_list->next = NULL;

  new_bloom.param_6.b = rand()%MAXP;
  new_bloom.param_6.size = 2000000;
  new_bloom.param_6.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_6.a_list->next = NULL;

  new_bloom.param_7.b = rand()%MAXP;
  new_bloom.param_7.size = 2000000;
  new_bloom.param_7.a_list = (htp_l_node_t *) get_node();
  new_bloom.param_7.a_list->next = NULL;
  return &new_bloom;
}

void insert_bf(bf_t *b, char *s) {
  int i=0;
  int h_result;

      h_result = universalhashfunction(s,b->param_0);
      b_array_0[h_result/8] |= 1 <<(h_result%8);

      h_result = universalhashfunction(s,b->param_1);
      b_array_1[h_result/8] |= 1 <<(h_result%8);

      h_result = universalhashfunction(s,b->param_2);
      b_array_2[h_result/8] |= 1 <<(h_result%8); 

      h_result = universalhashfunction(s,b->param_3);
      b_array_3[h_result/8] |= 1 <<(h_result%8);
      i++;

      h_result = universalhashfunction(s,b->param_4);
      b_array_4[h_result/8] |= 1 <<(h_result%8);
      i++;

      h_result = universalhashfunction(s,b->param_5);
      b_array_5[h_result/8] |= 1 <<(h_result%8);
      i++;

      h_result = universalhashfunction(s,b->param_6);
      b_array_6[h_result/8] |= 1 <<(h_result%8);
      i++;

      h_result = universalhashfunction(s,b->param_7);
      b_array_7[h_result/8] |= 1 <<(h_result%8);

  return;
}

int is_element(bf_t *b, char *q) {
  int i=0;
  int k;

      k = universalhashfunction(q,b->param_0);
      if ((b_array_0[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_1);
      if ((b_array_1[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_2);
      if ((b_array_2[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_3);
      if ((b_array_3[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_4);
      if ((b_array_4[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_5);
      if ((b_array_5[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_6);
      if ((b_array_6[k/8] & (1 << (k%8) )) == 0)
        return 0;

      k = universalhashfunction(q,b->param_7);
      if ((b_array_7[k/8] & (1 << (k%8) )) == 0)
        return 0;

  return 1;
}

void sample_string_A(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = 'b';
   s[5] = (char)(1 + ((i*(i-3)) %217));
   s[6] = (char)(1 + ((17*i+129)%233 ));
   s[7] = '\0';
}
void sample_string_B(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = (char)(1 + ((i*(i-3)) %217));
   s[5] = (char)(1 + ((17*i+129)%233 ));
   s[6] = '\0';
}
void sample_string_C(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = 'a';
   s[3] = (char)(1 + ((i*(i-3)) %217));
   s[4] = (char)(1 + ((17*i+129)%233 ));
   s[5] = '\0';
}
void sample_string_D(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'b';
   s[4] = 'b';
   s[5] = (char)(1 + ((i*(i-3)) %217));
   s[6] = (char)(1 + ((17*i+129)%233 ));
   s[7] = '\0';
}
void sample_string_E(char *s, long i)
{  s[0] = (char)(1 + (i%254));
   s[1] = (char)(1 + ((i/254)%254));
   s[2] = (char)(1 + (((i/254)/254)%254));
   s[3] = 'a';
   s[4] = (char)(2 + ((i*(i-3)) %217));
   s[5] = (char)(1 + ((17*i+129)%233 ));
   s[6] = '\0';
}



int main()
{  long i,j; 
   bf_t * bloom;
   bloom = create_bf();
   printf("Created Filter\n");
   for( i= 0; i< 1450000; i++ )
   {  char s[8];
      sample_string_A(s,i);
      insert_bf( bloom, s );
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_B(s,i);
      insert_bf( bloom, s );
   }
   for( i= 0; i< 50000; i++ )
   {  char s[6];
      sample_string_C(s,i);
      insert_bf( bloom, s );
   }
   printf("inserted 2,000,000 strings of length 8,7,6.\n");
   
   for( i= 0; i< 1450000; i++ )
   {  char s[8];
      sample_string_A(s,i);
      if( is_element( bloom, s ) != 1 )
  {  printf("found negative error (1)\n"); exit(0); }
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_B(s,i);
      if( is_element( bloom, s ) != 1 )
  {  printf("found negative error (2)\n"); exit(0); }
   }
   for( i= 0; i< 50000; i++ )
   {  char s[6];
      sample_string_C(s,i);
      if( is_element( bloom, s ) != 1 )
  {  printf("found negative error (3)\n"); exit(0); }
   }
   j = 0;
   for( i= 0; i< 500000; i++ )
   {  char s[8];
      sample_string_D(s,i);
      if( is_element( bloom, s ) != 0 )
  j+=1;
   }
   for( i= 0; i< 500000; i++ )
   {  char s[7];
      sample_string_E(s,i);
      if( is_element( bloom, s ) != 0 )
  j+=1;
   }
   printf("Found %d positive errors out of 1,000,000 tests.\n",j);
   printf("Positive error rate %f\%.\n", (float)j/10000.0);

}