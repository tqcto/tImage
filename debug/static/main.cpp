#include <tImage.h>

#define IMG_PATH "..\\..\\..\\img.png"

using namespace tImage;

t_int main(void) {

    Image img;
    decodePNG(&img, IMG_PATH);

    encodePNG(&img, "output.png");

    return 0;

}