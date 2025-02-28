/* Carl Tuck & Ethan Sumner
 * ECE 4680
 * Lab 4
 * 4 March '25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree{
	int freq;
	unsigned char *data;
	tree_t *left;
	tree_t *right;
} tree_t;

long int write_tree(tree_t *root, unsigned char *file_data, unsigned char *final_byte);
unsigned char write_bit(unsigned char bit, unsigned char *file_data, long int size);

// FUNCTIONS FOR READING AND WRITING

/* fread(ptr, size, nmemb, stream)
 * ptr: pointer to block to be read
 * size: size of element to read in bytes
 * nmemb: number of elements
 * stream: pointer to input file stream
 */

/* fwrite(ptr, size, nmemb, stream)
 * ptr: pointer to block to be written
 * size: size of element to write in bytes
 * nmemb: number of elements
 * stream: pointer to output file stream
 */




int main(int argc, char *argv[]){
	FILE *og_file, *new_file;
	long int file_size, tree_size;
	unsigned char *file_data, *new_data, final_byte;
	tree_t *root;

	
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

	new_file = fopen(argv[2], "wb");
	if(new_file == NULL){
		printf("\n\nFailed to open output file\n");
		exit(0);
	}

	file_data = (unsigned char *)malloc(file_size); 
	fread(file_data, 1, file_size, og_file);

	// COMPRESSING
	if (strcmp(argv[3], "c") == 0) {
	    //build tree function here

		tree_size = write_tree(root, new_data, &final_byte);
		fwrite(tree_size, sizeof(long int), 1, new_file);
		fwrite(new_data, 1, tree_size/8, new_file);
		if(tree_size % 8 != 0) fwrite(&final_byte, 1, 1, new_file);

		//output data to file here
	}
	
	// DECOMPRESSING
	else {
	    
	}

	//free tree
	
	return 0;
}

/*
tree_t *root: root of the huffman tree
unsigned char *file_data: block to write full bytes to
unsigned char *final_byte: contains last few bits if (file_size % 8 != 0)

return: size of the tree in bits
*/
long int write_tree(tree_t *root, unsigned char *file_data, unsigned char *final_byte){
	static long int file_size;
	static int count;

	if(root == NULL) return 0;

	file_size++;
	if(root->data != NULL){
		//writes 1 for leaf node
		*final_byte = write_bit(0x01, file_data, file_size);
		for(int i = 7; i >= 0; i--){
			//writes each individual bit of the char on the leaf
			file_size++;
			*final_byte = write_bit((root->data[0] >> i) & 0x01, file_data, file_size);
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
