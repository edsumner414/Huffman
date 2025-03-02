/* Carl Tuck & Ethan Sumner
 * ECE 4680
 * Lab 4
 * 4 March '25
 */



/* COMPILATION INSTRUCTIONS
 *
 * Compile lab.c with tree.c and tree.h files for the BST functions.
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"



// Structure for symbol frequency handling.
typedef struct {
    unsigned char symbol;
    int frequency;
} SymbolFrequency;



/* BINARY SEARCH TREE DEFINITIONS FROM TREE.H
 *
 * Nodes of the BST.
 * -----------------
 * key: Key value for the node.
 * data_ptr: Data value for the node.
 * left: Pointer to the left branch.
 * right: Pointer to the right branch.
 *
 * typedef struct NodeTag
 * {
 *	tree_key_t key;
 *	mydata_t *data_ptr;
 *	struct NodeTag *left;
 *	struct NodeTag *right;
 * } tree_node_t;
 * 
 * 
 *
 * Header for the BST.
 * -------------------
 * root: Pointer to the root of the BST.
 * tree_size: number of keys in the tree.
 * num_recent_key_comparisons: number of key comparisons during the most recent search, insert, or remove.
 *
 * typedef struct TreeTag
 * {
 *	tree_node_t *root;
 *	int tree_size;
 *	int num_recent_key_comparisons;
 * } tree_t;
 */



// FUNCTION PROTOTYPES
long int write_tree(tree_node_t *root, unsigned char *file_data, unsigned char *final_byte);
unsigned char write_bit(unsigned char bit, unsigned char *file_data, long int size);
char **tree_operations(tree_t *Tree, unsigned char *file_data, long int file_size);
void output_data_to_file(FILE *new_file, unsigned char *file_data, long int file_size, char **codes);


/* C FUNCTIONS FOR READING AND WRITING
 * ---------------------------------
 * fread(ptr, size, nmemb, stream)
 * ptr: pointer to block to be read
 * size: size of element to read in bytes
 * nmemb: number of elements
 * stream: pointer to input file stream
 *
 * fwrite(ptr, size, nmemb, stream)
 * ptr: pointer to block to be written
 * size: size of element to write in bytes
 * nmemb: number of elements
 * stream: pointer to output file stream
 */



int main(int argc, char *argv[]){
	
	FILE *og_file, *new_file;
	long int file_size, tree_size; 
	unsigned char *file_data, *new_data, final_byte;
	tree_t *Tree;

	
	// Checking for 4 args and the c or d flag. 	
	if ((argc != 4) || ((strcmp("c", argv[3]) != 0) && (strcmp("d", argv[3]) != 0))) {
		printf("Incorrect arguments.\n");
		printf("Correct Usage: ./lab2 input_file_name output_file_name c\n");
		printf("               ./lab2 input_file_name output_file_name d\n");
		exit(1);
	}

	og_file = fopen(argv[1], "rb");
	
	if(og_file == NULL){
		printf("\n\nFailed to open input file\n");
		exit(0);
	}

	//finds the file size
	fseek(og_file, 0, SEEK_END);
	file_size = ftell(og_file);
	rewind(og_file);

	new_file = fopen(argv[2], "rb+");

	if(new_file == NULL){
		printf("\n\nFailed to open output file\n");
		exit(0);
	}

	file_data = (unsigned char *)malloc(file_size); 
	fread(file_data, 1, file_size, og_file);

	// COMPRESSING
	if (strcmp(argv[3], "c") == 0) {
		
		Tree = bst_construct();
		char **codes = tree_operations(Tree, file_data, file_size);

		// Need to realloc new_data to fit the actual size.
		new_data = (unsigned char *) malloc(file_size);

		tree_size = write_tree(Tree->root, new_data, &final_byte);
		
		// Realloc once tree size if known.
		new_data = (unsigned char *) realloc(new_data, (tree_size/8) + 1);

		fwrite(&tree_size, sizeof(long int), 1, new_file);
		fwrite(new_data, 1, tree_size/8, new_file);
		if(tree_size % 8 != 0) fwrite(&final_byte, 1, 1, new_file);

		// Printing file data for debug.
		printf("\nFile Data:\n");
	
		for (long int i = 0; i < file_size; i++) {
		    unsigned char current_symbol = file_data[i];
		    printf("Symbol: %c (ASCII: %d), Code: %s\n", current_symbol, current_symbol, codes[current_symbol]);
		}

		//output data to file here
		output_data_to_file(new_file, file_data, file_size, codes);
		
		free(new_data);
		
		for (int i = 0; i < 256; i++) {
		    
		    if (codes[i] != NULL) {
			free(codes[i]);
		    }
		}
		
		free(codes);
	}
	
	// DECOMPRESSING
	else {
	    
	}

	// Destruct the tree.
	bst_destruct(Tree);

	return 0;
}



/* output_data_to_file
 */
void output_data_to_file(FILE *new_file, unsigned char *file_data, long int file_size, char **codes) {
   
    unsigned char buffer = 0; // buffer to hold 8-bits at a time
    int count = 0; // bits in buffer

    // Printing the contents of the file before writing.
    printf("File contents before writing:\n");
    fseek(new_file, 0, SEEK_SET);
    int c;
    
    while ((c = fgetc(new_file)) != EOF) {
	printf("%02x ", (unsigned char) c);
    }
    
    printf("\n");

    for (long int i = 0; i < file_size; i++) {
	
	// Grabbing one byte at a time from file_data.
	unsigned char current_symbol = file_data[i];
	
	// Indexing the codes using the symbol as its own index.
	char *code = codes[current_symbol];

	if (code != NULL) {

	    // Debug print.
	    printf("Processing symbol: %c (ASCII: %d), Code: %s\n", current_symbol, current_symbol, code);

	    for (int j = 0; code[j] != '\0'; j++) {
		buffer <<= 1;
	
		// Setting a 1.
		if (code[j] == '1') {
		    buffer |= 1;
		}

		count++;
		
		// Debug print.
		printf("Bit: %c, Buffer: 0x%02x, Count: %d\n", code[j], buffer, count);

		// Buffer is full and can be written to the output file.
		if (count == 8) {
		    printf("Writing byte: 0x%02x\n", buffer);
		    fwrite(&buffer, sizeof(unsigned char), 1, new_file);
		    buffer = 0;
		    count = 0;
		}
	    }
	}
    }

    // Left over bits, requiring a final byte. 
    if (count > 0) {
	buffer <<= (8 - count);

	// Debug print.
	printf("Writing final byte: 0x%02x\n", buffer);
	fwrite(&buffer, sizeof(unsigned char), 1, new_file);
    }

    // Printing the file after writing for debugging. 
    printf("File content after writing:\n");
    fseek(new_file, 0, SEEK_SET);
    
    while ((c = fgetc(new_file)) != EOF) {
	printf("%02x ", (unsigned char) c);
    }
    
    printf("\n");
}



/* swap
 *
 * Swaps two SymbolFrequency structures.
 *
 * input:   a, pointer to a SymbolFrequency struct
 *	    b, pointer to a SymbolFrequency struct
 */
void swap(SymbolFrequency *a, SymbolFrequency *b) {
    SymbolFrequency temp = *a;
    *a = *b;
    *b = temp;
}



/* bubble_sort
 *
 * Sorts SymbolFrequency struct by ascending frequency.
 *
 * input:   symbols, pointer to a SymbolFrequency struct
 *	    size, number of symbols to sort
 */
void bubble_sort(SymbolFrequency *symbols, int size) {
    
    for (int i = 0; i < size - 1; i++) {
	for (int j = 0; j < size - i - 1; j++) {
	    if (symbols[j].frequency > symbols[j + 1].frequency) {
		swap(&symbols[j], &symbols[j + 1]);
	    }
	}
    }
}



/* build_codes
 *
 * Recursive function to build the bit pattern as a string.
 *
 * input:   node, pointer to a tree node
 *	    code, string buffer to build the code
 *	    codes, array of strings to store all symbol codes
 */
void build_codes(tree_node_t *node, char *code, int depth, char **codes) {
    
    if (node == NULL) {
	return;
    }

    if (node->left == NULL && node->right == NULL) {
	code[depth] = '\0';
	codes[*(node->data_ptr)] = strdup(code);
	return;
    }

    // Going left.
    code[depth] = '0';
    build_codes(node->left, code, depth + 1, codes);

    // Going right.
    code[depth] = '1';
    build_codes(node->right, code, depth + 1, codes);
}



/* print_codes
 *
 * Prints Huffman codes. 
 *
 * input:   codes, array of code strings
 */
void print_codes(char **codes) {
    
    printf("Huffman codes:\n");
    for (int i = 0; i < 256; i++) {
	if (codes[i] != NULL) {
	    printf("Symbol: %c (ASCII: %d), Code: %s\n", (unsigned char) i, i, codes[i]);
	}
    }
    printf("----------");
}



/* tree_operations
 *
 * Handles the 9 step process for encoding.
 *
 * input:   Tree, pointer to a Tree BST.
 *	    file_data, pointer to bytes of the input file
 *	    file_size, size of the input file
 */
char **tree_operations(tree_t *Tree, unsigned char *file_data, long int file_size) {

    // 1. calculate frequencies
    SymbolFrequency symbols[256];
    int num_symbols = 0;

    for (long int i = 0; i < file_size; i++) {
	unsigned char current_symbol = file_data[i];

	int found = 0;

	for (int j = 0; j < num_symbols; j++) {
	    if (symbols[j].symbol == current_symbol) {
		symbols[j].frequency++;
		found = 1;
		break;
	    }
	}

	if (!found) {
	    symbols[num_symbols].symbol = current_symbol;
	    symbols[num_symbols].frequency = 1;
	    num_symbols++;
	}
    }
    
    // 2. sort symbols by ascending frequencies
    bubble_sort(symbols, num_symbols);

    // Debug printing.
    for (int i = 0; i < num_symbols; i++) {
	printf("Symbol: %c, Frequency: %d\n", symbols[i].symbol, symbols[i].frequency);
    }
    printf("----------");

    // 3. each symbol is a node (w/ count)
    tree_node_t **heap = (tree_node_t **) malloc(num_symbols * sizeof(tree_node_t *));
    for (int i = 0; i < num_symbols; i++) {
	heap[i] = (tree_node_t *) malloc(sizeof(tree_node_t));
	heap[i]->key = symbols[i].frequency;
	heap[i]->data_ptr = (mydata_t *) malloc(sizeof(mydata_t));
	*(heap[i]->data_ptr) = (unsigned char) symbols[i].symbol;
	heap[i]->left = NULL;
	heap[i]->right = NULL;
    }

    // 4. combine two smallest parent node, into new parent node w/ frequency sum
    while (num_symbols > 1) {
	
	// New parent node. 
	tree_node_t *parent = (tree_node_t *) malloc(sizeof(tree_node_t));
	parent->key = heap[0]->key + heap[1]->key;
	parent->data_ptr = NULL;
	parent->left = heap[0];
	parent->right = heap[1];

	heap[0] = parent;
	heap[1] = heap[num_symbols - 1];
	num_symbols--;
    
	// 5. resort parent nodes
	for (int i = 0; i < num_symbols - 1; i++) {
	    for (int j = i + 1; j < num_symbols; j++) {
		if (heap[i]->key > heap[j]->key) {
		    tree_node_t *temp = heap[i];
		    heap[i] = heap[j];
		    heap[j] = temp;
		}
	    }
	}
    
	// 6. goto step 4 until 1 parent node remaining
    }
   
    Tree->root = heap[0];
    free(heap);
    bst_debug_print_tree(Tree);

    // 7. assign 0-bit to left branches and 1-bit to right branches
    // 8. build varying bit patterns
    char **codes = (char **) calloc(256, sizeof(char *)); // Array of Huffman codes (strings).
    char code[256]; // Temporary buffer.
    build_codes(Tree->root, code, 0, codes);
    print_codes(codes);

    return codes;

}



/*
tree_t *root: root of the huffman tree
unsigned char *file_data: block to write full bytes to
unsigned char *final_byte: contains last few bits if (file_size % 8 != 0)

return: size of the tree in bits
*/
long int write_tree(tree_node_t *root, unsigned char *file_data, unsigned char *final_byte){
	static long int file_size;
	//static int count;

	if(root == NULL) return 0;

	file_size++;
	if(root->data_ptr != NULL){
		//writes 1 for leaf node
		*final_byte = write_bit(0x01, file_data, file_size);
		for(int i = 7; i >= 0; i--){
			//writes each individual bit of the char on the leaf
			file_size++;
			*final_byte = write_bit((root->data_ptr[0] >> i) & 0x01, file_data, file_size);
		}
	}
	else{
		//writes zero for non leaf
		*final_byte = write_bit(0x00, file_data, file_size);
	}

	write_tree(root->left, file_data, final_byte);
	write_tree(root->right, file_data, final_byte);

	return file_size;
}


/*
unsigned char bit: value of bit to be written (1 or 0)
unsigned char *file_data: block to write full bytes to
long int size: size of the file in bits so far

return: current state of the next byte to be written
*/
unsigned char write_bit(unsigned char bit, unsigned char *file_data, long int size){
	static unsigned char byte;

	//adds new bit to end of buffer
	byte = (byte << 1) | (bit & 0x01);
	if(size % 8 == 0){
		//writes byte to full upon full buffer
		file_data = realloc(file_data, size/8);
		file_data[size/8 - 1] = byte;
		byte = 0x00;
	}
	return byte;
}
