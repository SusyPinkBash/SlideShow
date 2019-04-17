//
//  slideshow.c
//  Slideshow
//
//  Created by Susanna Ardigo'
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slideshow.h"


// ########## STRUCTS ##########

typedef enum {H, V} orientation;

struct photoset{
    int n;
    struct photo * first_photo;
};

struct photo {
    int photoID;
    orientation orientation;
    int n_of_tags;
    struct tag * first_tag;
    struct photo * next;
};

struct tag {
    struct photo * photo;
    char * hashtag;
    struct tag * next;
};

//typedef struct {
//    char * string;
//    int len;
//} tuple;

// ########## HELPER FUNCTIONS ##########
/* Given a char * buffer it clears it overriding it with '\0's */
void clear_buffer(char * buffer, size_t len) {
    for (size_t i=0; i < len; ++i) {
        buffer[i] = '\0';
    }
}

/* Copies a char data inside another char data with the end char without an end character*/
void copy_char_from_buffer(char * from, char * to, char end, int to_len) {
    int i=0;
    while (from[i] != end) {
        if (i >= to_len) {
            to_len += 10;
            to = realloc(to, to_len*sizeof(char));
        }
        to[i] = from[i];
        ++i;
    }
    to[i] = '\0';
}

/* Copies a char data inside another char data with the end char without an end character*/
void get_len_and_copy_char(char * from, char * to, char end, int to_len) {
    int i=0;
    while (from[i] != end) {
        if (i >= to_len) {
            to_len += 10;
            to = realloc(to, to_len*sizeof(char));
        }
        to[i] = from[i];
        ++i;
    }
    from[i] = '\0';
    to = realloc(to, i);
}

/* Copies a char data inside another char data with the end char without an end character*/
int get_len_copy_char(char * from, char * to, char end, int to_len) {
    int i=0;
    while (from[i] != end && from[i] != '\0') {
        if (i >= to_len) {
            to_len += 10;
            to = realloc(to, to_len*sizeof(char));
        }
        to[i] = from[i];
        ++i;
    }
    to[i] = '\0';
    to = realloc(to, i+1);
    return i;
}


/* Copies chars from a file to a char * buffer given an end char to stop at */
void file_to_buffer(FILE * file, char * buffer, char end, int buff_len) {
    int c;
    int i=0;
    while ((c=fgetc(file)) != end && c != EOF) {
        if (i >= buff_len) {
            buff_len += 100;
            buffer = realloc(buffer, buff_len*sizeof(char));
        }
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}

orientation get_orientation(char * line) {
    orientation or = (line[0] == 'H') ? H : V;
    return or;
}

int get_n_tags(char * line) {
    char * buffer = malloc(10*sizeof(char));
    copy_char_from_buffer(line, buffer, ' ', 10);
    int n = atoi(buffer);
    free(buffer);
    return n;
}

int get_index(char * line) {
    int i = 0;
    while(line[i] != ' ') {
        ++i;
    }
    return ++i;
}



// ########## CREATE STRUCTS FUNCTIONS ##########
/* Constructor: returns a null pointer in case of failure/
 * Creates a Tag from a given char *
 */
struct tag * tag_new(char * line) {
    struct tag * this = malloc(sizeof(struct tag));
    if (!this) {
        return NULL;
    }
    /* index to iteate the line char * */
    this->photo = NULL;
    
    char * name = malloc(10*sizeof(char));
    int offset = get_len_copy_char(line, name, ' ', 10);
    this->hashtag = name;
//    printf("%c\n", &line[offset]);
    if (line[offset] != '\0'){
        this->next = tag_new(&line[++offset]);
    }
    else {
        this->next = NULL;
    }
    return this;
}


/* Constructor: returns a null pointer in case of failure/
 * Creates a Photo from a given char *
 */
struct photo * photo_new(char * line, int photoID) {
//    printf("%s\n", line);
    struct photo * this = malloc(sizeof(struct photo));
    if (!this) {
        return NULL;
    }
    this->photoID = photoID;
    this->orientation = get_orientation(line);
    this->n_of_tags = get_n_tags(&line[2]);
    this->first_tag = NULL; // TODO: create tag
    this->first_tag = tag_new(&line[3+get_index(&line[3])]);
//    this->first_tag->photo = this; // TODO: add photo to all tag structs
    struct tag * current = this->first_tag;
    while (current->next != NULL) {
        current->photo = this;
        current = current->next;
    }
    
    this->next = NULL; // TODO: next photo
    return this;
}


/* Constructor: returns the null pointer in case of failure.
 * Reads the set of photo from the given file.
 */
struct photoset * ps_new(char * filename) {
    printf("%s\n", filename);
    FILE * file = fopen(filename, "r");
    struct photoset * this = malloc(sizeof(struct photoset));
    if (!file || !this) {
        // if this executes than there was an error;
        perror("Error trying to create the photoset");
        return NULL;
    }
    // read all the file and create everything
    char * buffer = malloc(10*sizeof(char));
    file_to_buffer(file, buffer, '\n', 10);
    this->n = atoi(buffer);
    free(buffer);
    this->first_photo = NULL; // TODO: create photos
    char * line = malloc(100*sizeof(char));
    file_to_buffer(file, line, '\n', 100);
    this->first_photo = photo_new(line, 0);

    return this;
}

