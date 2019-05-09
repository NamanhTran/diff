#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

static int length1 = 0;
static int length2 = 0;

static OperationInfo op_info_arr[MAXLINES * 2];
static int op_arr_len = 0;

int main(int argc, char* argv[]) 
{
    char flag, file1[25], file2[25];
    char* lines1[MAXLINES]; 
    char* lines2[MAXLINES];

    // Check if user entered the correct amount of arguments
    if (argc < 3 || argc > 4) {
        printf("Please provide at least 2 files to use this program\n");
        return 1;
    }

    // Check if the user has passed option flags through the arguments
    if (argv[1][0] == '-') {
        if (strlen(argv[1]) != 2) {
            printf("%s is not a valid flag\n", argv[1]);
            return 2;
        }

        printf("Options %s enabled\n", argv[1]);
        flag = argv[1][0];
        // File1 = y
        strcpy(file1, argv[3]);

        // File2 = x
        strcpy(file2, argv[2]);
    }

    // If no flags
    else {
        strcpy(file2, argv[1]);
        strcpy(file1, argv[2]);
    }

    // Gets the length of the array and puts all the file lines into array
    length1 = fp_to_arr(lines1, file1);
    length2 = fp_to_arr(lines2, file2);

    // Fills the table needed for longest common sub-sequence algorithm
    int table[ROW][COL];
    fill_table(table, lines1, lines2);
    
    for (int i = 0; i <= length2; i++) {
        for (int j = 0; j <= length1; j++) {
            if (table[i][j] >= 10)
                printf(" %i", table[i][j]);
            else 
                printf("  %i", table[i][j]);
        }
        printf("\n");
    }

    diff_algo(table, lines1, lines2, length1, length2);

    DiffLinesInfo lines_info[MAXLINES * 2];

    int lines_info_length = format(lines_info);

    // Case for default case
    if (!flag) {
        default_print(lines_info, lines_info_length, lines1, lines2);
    }

    // Free all of the lines in the arrays
    free_str_arr(lines1);
    free_str_arr(lines2);
    return 0;
}

int fp_to_arr(char* arr[], char* name) 
{
    FILE *fp;
    char line[MAXLENGTH];
    int i = 0;

    fp = fopen(name, "r");
    
    if (fp == NULL) {
        printf("Cannot open %s\n", name);
        exit(3);
    }

    while (fgets(line, MAXLENGTH, fp) != NULL) {
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        arr[i] = malloc(sizeof(line) * 1);
        //printf("line: %s\n", line);
        strcpy(arr[i], line);
        i++;
    }

    fclose(fp);

    return i;
}

// Free all the lines we malloc
void free_str_arr(char* arr[]) 
{
    for (int i = 0; i < MAXLINES; i++) {
        if (arr[i] == NULL)
            return;

        free(arr[i]);
    }
}

void fill_table(int table[ROW][COL], char* arr1[], char* arr2[]) 
{
    // length1 = col  
    for (int i = 0; i <= length1; i++) {
        table[0][i] = 0;
    }

    // length2 = row
    for (int i = 0; i <= length2; i++) {
        table[i][0] = 0;
    }

    // Typical LCS Table fill-up code 
    for (int i = 1; i <= length2; i++) {
        for (int j = 1; j <= length1; j++) {
            if (strcmp(arr1[j - 1], arr2[i - 1]) == 0) {
                table[i][j] = table[i - 1][j - 1] + 1;
            }
            
            else
                table[i][j] = max(table[i - 1][j], table[i][j - 1]);
        }
    }
}

void diff_algo(int table[ROW][COL], char* arr1[], char* arr2[], int col, int row) 
{ 
    while(row > 0 || col > 0) {
        // If the lines match
        if (row > 0 && col > 0 && strcmp(arr1[col - 1], arr2[row - 1]) == 0) {
            col--;
            row--;
        }

        else if (col > 0 && row == 0) {
            add_to_op('a', row, col, arr1[col - 1]);
            //printf("%i a %i: '%s'\n", row, col, arr1[col - 1]);
            col--;
        }

        // Case for add (If col - 1 > row - 1)
        else if (col > 0 && table[row][col - 1] > table[row - 1][col]) {
            add_to_op('a', row, col, arr1[col - 1]);
            //printf("%i a %i: '%s'\n", row, col, arr1[col - 1]);
            col--;
        }

        else if (col > 0 && row == 0) {
            add_to_op('c', row, col, arr1[col - 1]);
            //printf("%i c %i: '%s' -> '%s'\n", row, col, arr2[row - 1], arr1[col - 1]);
            col--;
            row--;
        }
        
        // Case for change (If col - 1 and row - 1 are equal and are not present in both arr1 and arr2)
        else if (col > 0 && table[row][col - 1] == table[row - 1][col]) {
            add_to_op('c', row, col, arr1[col - 1]);
            //printf("%i c %i: '%s' -> '%s'\n", row, col, arr2[row - 1], arr1[col - 1]);
            col--;
            row--;
        }

        else if (row > 0 && col == 0) {
            add_to_op('d', row, col + 1, arr2[row - 1]);
            //printf("%i d %i: '%s'\n", row, col + 1, arr2[row - 1]);
            row--;
        }

        // Case for delete (if row - 1 < col - 1)
        else if (row > 0 && table[row][col - 1] < table[row - 1][col]) {
            add_to_op('d', row, col + 1, arr2[row - 1]);
            //printf("%i d %i: '%s'\n", row, col + 1, arr2[row - 1]);
            row--;
        }

        //printf("Row: %i, Col: %i\n", row, col);
    }

    // Reverses the array to make the formatting easier
    OperationInfo tempArr[MAXLINES * 2];

    for (int i = 0; i < op_arr_len; i++) {
        tempArr[i] = op_info_arr[i];
    }

    int arr_counter = 0;

    for (int i = op_arr_len - 1; i >= 0; i--) {
        op_info_arr[arr_counter++] = tempArr[i];
    }
}

void add_to_op(int op, int left, int right, char* line) 
{
    OperationInfo temp;
    strcpy(temp.line, line);
    temp.line_num_left = left;
    temp.line_num_right = right;
    temp.op = op;
    op_info_arr[op_arr_len++] = temp;
}

DiffLinesInfo construct_DiffLines(int op, int ls, int le, int rs, int re) {
    DiffLinesInfo temp;
    temp.op = op;
    temp.left_start = ls;
    temp.left_end = le;
    temp.right_start = rs;
    temp.right_end = re;
    //strcpy(temp.line, line);
    return temp;
}

int format(DiffLinesInfo lines_arr[]) 
{
    int last_op = 0;
    int left_start = 0;
    int left_end = 0;
    int right_start = 0;
    int right_end = 0;
    int arr_length = 0;

    for (int i = 0; i < op_arr_len; i++) {
        OperationInfo temp = op_info_arr[i];

        // Case for new op
        if (last_op == 0 || last_op != temp.op || (left_end + 1 != temp.line_num_left && temp.op != 'a') || (right_end + 1 != temp.line_num_right && temp.op != 'd')) {

            // Case for any iteratation except the first
            if (last_op != 0) {
                //printf("left range: %i - %i op: %c right range %i - %i\n", left_start, left_end, last_op, right_start, right_end);
                if (left_end - left_start > 0 ) {
                    //printf("%i,%i%c", left_start, left_end, last_op);
                }

                else {
                    //printf("%i%c", left_start, last_op);
                    left_end = -1;
                }

                if (right_end - right_start > 0) {
                    //printf("%i,%i\n\n", right_start, right_end);
                    lines_arr[arr_length++] = construct_DiffLines(last_op, left_start, left_end, right_start, right_end);
                }

                else {
                    //printf("%i\n\n", right_start);
                    right_end = -1;
                    lines_arr[arr_length++] = construct_DiffLines(last_op, left_start, left_end, right_start, right_end);
                }
            }

            //printf("New op\n");
            left_start = temp.line_num_left;
            right_start = temp.line_num_right;
            left_end = temp.line_num_left;
            right_end = temp.line_num_right;
        }
        
        // Case for op continuing
        else {
            left_end = temp.line_num_left;
            right_end = temp.line_num_right;
        }

        last_op = temp.op;

        //printf("%i %c %i '%s'\n", temp.line_num_left, temp.op, temp.line_num_right, temp.line);

        // Print the last interation of the loop
        if (i == op_arr_len - 1) {
                //printf("left range: %i - %i op: %c right range %i - %i\n\n", left_start, left_end, last_op, right_start, right_end);
                if (left_end - left_start > 0) {
                    //printf("%i,%i%c", left_start, left_end, last_op);
                }

                else {
                    //printf("%i%c", left_start, last_op);
                    left_end = -1;
                }

                if (right_end - right_start > 0) {
                    //printf("%i,%i\n", right_start, right_end);
                    lines_arr[arr_length++] = construct_DiffLines(last_op, left_start, left_end, right_start, right_end);
                }

                else {
                    //printf("%i\n", right_start);
                    right_end = -1;
                    lines_arr[arr_length++] = construct_DiffLines(last_op, left_start, left_end, right_start, right_end);
                }
        }
    }

    return arr_length;
}

void default_print(DiffLinesInfo info_arr[], int info_arr_length, char* arr1[], char* arr2[]) {

    for (int i = 0; i < info_arr_length; i++) {
        DiffLinesInfo temp = info_arr[i];
        int single_left = 0;
        int single_right = 0;
        // Prints the lines
        if (temp.left_end != -1) {
            printf("%i,%i%c", temp.left_start, temp.left_end, temp.op);
        }

        else {
            printf("%i%c", temp.left_start, temp.op);
        }

        if (temp.right_end != -1) {
            printf("%i,%i\n", temp.right_start, temp.right_end);
        }

        else {
            printf("%i\n", temp.right_start);
        }

        // Prints out the lines from the files
        switch ((char)temp.op) {
            case 'a':   
                        for (int i = temp.right_start; i <= temp.right_end; i++) {
                            printf("> %s\n", arr1[i - 1]);
                        }
                        break;
            case 'd':   
                        for (int i = temp.left_start; i <= temp.left_end; i++) {
                            printf("< %s\n", arr2[i - 1]);
                        }
                        break;
            case 'c':
                        for (int i = temp.left_start; i <= temp.left_end; i++) {
                            printf("< %s\n", arr2[i - 1]);
                        }

                        printf("---\n");

                        for (int i = temp.right_start; i <= temp.right_end; i++) {
                            printf("> %s\n", arr1[i - 1]);
                        }
                        break;
        }
    }
}
