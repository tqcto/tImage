#include <stdio.h>

#include "..\include\tImage.h"

#define IMG_PATH "..\\..\\img.png"

int main(void) {

    tImage::Image src;
    tImage::decodePNG(&src, IMG_PATH);

    tImage::Image dst;
    tImage::transpose(&src, &dst);

    tImage::encodePNG(&dst, "test.png");
    
    return 0;

}