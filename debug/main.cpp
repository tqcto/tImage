#include <stdio.h>

#include "..\include\tImage.h"

#define IMG_PATH "..\\..\\img.png"

int main(void) {

    tImage::Image src;
    tImage::decodePNG(&src, IMG_PATH);

    tImage::encodePNG(&src, "test.png");
    
    return 0;

}