#include <stdio.h>
#include <assert.h>
#include "slideshow.h"

int my_method(unsigned i, unsigned j){
    return (i-j) * (i-j);
}

int main(){
    struct photoset* p = ps_new("test/photoset1.in");
    
    int s1 = ps_score_default(p, "0,1,2");
//    int s1 = ps_score_default(p, "0,1,2");
//    int s2 = ps_score_default(p, "0,3");
//    int s3 = ps_score_default(p, "0,1,2,3");
//    
//    assert((s1 == 0));
//    assert((s2 == 1));
//    assert((s3 == 1));
    
    ps_delete(p);
}
