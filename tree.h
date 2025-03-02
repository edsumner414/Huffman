/* tree.h 
 * 
 * Carl Tuck & Ethan Sumner
 * Lab 4: Huffman Codec
 * ECE 4680, Spring 2025
 */



// Keys are integers.
typedef int tree_key_t;

// Data are characters.
typedef unsigned char mydata_t;

/* tree_node_t 
 * Nodes of the BST.
 *
 * key: Key value for the node.
 *
 * data_ptr: Data value for the node.
 *
 * left: Pointer to the left branch.
 *
 * right: Pointer to the right branch.
 */
typedef struct NodeTag
{
    tree_key_t key;
    mydata_t *data_ptr;
    struct NodeTag *left;
    struct NodeTag *right;
} tree_node_t;

/* tree_t
 * Header for the BST.
 * 
 * root: Pointer to the root of the BST.
 * 
 * tree_size: number of keys in the tree.
 * 
 * num_recent_key_comparisons: number of key comparisons during
 *		      the most recent search, insert, or remove.
 */
typedef struct TreeTag
{
    tree_node_t *root;
    int tree_size;
    int num_recent_key_comparisons;
} tree_t;



// FUNCTION PROTOTYPES
tree_t *bst_construct (void);
void bst_destruct (tree_t *);
int bst_insert (tree_t *, tree_key_t Key, mydata_t *DataPointer);
mydata_t *bst_remove (tree_t *, tree_key_t Key);
mydata_t *bst_search (tree_t *, tree_key_t Key);
int bst_size (tree_t *);
int bst_stats (tree_t *);
int bst_internal_path_len (tree_t *);
void bst_debug_print_tree (tree_t *);
void bst_debug_validate (tree_t *);

/* vi:set ts=8 sts=4 sw=4 et: */

