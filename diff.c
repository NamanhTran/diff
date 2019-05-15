#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

static int length1 = 0;
static int length2 = 0;
static int version_flag = 0;                    // diff -v aka --version
static int brief_flag = 0;                      // diff -q aka --brief 
static int ignore_case_flag = 0;                // diff -i aka --ignore-case  
static int identical_flag = 0;                  // diff -s aka --report-identical-files
static int side_flag = 0;                       // diff -y aka --side-by-side
static int left_flag = 0;                       // diff --left-column
static int suppress_flag = 0;                   // diff --suppress-common-lines
static int context_flag = 0;                    // diff -c NUM aka --context=NUM
static int unified_flag = 0;                    // diff -u NUM aka --unified=NUM
static int num_val = 3;                         // NUM variable for context and unified flag
static OperationInfo op_info_arr[MAXLINES * 2]; // Array holding all operation informations
static int op_arr_len = 0;                      // Length of array for op_info_arr

int main(int argc, char* argv[]) 
{
    char first_priority = 0;     // First flag that has priority
    char file1[25], file2[25];   // File Buffers
    char* lines1[MAXLINES];      // String arrays holding strings pertaining to their files
    char* lines2[MAXLINES];

    // Check if user entered the correct amount of arguments
    if (argc == 1) {
        printf("Usage: ./diff [FLAGS] [FILE1] [FILE2]\n");
        return 1;
    }

    for (int i = 1; i < argc - 2; i++) {
        char flag[MAXLENGTH]; 
        strcpy(flag , argv[i]);

        if (strcmp(flag, "-v") == 0 || strcmp(flag, "--version") == 0) { 
            version_flag = 1;
            if (first_priority == 0) 
                first_priority = 'v';
        }

        else if (strcmp(flag, "-q") == 0 || strcmp(flag, "--brief") == 0)
            brief_flag = 1;

        else if (strcmp(flag, "-i") == 0 || strcmp(flag, "--ignore-case") == 0)
            ignore_case_flag = 1;

        else if (strcmp(flag, "-s") == 0 || strcmp(flag, "--report-identical-files") == 0) 
            identical_flag = 1;
        
        else if (strcmp(flag, "-y") == 0 || strcmp(flag, "--side-by-side") == 0) {
            side_flag = 1;
            if (first_priority == 0) 
                first_priority = 'y';
        }

        else if (strcmp(flag, "--left-column") == 0)
            left_flag = 1;

        else if (strcmp(flag, "--suppress-common-lines") == 0) 
             suppress_flag = 1;

        else if (strcmp(flag, "-c") == 0) {
            context_flag = 1;
            i++;
            for (int j = 0; j < (int)strlen(argv[i]); j++) {
                char temp = argv[i][j];
                if (!isdigit(temp)) {
                    i--;
                    break;
                }

                if (j == (int)strlen(argv[i]) - 1) 
                    num_val = atoi(argv[i]);
            }

            if (first_priority == 0) 
                first_priority = 'c';
        }

        else if (strstr(flag, "--context") != NULL) {
            context_flag = 1;
            char* token = strtok(flag, "=");
            token = strtok(NULL, "=");
            if (token != NULL) {
                for (int j = 0; j < (int)strlen(token); j++) {
                    char temp = token[j];
                    if (!isdigit(temp))
                        break;

                    if (j == (int)strlen(token) - 1) 
                        num_val = atoi(token);
                }
            }

            if (first_priority == 0) 
                first_priority = 'c';
        }

        else if (strcmp(flag, "-u") == 0) {
            unified_flag = 1;
            i++;
            for (int j = 0; j < (int)strlen(argv[i]); j++) {
                char temp = argv[i][j];
                if (!isdigit(temp)) {
                    i--;
                    break;
                }

                if (j == (int)strlen(argv[i]) - 1) 
                    num_val = atoi(argv[i]);
            }

            if (first_priority == 0) 
                first_priority = 'u';
        }

        else if (strstr(flag, "--unified")) {
            unified_flag = 1;
            char* token = strtok(flag, "=");
            token = strtok(NULL, "=");
            if (token != NULL) {
                for (int j = 0; j < (int)strlen(token); j++) {
                    char temp = token[j];
                    if (!isdigit(temp))
                        break;

                    if (j == (int)strlen(token) - 1) 
                        num_val = atoi(token);
                }
            }

            if (first_priority == 0) 
                first_priority = 'u';
        }

        else {
            printf("%s is a unsupported flag please enter valid flags only\n", flag);
            return 1;
        }
                
    }

    if (first_priority == 0) 
                first_priority = 'd';

    //printf("first priority: %c\nversion: %i\nbrief: %i\nignore-case: %i\nidentical: %i\nside-by-side: %i\nleft-col: %i\nsuppress: %i\ncontext: %i\nunified: %i\nnum: %i\n",first_priority, version_flag, brief_flag, ignore_case_flag, identical_flag, side_flag, left_flag, suppress_flag, context_flag, unified_flag, num_val);

    if (version_flag) {
        printf("diff version 0.0.0.1\nCreated by Namanh Tran\n");
        return 0;
    }

    if (argc < 3) {
        printf("Usage: ./diff [FLAGS] [FILE1] [FILE2]\n");
        return 1;
    }

    strcpy(file2, argv[argc - 2]);
    strcpy(file1, argv[argc - 1]);

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

    switch(first_priority) {
        case('y'):
        printf("line1[0]: %s\n", lines1[0]);
            side_print(lines_info, lines_info_length, lines2, lines1);
            break;

        case('c'):
            break;

        case('u'):
            break;
        
        case('d'):
            // Messed up put the wrong files to the wrong argv
            default_print(lines_info, lines_info_length, lines1, lines2);
            break;
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
    // Sources: https://www.youtube.com/watch?v=NnD96abizww (Psudocode at the end of the video)
    //          Introduction to Algorithms, Thrid Edition (Section 15.4)
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

// Heavily modified (by me) PRINT-LCS algorithm from Introduction to Algorithm, Thrid Edition (Section 15.4)
void diff_algo(int table[ROW][COL], char* arr1[], char* arr2[], int col, int row) 
{ 
    int is_diff = 0;
    int is_same = 1;

    while(row > 0 || col > 0) {
        // Case if the lines match, we don't care because we are only worried about everything but matched lines
        if (row > 0 && col > 0 && strcmp(arr1[col - 1], arr2[row - 1]) == 0) {
            col--;
            row--;
            
        }

        // Case for add if we hit the top of the matrix (row == 0 means end of arr1) and there are still lines left in arr2
        else if (col > 0 && row == 0) {
            add_to_op('a', row, col, arr1[col - 1]);
            //printf("%i a %i: '%s'\n", row, col, arr1[col - 1]);
            col--;
            is_diff = 1;
            is_same = 0;
        }

        // Case for add (If col - 1 > row - 1) meaning that there is a line missing in arr1
        else if (col > 0 && table[row][col - 1] > table[row - 1][col]) {
            add_to_op('a', row, col, arr1[col - 1]);
            //printf("%i a %i: '%s'\n", row, col, arr1[col - 1]);
            col--;
            is_diff = 1;
            is_same = 0;
        }
        
        // Case for change (If col - 1 and row - 1 are equal and there are lines but not the same lines)
        else if (col > 0 && table[row][col - 1] == table[row - 1][col]) {
            add_to_op('c', row, col, arr1[col - 1]);
            //printf("%i c %i: '%s' -> '%s'\n", row, col, arr2[row - 1], arr1[col - 1]);
            col--;
            row--;
            is_diff = 1;
            is_same = 0;
        }

        // Case for delete if we hit the top of the matrix (end of arr2) but there are still lines in arr1
        else if (row > 0 && col == 0) {
            add_to_op('d', row, col + 1, arr2[row - 1]);
            //printf("%i d %i: '%s'\n", row, col + 1, arr2[row - 1]);
            row--;
            is_diff = 1;
            is_same = 0;
        }

        // Case for delete (if row - 1 < col - 1) means that the change in number indicates that there are lines in arr1 that are not in arr2
        else if (row > 0 && table[row][col - 1] < table[row - 1][col]) {
            add_to_op('d', row, col + 1, arr2[row - 1]);
            //printf("%i d %i: '%s'\n", row, col + 1, arr2[row - 1]);
            row--;
            is_diff = 1;
            is_same = 0;
        }

        //printf("Row: %i, Col: %i\n", row, col);
    }

    if (brief_flag) {
        if (is_diff)
            printf("The files are different\n");

        exit(0);
    }

    if (is_same && identical_flag) {
        printf("The files are the same\n");

        exit(0);
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

void default_print(DiffLinesInfo info_arr[], int info_arr_length, char* arr1[], char* arr2[]) 
{

    for (int i = 0; i < info_arr_length; i++) {
        DiffLinesInfo temp = info_arr[i];
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
                        for (int j = temp.right_start; j <= temp.right_end; j++) {
                            printf("> %s\n", arr1[j - 1]);
                        }
                        break;
            case 'd':   
                        for (int j = temp.left_start; j <= temp.left_end; j++) {
                            printf("< %s\n", arr2[j - 1]);
                        }
                        break;
            case 'c':
                        for (int j = temp.left_start; j <= temp.left_end; j++) {
                            printf("< %s\n", arr2[j - 1]);
                        }

                        printf("---\n");

                        for (int j = temp.right_start; j <= temp.right_end; j++) {
                            printf("> %s\n", arr1[j - 1]);
                        }
                        break;
        }
    }
}

void side_print(DiffLinesInfo info_arr[], int arr_length, char* arr1[], char* arr2[]) 
{
    int longest_len = max(length1, length2);
    int counter = 0;
    DiffLinesInfo temp = info_arr[counter];
    for (int i = 0; i < longest_len; i++) {
        
    }
}
