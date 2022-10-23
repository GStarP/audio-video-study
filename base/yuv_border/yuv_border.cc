#include <string>
#include <fstream>

using namespace std;

/**
 * add a border to yuv420 file
 * @param path: .yuv file path
 * @param w:    frame width
 * @param h:    frame height
 * @param c:    frame count
 * @param border_size (px)
 */
void yuv420_border(string path, int w, int h, int c, int border_size)
{
    /**
     * create fp
     */
    ifstream yuv_file(path, ios::in | ios::binary);
    ofstream border_file("./dist/border_256x256_yuv420p.yuv", ios::out | ios::binary | ios::trunc);

    /**
     * read and write video data
     */
    const std::streamsize YUV_SIZE = w * h * 3 / 2;
    char *buffer = new char[YUV_SIZE];
    for (int i = 0; i < c; ++i)
    {
        yuv_file.read(buffer, YUV_SIZE);
        // in border position, make y=255
        // not a real border, still can see image
        for (int j = 0; j < h; ++j)
        {
            for (int k = 0; k < w; ++k)
            {
                if (j < border_size || j > (h - border_size) || k < border_size || k > (w - border_size))
                {
                    buffer[j * w + k] = 255;
                }
            }
        }
        border_file.write(buffer, YUV_SIZE);
    }

    /**
     * clean
     */
    delete buffer;
    yuv_file.close();
    border_file.close();
}

int main()
{
    yuv420_border(
        "../../resources/lena_256x256_yuv420p.yuv",
        256,
        256,
        1,
        10);
}