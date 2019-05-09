#define MAXLINES 1000
#define MAXLENGTH 254
#define ROW 1000
#define COL 1000
#define max(a, b) (a > b ? a : b)

typedef struct OperationInfo {
    // op = 0 is nothing op = 1 is add op = 2 is change op = 3 is delete
    int op;
    int line_num_left;
    int line_num_right;
    char line[MAXLENGTH];
} OperationInfo;

typedef struct DiffLinesInfo {
    int op;
    int left_start;
    int left_end;
    int right_start;
    int right_end;
    char line[MAXLENGTH];
} DiffLinesInfo;

DiffLinesInfo construct_DiffLines(int op, int ls, int le, int rs, int re);
void add_to_op(int op, int left, int right, char* line);
int fp_to_arr(char* arr[], char* name);
void free_str_arr(char* arr[]);
void fill_table(int table[ROW][COL], char* arr1[], char* arr2[]);
void diff_algo(int table[ROW][COL], char* arr1[], char* arr2[], int col, int row);
int format(DiffLinesInfo lines_arr[]);
void default_print(DiffLinesInfo info_arr[], int arr_length, char* arr1[], char* arr2[]);