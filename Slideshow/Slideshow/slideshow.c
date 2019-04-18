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
    struct photo * next_photo;
};

struct tag {
    struct photo * photo;
    char * hashtag;
    struct tag * next_tag;
};

struct slide {
    orientation orientation;
    struct photo * first;
    struct photo * second;
    struct slide * next_slide;
};

typedef struct {
    char * string;
    int n;
} string_int_tuple;

typedef struct {
    int n;
    int len;
} ints_tuple;

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



/* Copies chars from a file to a char * buffer given an end char to stop at */
char * file_to_buffer(FILE * file, char * buffer, char end, int buff_len) {
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
    return buffer;
}



// ##### GETTER FUNCTIONS #####

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
string_int_tuple get_len_copy_char(char * from, char * to, char end, int to_len) {
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
    string_int_tuple temp = {to, i};
    return  temp;
}

/* gives the orientation of a photo */
orientation get_orientation(char * line) {
    orientation or = (line[0] == 'H') ? H : V;
    return or;
}

/* gives the number of tags */
int get_n_tags(char * line) {
    char * buffer = malloc(10*sizeof(char));
    copy_char_from_buffer(line, buffer, ' ', 10);
    int n = atoi(buffer);
    free(buffer);
    return n;
}

/* gets the index in a char pointer */
int get_index(char * line) {
    int i = 0;
    while(line[i] != ' ') {
        ++i;
    }
    return ++i;
}


/* gives the number id of the next photo */
ints_tuple get_next_photoID(const char * slideshow) {
    int buffer_len = 10;
    char * buffer = malloc(buffer_len*sizeof(char));
    int i=0;
    
    while (slideshow[i] != ',') {
        if (i >= buffer_len) {
            buffer_len += 10;
            buffer = realloc(buffer, buffer_len*sizeof(char));
        }
        buffer[i] = slideshow[i];
        ++i;
    }
    buffer[i] = '\0';
    int n = atoi(buffer);
    free(buffer);
    ints_tuple temp = {n, i};
    return temp;
}

struct photo * get_photo(struct photoset * p, int photoID) {
    if (photoID > p->n) {
        return NULL;
    }
    struct photo * current = p->first_photo;
    while(current->photoID != photoID) {
        current = current->next_photo;
    }
    return current;
}


// ##### SETTER FUNCTIONS #####


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
    string_int_tuple temp = get_len_copy_char(line, name, ' ', 10);
    int offset = temp.n;
    name = temp.string;
    this->hashtag = name;
//    printf("%c\n", &line[offset]);
    if (line[offset] != '\0'){
        this->next_tag = tag_new(&line[++offset]);
    }
    else {
        this->next_tag = NULL;
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
    this->first_tag = tag_new(&line[3+get_index(&line[3])]);
//    this->first_tag->photo = this; // TODO: add photo to all tag structs
    struct tag * current = this->first_tag;
    while (current->next_tag != NULL) {
        current->photo = this;
        current = current->next_tag;
    }
    
    this->next_photo = NULL; // TODO: next photo
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
    buffer = file_to_buffer(file, buffer, '\n', 10);
    this->n = atoi(buffer);
    free(buffer);
    char * line = malloc(100*sizeof(char));
    line = file_to_buffer(file, line, '\n', 100);
    this->first_photo = photo_new(line, 0);
    free (line);
    
    
    
    
    struct photo * prev = this->first_photo;
    struct photo * current;
    for (int i=1; i < this->n; ++i) {
        line = malloc(100*sizeof(char));
        line = file_to_buffer(file, line, '\n', 100);
//        printf("%s\n", line);
        current = photo_new(line, i);
        prev->next_photo = current;
        prev = current;
        free(line);
    }
    

    return this;
}

struct slide * create_slide(struct photoset * p, int photoid) {
    struct slide * this = malloc(sizeof(struct slide));
    if (!this) {
        return NULL;
    }
    this->next_slide = NULL;
    this->second = NULL;
    this->first = get_photo(p, photoid);
    this->orientation = this->first->orientation;

    return this;
}



// ##### OTHER HELPING FUNCTIONS #####
struct slide * create_slideshow(struct photoset * photoset, const char * slideshow) {
    // TODO create slideshow;
    int vertical = 0;
    int index = 0;
    
    struct slide * s = NULL;
    struct slide * prev = NULL;
    
    ints_tuple next_photoID = get_next_photoID(&slideshow[index]);
    index += next_photoID.len +1;
    prev = create_slide(photoset, next_photoID.n);
    
    
    while(slideshow[index] != 0) {
        next_photoID = get_next_photoID(&slideshow[index]);
        index += next_photoID.len +1;
        
        s = create_slide(photoset, next_photoID.n);
        if (s->orientation == V) {
            vertical = 1;
        }
    }
    
    return NULL;
}

// ########## DISTRUCT STRUCTS FUNCTIONS ##########

/* Destructor: clear all memory allocated for the given tag. */
void tag_delete(struct tag * this) {
    if (this) {
        if (this->next_tag) {
            tag_delete(this->next_tag);
            this->next_tag = NULL;
        }
        this->photo = NULL;
        free(this->hashtag);
        free(this);
    }
    
}

/* Destructor: clear all memory allocated for the given photo. */
void photo_delete(struct photo * this) {
    if (this) {
        if (this->first_tag) {
            tag_delete(this->first_tag);
            this->first_tag = NULL;
        }
        if (this->next_photo) {
            photo_delete(this->next_photo);
            this->next_photo = NULL;
        }
        free(this);
    }
    
}


/* Destructor: clear all memory allocated for the given photoset. */
void ps_delete(struct photoset * p) {
    if (p) {
        if (p->first_photo) {
            photo_delete(p->first_photo);
            p->first_photo = NULL;
        }
        free(p);
    }
}

// ########## SCORING FUNCTIONS ##########

/* Calculates the score of the given slideshow.
 * The slideshow is a string representation of the
 * photo IDs concatenated with ','. The interest factor
 * between two adjacent slides is the minimum between two
 * numbers: number of common tags and number of
 * different tags. Return -1 if slideshow contains
 * at least a duplicated or a nonpaired vertical photo.
 */
int ps_score_default(struct photoset * p, const char * slideshow) {
    struct slide * first_slide = create_slideshow(p, slideshow);
    return 0;
}
