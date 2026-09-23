#include <tImage.h>
#include <core/cpu.h>

#define IMG_PATH "..\\..\\..\\img.png"

using namespace tImage;

t_int main(void) {

    printf("vendor:%s\n", core::t_CPU_INFO.vendor == core::t_cpu_vendor_Intel ? "Intel" : "others");

    Image img;
    decodePNG(&img, IMG_PATH);

    encodePNG(&img, "output.png");

    return 0;

}