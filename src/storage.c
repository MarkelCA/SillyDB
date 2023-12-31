#include <c++/11/bits/fs_fwd.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include "./hashmap.h"

#define BUFFER_SIZE 4096

#ifndef DATABASE_PATH
    #define DATABASE_PATH "~/.silly.db"
#endif /* ifndef DATABASE_PATH */

/**
 * Creates the database file if it doesn't exist.
 */
void create_database() {
    printf("%s", DATABASE_PATH);
    // if (access(DATABASE_PATH, F_OK) != 0) {
    //     printf("dont exist, %s", DATABASE_PATH);
    //     FILE* fptr = fopen(DATABASE_PATH, "w");
    //     fclose(fptr);
    // }
}

/**
 * Gets a key from the database binary file.
 */
char* get(char* key) {
    assert(key != NULL);
    FILE *file = NULL;
    char line[BUFFER_SIZE];
    memset(&line,0,sizeof (line));
    char* result = NULL;

    file = fopen(DATABASE_PATH, "r"); // DATABASE_PATH defined in the CMakeLists.txt and injected at bulid time.
    if (file == NULL) {
        perror("Failed to open the file.");
        return result;
    }
    char* val_data = NULL;

    char* current_key_data  = NULL;
    char* current_val_data  = NULL;

    // We iterate through all the file
    while (!feof(file)) {
        u_int8_t current_key_length = 0;
        u_int16_t current_val_lenght = 0;

        // We read the key
        fread(&current_key_length, sizeof(u_int8_t), 1, file);
        current_key_data = (char*)malloc(sizeof(char)*current_key_length + 1); // +1 to append a null pointer
        fread(current_key_data, sizeof(char), current_key_length, file);
        current_key_data[current_key_length] = '\0'; // We add the null terminator to avoid storing junk bytes

        // We read the value
        fread(&current_val_lenght, sizeof(u_int16_t), 1, file);
        current_val_data = (char*)malloc(sizeof(char)*current_val_lenght + 1);
        fread(current_val_data, sizeof(char), current_val_lenght, file);
        current_val_data[current_val_lenght] = '\0'; // We add the null terminator to avoid storing junk bytes
        // If we find the key we store it along with the value
        if(strcmp(key, current_key_data) == 0) {
            val_data    = current_val_data;
        }
    }
    // We deallocate the temporal variables
    free(current_key_data);

    // We check that the current value it's not the last one to avoid deallocating
    // the returned result pointer.
    if(current_val_data != val_data) {
        free(current_val_data);
    }

    if (val_data != NULL) {
        result = (char*)malloc( sizeof(char) * strlen(val_data) );
        result = val_data;
    } else {
        result = NULL;
    }

    fclose(file);

    return result;
}

/**
 * Sets a key in the database binary file.
 */
void set(const char* key, const char* value) {
    FILE *file = fopen(DATABASE_PATH, "ab+");
    assert(key && value);
    
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    u_int8_t key_length   = strlen(key);    // Max key lenght in bytes = 1 (key data: ~255 B)
    u_int16_t val_lenght  = strlen(value); //  Max val lenght in bytes = 2 (val data: ~65 KB)

    // Write key length and data
    fwrite(&key_length, sizeof(u_int8_t), 1, file);
    fwrite(key, sizeof(char), key_length, file);

    // Write value length and data
    fwrite(&val_lenght, sizeof(u_int16_t), 1, file);
    fwrite(value, sizeof(char), val_lenght, file);

    fclose(file);
}

void del(const char* key) {
    printf("Deleting (Not implemented yet!) %c", 2);
}

char* list() {
    FILE *file = NULL;
    char line[BUFFER_SIZE];
    memset(&line,0,sizeof (line));
    char* result = NULL;

    file = fopen(DATABASE_PATH, "r");
    if (file == NULL) {
        perror("Failed to open the file.");
        return result;
    }
    char* val_data = NULL;

    char* current_key_data  = NULL;
    char* current_val_data  = NULL;

    HashMap* keys_map = hashmap_create(100);
    //memset(keys_map, 0, sizeof(HashMap));

    // We iterate through all the file
    while (!feof(file)) {
        u_int8_t current_key_length = 0;
        u_int16_t current_val_lenght = 0;

        // We read the key
        fread(&current_key_length, sizeof(u_int8_t), 1, file);
        current_key_data = (char*)malloc(sizeof(char)*current_key_length + 1); // +1 to append a null pointer
        fread(current_key_data, sizeof(char), current_key_length, file);
        current_key_data[current_key_length] = '\0'; // We add the null terminator to avoid storing junk bytes

        // We read the value
        fread(&current_val_lenght, sizeof(u_int16_t), 1, file);
        current_val_data = (char*)malloc(sizeof(char)*current_val_lenght + 1);
        fread(current_val_data, sizeof(char), current_val_lenght, file);
        current_val_data[current_val_lenght] = '\0'; // We add the null terminator to avoid storing junk bytes
        // If we find the key we store it along with the value


        if(hashmap_get(keys_map, current_key_data) == NULL && strlen(current_key_data) > 0) {
            hashmap_insert(keys_map, current_key_data, "true");

        }
    }
    // We deallocate the temporal variables
    free(current_key_data);

    char* all_keys = hashmap_get_keys(keys_map);
    hashmap_free(keys_map);

    // We check that the current value it's not the last one to avoid deallocating
    // the returned result pointer.
    free(current_val_data);

    if (val_data != NULL) {
        result = (char*)malloc( sizeof(char) * strlen(val_data) );
        result = val_data;
    } else {
        result = NULL;
    }

    fclose(file);

    return all_keys;
}
