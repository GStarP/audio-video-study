#include <string>
#include <fstream>

using namespace std;

/**
 * transfer rgb file to bmp file
 * @param path: rgb file path
 * @param w:    frame width
 * @param h:    frame height
 */
void rgb_to_bmp(string path, int w, int h)
{
    /**
     * define header info structure
     * reference: https://zh.wikipedia.org/wiki/BMP
     */
    struct tagBITMAPFILEHEADER
    {
        // @ATTENTION
        // if we add this, sizeof will return 16
        // but not the correct 14!
        // short bfType;      // type of bitmap (=BM)
        long bfSize;       // bmp file size (bytes)
        short bfReserved1; // not use (=0)
        short bfReserved2; // not use (=0)
        long bfOffBits;    // offset -> image data
    };

    struct tagBITMAPINFOHEADER
    {

        long biSize;          // size of this header (bytes)
        long biWidth;         // image width (px)
        long biHeight;        // image height (px)
        short biPlanes;       // (=1)
        short biBitcount;     // bit depth (use how many bit to represent r/g/b)
        long biCompression;   // compress format (0~6)
        long biSizeImage;     // image size (bytes)
        long biXPelsPermeter; // x-axis pixel num
        long biYPelsPermeter; // y-axis pixel num
        long biClrUsed;       // color space num
        long biClrImportant;  // important color num
    };

    /**
     * create fp
     */
    ifstream rgb_file(path, ios::in | ios::binary);
    ofstream bmp_file("./dist/lena_256x256_rgb24p.bmp", ios::out | ios::binary | ios::trunc);

    /**
     * read rgb data
     */
    const std::streamsize RGB_SIZE = w * h * 3;
    char *buffer = new char[RGB_SIZE];
    rgb_file.read(buffer, RGB_SIZE);

    /**
     * set header metadata
     */
    tagBITMAPFILEHEADER file_header = {0};
    tagBITMAPINFOHEADER info_header = {0};

    /* file header */
    // bfType of bitmap should be "BM"
    char bfType[2] = {'B', 'M'};
    // bfSize includes header size and image size
    int header_size = sizeof(file_header) + sizeof(info_header);
    int image_size = RGB_SIZE;
    file_header.bfSize = header_size + image_size;
    // reserved attr all 0
    file_header.bfReserved1 = 0;
    file_header.bfReserved2 = 0;
    // after header data begins image data
    file_header.bfOffBits = header_size;

    /* info header */
    info_header.biSize = sizeof(info_header);
    info_header.biWidth = w;
    info_header.biHeight = -h;
    info_header.biPlanes = 1;
    // rgb24 use 24 bit for r/g/b
    info_header.biBitcount = 24;
    info_header.biCompression = 0;
    info_header.biSizeImage = RGB_SIZE;
    // TODO: seems not used
    info_header.biXPelsPermeter = 0;
    info_header.biYPelsPermeter = 0;
    // 0 means color space depended on biBitcount
    info_header.biClrUsed = 0;
    info_header.biClrImportant = 0;

    /**
     * write header data
     */
    // write bfType manually
    bmp_file.write(bfType, sizeof(bfType));
    bmp_file.write((char *)&file_header, sizeof(file_header));
    bmp_file.write((char *)&info_header, sizeof(info_header));
    // switch r <-> b:
    // because bmp save image data by "little endian"
    // which means r/g/b are stored as b/g/r
    for (int j = 0; j < h; ++j)
    {
        for (int i = 0; i < w; ++i)
        {
            char tmp = buffer[(j * w + i) * 3 + 2];
            buffer[(j * w + i) * 3 + 2] = buffer[(j * w + i) * 3];
            buffer[(j * w + i) * 3] = tmp;
        }
    }
    // write image data
    bmp_file.write(buffer, RGB_SIZE);

    /**
     * clean
     */
    delete buffer;
    rgb_file.close();
    bmp_file.close();
}

int main()
{
    rgb_to_bmp("../../resources/lena_256x256_rgb24p.yuv", 256, 256);
}