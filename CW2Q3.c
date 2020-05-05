#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Default table size
#define TABLE_SIZE 20000

// struct for storing names
struct table {
    char* name;
};

struct table *array;


// ALTERNATIVE to strlen method
int string_length(const char *str) {
    int n = 0;
    while (str[n] != '\0') {
        n++;
    }
    return n;
}


// ALTERNATIVE to memmove
char* my_memmove(int start, char* string, int end) {
    if(string[0] == '\"') {
        string++;
    }
    if (string[string_length(string)-1] == '\"') {
        string[strlen(string)-1] = '\0';
    }

    return string;
}


// ALTERNATIVE to strcat method
char* concatenate_string(char *str1, char *str2)
{
    while(*str1)
        str1++;

    while(*str2)
    {
        *str1 = *str2;
        str2++;
        str1++;
    }
    *str1 = '\0';
    return str1;
}


// ALTERNATIVE to strcmp method
int string_compare(char *str1, char *str2) {
    while (*str1 && *str1 == *str2) {
        str1++;
        str2++;
    }

    if (*str1 == '\0' && *str2 == '\0') {
        return 0;
    }
    else {
        return *str1 - *str2;
    }
}


// ALTERNATIVE to strcpy method
char* copy_string(char *destination, char *source)
{
    char *start = destination;

    while(*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }

    *destination = '\0';
    return start;
}


// function to hash key for index
int hash_func(char* name) {
    int value = 0;
    for(int i = 0; i < string_length(name); i++) {
        value = value * 23 + name[i];
    }

    // resolves negative numbers
    // allowing index to always be positive
    // ALTERNATIVE to Abs method
    if (value < 0) {
        value *= -1;
    }

    // by finding modulus we ensure the index is within the table size
    value = value % TABLE_SIZE;

    return value;
}


// initialises hashtable by dedicating size and setting buckets to NULL
void init_table() {
    array = (struct table*) malloc(TABLE_SIZE * sizeof(struct table));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        array[i].name = NULL;
    }
}


// function to check if name is in chained string
int check_name_in_table(char* name) {
    int index = hash_func(name);
    int len = string_length(array[index].name);
    char *chained_string = malloc(len - 1);
    copy_string(chained_string, array[index].name);
    int n = 0;

    // loop goes through each character comparing to name
    for (int i = 0; i < len; i++) {
        if (chained_string[i] == name[n] ) {
            n++;
            if (n == string_length(name)) {
                return 1;
            }
        }
        else {
            n = 0;
        }
    }
    return 0;
}

// procedure to add names to hashtable
void add(char* name) {
    printf("\nAdding %s into bucket\n", name);
    int index = hash_func(name);

    // current contents is checked before adding name to hashtable
    if (array[index].name == NULL) {
        array[index].name = name;
        printf("%s ADDED into bucket %d\n", name, index);
    }
    else if (check_name_in_table(name)) {
        printf("%s is already in chain.\n", name);
    }
    // implementation of Chaining into hash table to combat collisions
    else {
        printf("Another item is already in bucket, using chaining at index: %d\n", index);
        char bucket[string_length(array[index].name) + string_length(name)];
        copy_string(bucket, array[index].name);
        concatenate_string(bucket, ", ");
        concatenate_string(bucket, name);
        copy_string(array[index].name, bucket);
        printf("Updated contents: %s\n", array[index].name);
    }
}


// procedure to find a specified name within the hashtable
void search(char* name) {
    printf("\n");
    int index = hash_func(name);
    if (array[index].name == NULL) {
        printf("%s does not exist in the hashtable\n", name);
        return;
    }
    else if (string_compare(name, array[index].name) == 0 || check_name_in_table(name)) {
        printf("%s exists in the hashtable\n", name);
        return;
    }
    printf("%s does not exist in hashtable\n", name);
}


// procedure to delete a name within a chain
void remove_chained_name(char* name, char* chained_string, int index, int i, int n) {
    // selection statements to find position of name in chained string
    if (i + 1 == n) {
        int m = 0;
        // plus 2 so comma and space are removed from string
        while (m < n) {
            chained_string++;
            m++;
        }
        // if statement checks if string is only name in string
        if (string_length(chained_string) < 2) {
            array[index].name = NULL;
        }
        else {
            copy_string(array[index].name, chained_string);
        }
    }
    // name is at end of string
    else if (i + 1 == string_length(chained_string)) {
        chained_string[i - n - 1] = '\0';
        copy_string(array[index].name, chained_string);
    }
    // name is surrounded within the string
    else {
        char *str2 = strtok(chained_string, ",");
        char *new_name = "";
        while (str2) {
            if (string_compare(str2, name) != 0) {
                if (string_length(new_name) == 0) {
                    new_name = str2;
                }
                else {
                    concatenate_string(new_name, ", ");
                    concatenate_string(new_name, str2);
                }
            }
            str2 = strtok(NULL, ", ");
        }
        copy_string(array[index].name, new_name);
    }
    printf("%s has been removed from the table.\n", name);
}


// procedure to delete name and check if name is in chain
void delete(char* name) {
    printf("\n");
    int index = hash_func(name);
    // bucket is empty
    if (array[index].name == NULL) {
        printf("%s does not exist in the hashtable.\n", name);
        return;
    }
    // name is in bucket with no chaining
    else if (string_compare(name, array[index].name) == 0) {
        array[index].name = NULL;
        printf("Deleted %s from table.\n", name);
    }
    // name is in chained string
    else {
        int len = string_length(array[index].name);
        char *name_string = malloc(len);
        copy_string(name_string, array[index].name);
        int n = 0;
        // finds index of name in string
        for (int i = 0; i < len; i++) {
            if (name_string[i] == name[n]) {
                n++;
                // index has been found
                if (n == string_length(name)) {
                    remove_chained_name(name, name_string, index, i, n);
                }
            }
            else {
                n = 0;
            }
        }
    }
}


// procedure to read name from file and add them to hashtable
void read_from_file() {
    char *filename = "./names.txt";
    FILE *fp = fopen(filename, "r");

    // checks if file exists
    if (!fp) {
        printf("\nCan't open file\n");
        return;
    }
    char c;
    char *name = "";
    // reads contents of file until end of file
    while (!feof(fp)) {
        c = fgetc(fp);
        char *str2 = NULL;

        // checks if character is punctuation as don't want this added to name
        if (c == '"' || c == ',' || c == ' ') {
            if (string_length(name) > 0) {
                add(name);
            }
            name = "";
            free(str2);
        }

        // add character to string
        else {
            int len = 0;
            if (string_length(name) > 0) {
                len = string_length(name);
            }
            str2 = malloc(len + 2);
            strcpy(str2, name);
            str2[len] = c;
            str2[len + 1] = '\0';
            name = str2;
        }
    }
    printf("\nAll names have been added into hashtable from file\n");
}


int main() {
    // initialise hashtable
    init_table();

    // read contents of file into array
    read_from_file();

    // start of showing subroutines
    printf("\n----------------------------------------------------");
    printf("\n           START OF SHOWING OFF SUBROUTINES");
    printf("\n----------------------------------------------------\n");

    // adding same name multiple times
    add("MARY");
    delete("MARY");
    add("MARY");

    // adding name with punctuation
    add("\"he>oo!ro,");

    // testing name that is hashtable
    search("LEWIS");

    // testing with punctuation in name
    search("\"he>oo!ro,");
    delete("\"he>oo!ro,");

    // proof name is in list
    printf("\n%s: contents of bucket at index.\n", array[hash_func("\"he>oo!ro,")].name);

    // trying to delete something not in a bucket
    // but the bucket has another name in
    delete("dafjha");

    // testing with chained string
    // SOILA, ALLYN, RUDOLF
    search("RUDOLF");
    delete("ALLYN");
    delete("RUDOLF");
    search("RUDOLF");
    delete("SOILA");

    // proof name is deleted
    delete("RUDOLF");
    delete("RUDOLF");

    // proof adding commas in causes no error when adding names
    add("RUDOLF, SOILA");
    search("RUDOLF, SOILA");
    printf("\n%s: contents of bucket at index.\n", array[hash_func("RUDOLF, SOILA")].name);
    delete("RUDOLF, SOILA");
    printf("\n%s: contents of bucket at index.\n", array[hash_func("RUDOLF, SOILA")].name);
    return 0;
}