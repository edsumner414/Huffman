/* tree.c
 *
 * Carl Tuck
 * Lab 4: Huffman Codec
 * ECE 4680, Spring 2025
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "tree.h"

#define TRUE 1
#define FALSE 0

/* bst_construct
 * Create the header block for the tree. Initialize the root
 * pointer to NULL. The tree_size stores the current number of 
 * keys in the tree. The num_recent_key_comparisons stores the
 * number of key comparisons during the most recent search,
 * insert, or remove.
 *
 * There is one comparison to determine if the key is found at 
 * the current level and if the key is not found one more comparison
 * to determine if the next step is to the left or right. Do not
 * count checks for NULL pointers.
 *
 * Input: Void.
 *
 * Output: Pointer to the tree_t structure constructed.
 */
tree_t *bst_construct (void)
{
    tree_t *Tree;
    Tree = (tree_t *) malloc(sizeof(tree_t));
    Tree->root = NULL;
    Tree->tree_size = 0;
    Tree->num_recent_key_comparisons = 0;
    return Tree;
}



/* bst_destruct_aux
 * Auxiliary function to destruct the BST recursively.
 * This function recursively accesses all nodes and then
 * frees their data pointers, and finally the Node structure itself.
 *
 * Input: Pointer to a tree_node_t structure.
 *
 * Output: Void.
 */
void bst_destruct_aux (tree_node_t *Node)
{
    if (Node != NULL)
    {
	bst_destruct_aux(Node->left);
	bst_destruct_aux(Node->right);
	free(Node->data_ptr);
	Node->data_ptr = NULL;
	Node->left = NULL;
	Node->right = NULL;
	free(Node);
    }
}



/* bst_destruct
 * Free all items stored in the tree including the memory block
 * with the data and the tree_node_t structure. Also frees the
 * header block.
 *
 * Input: Pointer to a tree_t structure.
 *
 * Output: Void.
 */
void bst_destruct (tree_t *Tree)
{
    bst_destruct_aux(Tree->root);
    free(Tree);
}



/* bst_insert
 * Insert the memory block pointed to by DataPointer into the
 * tree with the associated key. The function must return 0 if
 * the key is already in the tree (in which case the data memory
 * block is replaced). The function must return 1 if the key was
 * not already in the tree but was instead added to the tree.
 *
 * Inputs: Pointer to a tree_t structure. 
 *	   Key associated with the memory block.
 *	   Pointer to the memory block.
 * 
 * Outputs: 0 if the key is already in the tree.
 *	    1 if the key was added to the tree.
 */
int bst_insert (tree_t *Tree, tree_key_t Key, mydata_t *DataPointer)
{
    Tree->num_recent_key_comparisons = 0;
    
    tree_node_t *Node = (tree_node_t *) malloc(sizeof(tree_node_t));
    Node->key = Key;
    Node->data_ptr = DataPointer;
    Node->left = NULL;
    Node->right = NULL;

    // No nodes in Tree.
    if (Tree->root == NULL)
    {
	Tree->root = Node;
	Tree->tree_size += 1;
	return 1;
    }
    tree_node_t *Rover = Tree->root;
    tree_node_t *Parent = NULL;

    while (Rover != NULL)
    {
	Tree->num_recent_key_comparisons += 1;
	Parent = Rover;

	if (Key < Rover->key)
	{
	    Rover = Rover->left;
	}
	else if (Key > Rover->key)
	{
	    Rover = Rover->right;
	}
	else
	{
	    free(Node);
	    free(Rover->data_ptr);
	    Rover->data_ptr = DataPointer;
	    return 0;
	}
    } // Rover reaches NULL.
    if (Key < Parent->key)
    {
	Parent->left = Node;
    }
    else
    {
	Parent->right = Node;
    }
    Tree->tree_size += 1;
    return 1;
}



/* bst_remove
 * Remove the item in the tree with the matching key. 
 * Return the pointer to the data memory block and free the
 * tree_node_t memory block. If the key is not found
 * in the tree, return NULL.
 *
 * Inputs: Pointer to a tree_t structure.
 *	   Key to search for.
 * 
 * Output: Pointer to the mydata_t memory block removed.
 */
mydata_t *bst_remove (tree_t *Tree, tree_key_t Key)
{
    Tree->num_recent_key_comparisons = 0;
    
    // No nodes in Tree.
    if (Tree->root == NULL)
    {
	return NULL;
    }
    tree_node_t *Rover = Tree->root;
    tree_node_t *Parent = NULL;
    mydata_t *Data = NULL;

    while (Rover != NULL)
    {
	Tree->num_recent_key_comparisons += 1;
	if (Key < Rover->key)
	{
	    Parent = Rover;
	    Rover = Rover->left;
	}
	else if (Key > Rover->key)
	{
	    Parent = Rover;
	    Rover = Rover->right;
	}
	else
	{
	    Data = Rover->data_ptr;
	    // No children.
	    if (Rover->left == NULL && Rover->right == NULL)
	    {
		// Removing a root with no children.
		if (Parent == NULL)
		{
		    Tree->root = NULL;
		}
		else if (Parent->left == Rover)
		{
		    Parent->left = NULL;
		}
		else
		{
		    Parent->right = NULL;
		}
		free(Rover);
		Tree->tree_size -= 1;
		return Data;
	    }
	    // 2 children.
	    else if (Rover->left != NULL && Rover->right != NULL)
	    {
		tree_node_t *Successor = Rover->left;
		tree_node_t *Successor_Parent = Rover;
		while (Successor->right != NULL)
		{
		    Successor_Parent = Successor;
		    Successor = Successor->right;
		}
		Rover->key = Successor->key;
		Rover->data_ptr = Successor->data_ptr;
		if (Successor_Parent->right == Successor)
		{
		    Successor_Parent->right = Successor->left;
		}
		else
		{
		    Successor_Parent->left = Successor->left;
		}
		free(Successor);
		Tree->tree_size -= 1;
		return Data;
	    }
	    // 1 child.
	    else
	    {
		tree_node_t *Child = NULL;
		if (Rover->left != NULL)
		{
		    Child = Rover->left;
		}
		else
		{
		    Child = Rover->right;
		}
		// Root with one child.
		if (Parent == NULL)
		{
		    free(Rover);
		    Tree->root = Child;
		}
		else if (Parent->left == Rover)
		{
		    Parent->left = Child;
		    free(Rover);
		}
		else
		{
		    Parent->right = Child;
		    free(Rover);
		}
		Tree->tree_size -= 1;
		return Data;
	    }
	}
    }
    return NULL;
}



/* bst_search
 * Find the tree element with the matching key and return a pointer
 * to the data block that is stored in this node in the tree.
 * If the key is not found in the tree then NULL is returned.
 *
 * Inputs: Pointer to a tree_t structure.
 *	   Key to search for.
 * 
 * Output: Pointer to a mydata_t data block. 
 */
mydata_t *bst_search (tree_t *Tree, tree_key_t Key)
{
    Tree->num_recent_key_comparisons = 0;
    tree_node_t *Rover = Tree->root;
    while (Rover != NULL)
    {
	Tree->num_recent_key_comparisons += 1;
	if (Key < Rover->key)
	{
	    Rover = Rover->left;
	}
	else if (Key > Rover->key)
	{
	    Rover = Rover->right;
	}
	else
	{
	    return Rover->data_ptr;
	}
    }
    return NULL;
}



/* bst_size
 * Returns the number of keys in the tree.
 *
 * Input: Pointer to a tree_t structure.
 * 
 * Output: Integer value of the number of keys in the tree.
 */
int bst_size (tree_t *Tree)
{
    return Tree->tree_size;
}



/* bst_stats
 * Returns num_recent_key_comparisons, the number of key comparisons for
 * the most recent call to bst_insert, bst_remove, or bst_search.
 *
 * Input: Pointer to a tree_t structure.
 * 
 * Output: Integer value of the number of key comparisons.
 */
int bst_stats (tree_t *Tree)
{
    return Tree->num_recent_key_comparisons;
}



/* bst_internal_path_len_aux
 * Helper function to recursively traverse the tree keep a running sum
 * of the level to find the longest path length.
 *
 * Input: Pointer to a tree_node_t structure.
 *	  Level number.
 * 
 * Output: Level number.
 */
int bst_internal_path_len_aux (tree_node_t *Node, int Level)
{
    if (Node == NULL)
    {
	return 0;
    }
    else 
    {
	return Level + bst_internal_path_len_aux(Node->left, Level + 1)
	    + bst_internal_path_len_aux(Node->right, Level + 1);
    }
}



/* bst_internal_path_len
 * Returns the internal path length of the tree. 
 * 
 * Input: Pointer to a tree_t structure.
 * 
 * Output: Integer value of internal path length.
 */
int bst_internal_path_len (tree_t *Tree)
{
    return bst_internal_path_len_aux(Tree->root, 0);
}



/* ugly_print
 *
 * Input: Pointer to a tree_node_t structure.
 *        Level number.
 *
 * Output: Void.
 */
void ugly_print (tree_node_t *Node, int level)
{
    if (Node == NULL) return;
    ugly_print(Node->right, level+1);
    for (int i=0; i<level; i++) printf("     "); // 5 spaces
    printf("%5d", Node->key);                  // field width is 5
    if (Node->data_ptr != NULL) printf("%s\n", Node->data_ptr);
    ugly_print(Node->left, level+1);
}



/* bst_debug_print_tree
 * Used to display the tree.
 *
 * Input: Pointer to a tree_t structure.
 * 
 * Output: Void.
 */
void bst_debug_print_tree (tree_t *Tree)
{
    //printf("Tree with %d keys\n", Tree->tree_size);
    ugly_print(Tree->root, 0);
    printf("\n");
    //bst_debug_validate(Tree);
}



/* bst_debug_validate_rec
 * 
 * Input: Pointer to a tree_node_t structure.
 *	  Minimum value.
 *	  Maximum value.
 * 
 * Output: TRUE if valid.
 *	   FALSE if not valid.
 */
int bst_debug_validate_rec (tree_node_t *Node, int min, int max, int *count)
{
    if (Node == NULL) return TRUE;
    if (Node->key <= min || Node->key >= max) return FALSE;
    assert(Node->data_ptr != NULL);
    *count += 1;
    return bst_debug_validate_rec(Node->left, min, Node->key, count) &&
	bst_debug_validate_rec(Node->right, Node->key, max, count);
}



/* bst_debug_validate
 * Used to make sure the code is using the proper structure definitions.
 *
 * Input: Pointer to a tree_t structure.
 * 
 * Output: Void.
 */
void bst_debug_validate (tree_t *Tree)
{
    int size = 0;
    assert(bst_debug_validate_rec(Tree->root, INT_MIN, INT_MAX, &size) == TRUE);
    assert(size == Tree->tree_size);
}

