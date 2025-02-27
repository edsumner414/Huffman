/* Carl Tuck
 * ECE 4680
 * Lab 2
 * 6 Feb '25
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

void write_tree(tree_t *root, FILE *new_file);

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
	long int file_size;
	unsigned char *file_data;
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
	    //build tree function
		write_tree(root, new_file);
		//output data to file
	}
	
	// DECOMPRESSING
	else {
	    
	}

	//free tree
	
	return 0;
}

void write_tree(tree_t *root, FILE *new_file){


}
