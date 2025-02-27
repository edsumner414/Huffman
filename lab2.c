/* Carl Tuck
 * ECE 4680
 * Lab 2
 * 6 Feb '25
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



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



/* compress
 *
 * This function takes an input file and compresses it using RLE.
 *
 * inputs: input_file, output_file (strings)
 * output: void
 */
void compress(const char *input_file, const char *output_file) {
    
    FILE *input_file_ptr = fopen(input_file, "rb");
    FILE *output_file_ptr = fopen(output_file, "wb");
    
    // Error opening either of the files.
    if (input_file_ptr == NULL) {
	printf("Error opening the input file.\n");
	fclose(input_file_ptr);
	exit(1);
    }
    
    if (output_file_ptr == NULL) {
	printf("Error opening the output file.\n");
	fclose(output_file_ptr);
	exit(1);
    }
    
    unsigned char current_byte, next_byte, number_of_byte;

    // Read one byte at a time into current_byte until the end of the file.
    while(fread(&current_byte, 1, 1, input_file_ptr)) {
	number_of_byte = 1;	

	// Read the next byte and if it matches, increment number_of_byte until no more duplicates.
	while(fread(&next_byte, 1, 1, input_file_ptr) && (next_byte == current_byte)) {
	    number_of_byte++;

	    /* Because the byte's number is an unsigned character, it cannot
	     * go over 255. When it reaches 255, it will be reset to 0.
	     */
	    if (number_of_byte == 255) {
		fwrite(&number_of_byte, 1, 1, output_file_ptr);
		fwrite(&current_byte, 1, 1, output_file_ptr);
		number_of_byte = 0; // reset back to 0 for next run
	    }
	} // When this condition is skipped, next_byte does not equal current_byte.

	if (number_of_byte > 0) {
	    fwrite(&number_of_byte, 1, 1, output_file_ptr);
	    fwrite(&current_byte, 1, 1, output_file_ptr);
	}
    
	// Moving the file pointer back one byte to properly read in more data at the right current_byte spot.
	if (next_byte != current_byte) {
	    fseek(input_file_ptr, -1, SEEK_CUR);
	}
    } // When this condition is skipped, the file pointer has reached the end of the file. 
    
    fclose(input_file_ptr);
    fclose(output_file_ptr);
}

/* decompress
 *
 * This function needs to do the opposite of compress.
 * Takes an input file and decompresses it from RLE format back to normal.
 * 
 * inputs: input_file, output_file (strings)
 * outputs: void
 */
void decompress(const char *input_file, const char *output_file) {

    // EZPZ 

    FILE *input_file_ptr = fopen(input_file, "rb");
    FILE *output_file_ptr = fopen(output_file, "wb");
    
    // Error opening either of the files.
    if (input_file_ptr == NULL) {
	printf("Error opening the input file.\n");
	fclose(input_file_ptr);
	exit(1);
    }
    
    if (output_file_ptr == NULL) {
	printf("Error opening the output file.\n");
	fclose(output_file_ptr);
	exit(1);
    }
    
    unsigned char byte;
    unsigned char number_of_byte; // Keeps track of repetitions of the byte.
    int i;

    /* Read the number of repetitions of the byte until the end of the file. 
     * Will always come in pairs (# : <byte>). 
     */
    while(fread(&number_of_byte, 1, 1, input_file_ptr)) /*This grabs the #.*/ {
	fread(&byte, 1, 1, input_file_ptr); 

	// Write the same byte # times.
	for (i = 0; i < number_of_byte; i++) {
	    fwrite(&byte, 1, 1, output_file_ptr);
	}
    } // When this condition is skipped, the file pointer has reached the end of the file. 
    
    fclose(input_file_ptr);
    fclose(output_file_ptr);
}

int main(int argc, char *argv[]){
	
	// Checking for 4 args and the c or d flag. 	
	if ((argc != 4) || ((strcmp("c", argv[3]) != 0) && (strcmp("d", argv[3]) != 0))) {
		printf("Incorrect arguments.\n");
		printf("Correct Usage: ./lab2 input_file_name output_file_name c\n");
		printf("               ./lab2 input_file_name output_file_name d\n");
		exit(1);
	}

	// COMPRESSING
	if (strcmp(argv[3], "c") == 0) {
	    compress(argv[1], argv[2]);
	}
	
	// DECOMPRESSING
	else {
	    decompress(argv[1], argv[2]);
	}
	
	return 0;
}
