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
    int intersection;
    int symm_diff0;
    int symm_diff1;
} tags_for_scoring;

typedef struct {
    char * string;
    int n;
} string_int_tuple;

typedef struct {
    int n;
    int len;
} ints_tuple;

// ########## HELPER FUNCTIONS ##########

/* Checks if the tags are equal */
int check_two_tags(char * first, char * second) {
    int i =0;
    while(first[i] != 0 && second[i] != 0) {
        if (first[i] != second[i])
            return 0;
        ++i;
    }
    return 1;
}

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

/* Checks if there is a duplicated photo in a slideshow */
int no_duplicates(struct slide * slideshow) {
    struct slide * this = slideshow;
    int this_ID = this->first->photoID;
    int second_ID = -1;
    int two_verticals = 0;
    if (this->second) {
        two_verticals = 1;
        second_ID = this->second->photoID;
        if (this_ID == second_ID) {
            return -1;
        }
    }
    
    struct slide * checking = this->next_slide;
    
    while(this && checking) {  // TODO: check condition
        if (checking->second) {
            if ((checking->second->photoID == this_ID) || (two_verticals && (checking->second->photoID == second_ID))) {
                return -1;
            }
        }
        if (checking->first) {
            if ((checking->first->photoID == this_ID) || (two_verticals && (checking->first->photoID == second_ID))) {
                return -1;
            }
        }
        if (checking->next_slide) {
            checking = checking->next_slide;
        }
        else {
            if (this->next_slide)
                this = this->next_slide;
            else break;
            if (this->next_slide)
                checking = this->next_slide;
            else break;
            
            this_ID = this->first->photoID;
            second_ID = -1;
            if (this->second) {
                two_verticals = 1;
                second_ID = this->second->photoID;
                if (this_ID == second_ID) {
                    return -1;
                }
            }
            else {
                two_verticals = 0;
                second_ID = -1;
            }
        }
        
        
    }
    return 0;
    
};


/* counts the number of equal tags in two V photos  */
int count_n_equal_tags(struct photo * first, struct photo * second) {
    
    struct tag * this = first->first_tag;
    struct tag * other = second->first_tag;
    
    int counter = 0;
    
    
    while (this->next_tag) {
        if (check_two_tags(this->hashtag, other->hashtag)) {
            ++counter;
            this = this->next_tag;
            other = second->first_tag;
        }
        else if (other->next_tag) {
            other = other->next_tag;
        }
        else {
            this = this->next_tag;
            other = second->first_tag;
        }
        
    }
    
    return counter;
    
}

struct tag * copy_tag(struct tag * original) {
    struct tag * copy = malloc(sizeof(struct tag));
    copy->photo = original->photo;
    copy->hashtag = original->hashtag;
    copy->next_tag = NULL;
    return copy;
}

/* Counts the number of tags in a linked list of tags */
int count_tags(struct tag * tag) {
    int counter = 0;
    while (tag) {
        ++counter;
        tag = tag->next_tag;
    }
    return counter;
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

int get_strlen(const char * slideshow) {
    int i = 0;
    while (slideshow[i] != 0) {
        ++i;
    }
    return i;
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

struct tag * get_no_dup_tags(struct photo * first, struct photo * second) {
    struct tag * this = first->first_tag;
    struct tag * other = second->first_tag;
    
    struct tag * starting = copy_tag(second->first_tag);
    other = other->next_tag;
    struct tag * new;
    struct tag * prev = starting;
    
    while (other) {
        new = copy_tag(other);
        prev->next_tag = new;
        prev = new;
        other = other->next_tag;
    }
    
    other = second->first_tag;
    
    while (this) {
        if (check_two_tags(this->hashtag, other->hashtag)) {
            this = this->next_tag;
            other = second->first_tag;
        }
        else if (!other->next_tag && !this->next_tag) {
            new = copy_tag(this);
            prev->next_tag = new;
            break;
        }
        else if (other->next_tag) {
            other = other->next_tag;
        }
        else {
            new = copy_tag(this);
            prev->next_tag = new;
            prev = new;
            this = this->next_tag;
            other = second->first_tag;
        }
        
    }
    
    return starting;
}

/* gets the min of two ints */
int get_min(int i, int j) {
    return (i <= j) ? i : j;
}

/* gets the minumum of the intersection and symbolyc differences */
int get_tag_min(tags_for_scoring data) {
    return get_min(data.intersection, (get_min(data.symm_diff0, data.symm_diff1)));
}




/* Given two slides computes the intersection and symmetric difference of the tags  */
tags_for_scoring get_intersection_slide(struct slide * first, struct slide * second) {
    struct tag * this, * other;
    int n_this, n_other;
    if (first->orientation == H) {
        this = first->first->first_tag;
        n_this = first->first->n_of_tags;
    }
    else {
        this = get_no_dup_tags(first->first, first->second);
        n_this = count_tags(this);
    }
    if (second->orientation == H) {
        other = second->first->first_tag;
        n_other = second->first->n_of_tags;
    }
    else {
        other = get_no_dup_tags(second->first, second->second);
        n_other = count_tags(other);
    }
    
    tags_for_scoring counters = {0, 0, 0};
    
    while (this->next_tag) {
        if (check_two_tags(this->hashtag, other->hashtag)) {
            ++counters.intersection;
            this = this->next_tag;
            other = second->first->first_tag;
        }
        else if (other->next_tag) {
            other = other->next_tag;
        }
        else if (!this->next_tag && !other->next_tag) {
            ++counters.symm_diff0;
        }
        else {
            //            ++counters.symm_diff0;
            this = this->next_tag;
            other = second->first->first_tag;
        }
        
    }
    
    counters.symm_diff0 = n_this - counters.intersection;
    counters.symm_diff1 = n_other - counters.intersection;
    
    return counters;
    
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


// ########## DISTRUCT STRUCTS FUNCTIONS ##########

/* Destructor: clear all memory allocated for the given slideshow. */
void slide_delete(struct slide * this) {
    if (this) {
        if (this->next_slide) {
            slide_delete(this->next_slide);
        }
        this->first = NULL;
        this->second = NULL;
        free(this);
    }
}


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


/* Destructor: clear all memory allocated for the given tag. */
void tags_dup_delete(struct tag * this) {
    if (this) {
        if (this->next_tag) {
            tag_delete(this->next_tag);
            this->next_tag = NULL;
        }
        this->photo = NULL;
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

// ##### OTHER HELPING FUNCTIONS #####
struct slide * create_slideshow(struct photoset * photoset, const char * slideshow) {
    int vertical = 0;
    int index = 0;
    
    ints_tuple next_photoID = get_next_photoID(&slideshow[index]);
    index += next_photoID.len +1;
    struct slide * first = create_slide(photoset, next_photoID.n);
    
    if (first->first->orientation == V) {
        vertical = 1;
    }
    
    
    struct slide * s = first;
    struct slide * p = first;
    
    int input_len = get_strlen(slideshow);
    while(index < input_len) {
        next_photoID = get_next_photoID(&slideshow[index]);
        index += next_photoID.len +1;
        
        if (vertical == 1) {
            //            struct photo * other = get_photo(p, next_photoID.n);
            struct photo * other = get_photo(photoset, next_photoID.n);
            if (other->orientation != V) {
                slide_delete(first);
                return NULL;
            }
            vertical = 0;
            s->second = other;
        }
        else {
            s = create_slide(photoset, next_photoID.n);
            if (s->first->orientation == V) {
                vertical = 1;
            }
            p->next_slide = s;
            p = s;
        }
        
        
    }
    
    return first;
}


/* Calculates the score of the given slideshow.
 * The slideshow is a string representation of the
 * photo IDs concatenated with ','. The interest factor
 * between two adjacent slides is the minimum between two
 * numbers: number of common tags and number of
 * different tags. Return -1 if slideshow contains
 * at least a duplicated or a nonpaired vertical photo.
 */
int ps_score_default(struct photoset * p, const char * slideshow) {
    struct slide * slides = create_slideshow(p, slideshow);
    if (slides == NULL || (no_duplicates(slides) == -1)) {
        return -1;
    }
    int score = 0;
    
    struct slide * s_i = slides;
    struct slide * s_j = slides->next_slide;
    
    tags_for_scoring data;
    
    
    while (s_j) {
        data = get_intersection_slide(s_i, s_j);
        score += get_tag_min(data);
        s_i = s_j;
        s_j = s_j->next_slide;
    }
    
    slide_delete(slides);
    
    
    
    return score;
}

/* Calculates the score of the given slideshow.
 * The slideshow is a string representation of the
 * photo IDs concatenated with ','. The interest factor
 * between every two adjacent slides are calculated by
 * the given input function. It returns -1 if
 * slideshow contains at least a duplicated photo or a
 * nonpaired vertical photo.
 */
int ps_score(struct photoset * p, const char * slideshow, int (*interest_factor)(unsigned intersection, unsigned differences)) {
    struct slide * slides = create_slideshow(p, slideshow);
    if (slides == NULL || (no_duplicates(slides) == -1)) {
        printf("dude no!\n");
        return -1;
    }
    int score = 0;
    
    struct slide * s_i = slides;
    struct slide * s_j = slides->next_slide;
    
    tags_for_scoring data;
    
    
    while (s_j) {
        data = get_intersection_slide(s_i, s_j);
        score += interest_factor(data.intersection, data.symm_diff0+data.symm_diff1);
        s_i = s_j;
        s_j = s_j->next_slide;
    }
    
    slide_delete(slides);
    
    
    
    return score;
    
}

