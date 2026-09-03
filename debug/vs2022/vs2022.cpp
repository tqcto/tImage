#include <iostream>

#include <tImage.h>

#define IMG_PATH "F:\\SS\\9512.png"

int main(void) {

    tImage::Image src;
    tImage::decodePNG(&src, IMG_PATH);

    tImage::encodePNG(&src, "test.png");
    
    return 0;

}