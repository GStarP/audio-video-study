#include <string>
#include <fstream>

using namespace std;

/**
 * limit x to [min, max]
 * @param x   value to be limited
 * @param min x min to be
 * @param max x max to be
 */
unsigned char limit(unsigned char x, unsigned char min, unsigned char max)
{
    if (x > max)
        return max;
    if (x < min)
        return min;
    return x;
}

/**
 * transfer rgb file to yuv file
 * @param path rgb file path
 * @param w    frame width
 * @param h    frame height
 */
void rgb_to_yuv(string path, int w, int h)
{
    /**
     * create fp
     */
    ifstream rgb_file(path, ios::in | ios::binary);
    ofstream yuv_file("./dist/lena_256x256_yuv420p.yuv", ios::out | ios::binary | ios::trunc);

    /**
     * read and write
     */
    const std::streamsize RGB_SIZE = w * h * 3;
    unsigned char *rgb_buffer = new unsigned char[RGB_SIZE];
    const std::streamsize YUV_SIZE = w * h * 3 / 2;
    const std::streamsize Y_SIZE = w * h;
    const std::streamsize U_SIZE = w * h / 4;
    unsigned char *yuv_buffer = new unsigned char[YUV_SIZE];

    rgb_file.read((char *)rgb_buffer, RGB_SIZE);

    // yuv write ptr
    unsigned char *y_ptr = yuv_buffer;
    unsigned char *u_ptr = yuv_buffer + Y_SIZE;
    unsigned char *v_ptr = yuv_buffer + Y_SIZE + U_SIZE;

    for (int j = 0; j < h; ++j)
    {
        // rgb read ptr
        unsigned char *rgb_ptr = rgb_buffer + j * w * 3;
        for (int i = 0; i < w; ++i)
        {
            // read r->g->b
            unsigned char r = *(rgb_ptr++);
            unsigned char g = *(rgb_ptr++);
            unsigned char b = *(rgb_ptr++);
            // compute y/u/v by r/g/b
            unsigned char y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            unsigned char u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            unsigned char v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
            // write y for every row
            *(y_ptr++) = limit(y, 0, 255);
            // write u&v for every second row
            // write u/v alternatively in this row
            // y y y y
            // v u v u
            // y y y y
            if (i % 2 == 0)
            {
                if (j % 2 == 0)
                {
                    *(u_ptr++) = limit(u, 0, 255);
                }
                else
                {
                    *(v_ptr++) = limit(v, 0, 255);
                }
            }
        }
    }

    yuv_file.write((char *)yuv_buffer, YUV_SIZE);

    /**
     * clean
     */
    delete rgb_buffer;
    delete yuv_buffer;
    rgb_file.close();
    yuv_file.close();
}

int main()
{
    rgb_to_yuv("../../resources/lena_256x256_rgb24p.yuv", 256, 256);
}