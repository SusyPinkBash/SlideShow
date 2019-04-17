#ifndef SLIDESHOW_H_INCLUDED
#define SLIDESHOW_H_INCLUDED

struct photoset;

/* Constructor: returns the null pointer in case of failure.
 * Reads the set of photo from the given file.
 */
struct photoset * ps_new(char * filename);

/* Destructor: clear all memory allocated for the given photoset. */
void ps_delete(struct photoset * p);

/* Calculates the score of the given slideshow.
 * The slideshow is a string representation of the
 * photo IDs concatenated with ','. The interest factor
 * between two adjacent slides is the minimum between two
 * numbers: number of common tags and number of
 * different tags. Return -1 if slideshow contains
 * at least a duplicated or a nonpaired vertical photo.
 */
int ps_score_default(struct photoset * p, const char * slideshow);

/* Calculates the score of the given slideshow.
 * The slideshow is a string representation of the
 * photo IDs concatenated with ','. The interest factor
 * between every two adjacent slides are calculated by
 * the given input function. It returns -1 if
 * slideshow contains at least a duplicated photo or a
 * nonpaired vertical photo.
 */
int ps_score(struct photoset * p, const char * slideshow, int (*interest_factor)(unsigned intersection, unsigned differences));

#endif
