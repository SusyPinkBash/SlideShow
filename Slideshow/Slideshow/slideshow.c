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

// ########## HELPER FUNCTIONS ##########
/* Given a char * buffer it clears it overriding it with '\0's */
void clear_buffer(char * buffer, size_t len) {
    for (size_t i=0; i < len; ++i) {
        buffer[i] = '\0';
    }
}

/* Copies a char data inside another char data with the end char without an end character*/
void copy_char_from_buffer(char * from, char * to, char end) {
    int i=0;
    while (from[i] != end) {
        to[i] = from[i];
        ++i;
    }
    from[i] = '\0';
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
    copy_char_from_buffer(line, buffer, ' ');
    return atoi(buffer);
}



// ########## CREATE STRUCTS FUNCTIONS ##########
/* Constructor: returns a null pointer in case of failure/
 * Creates a Photo from a given char *
 */
struct photo * photo_new(char * line, int photoID) {
    printf("%s\n", line);
    struct photo * this = malloc(sizeof(struct photo));
    if (!this) {
        return NULL;
    }
    this->photoID = photoID;
    this->orientation = get_orientation(line);
    this->n_of_tags = get_n_tags(&line[2]);
    this->first_tag = NULL;
    this->next = NULL;
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
//    printf("%s", line);
    
    this->first_photo = photo_new(line, 0);
    
    return this;
}

