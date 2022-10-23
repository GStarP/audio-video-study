#include <string>
#include <fstream>

using namespace std;

/**
 * make yuv420 file grey
 * @param path: .yuv file path
 * @param w:    frame width
 * @param h:    frame height
 * @param c:    frame count
 */
void yuv420_grey(string path, int w, int h, int c)
{
    /**
     * create fp
     */
    ifstream yuv_file(path, ios::in | ios::binary);
    ofstream grey_file("./dist/grey_256x256_yuv420p.yuv", ios::out | ios::binary | ios::trunc);

    /**
     * read and write video data
     */
    const std::streamsize YUV_SIZE = w * h * 3 / 2;
    char *buffer = new char[YUV_SIZE];
    for (int i = 0; i < c; ++i)
    {
        yuv_file.read(buffer, YUV_SIZE);
        // make u/v all 128
        // in [-128, 127], 0 is none-color
        // but in (char)[0, 256], 128 is none-color
        for (int p = w * h; p < YUV_SIZE; ++p)
        {
            buffer[p] = 128;
        }
        grey_file.write(buffer, YUV_SIZE);
    }

    /**
     * clean
     */
    delete buffer;
    yuv_file.close();
    grey_file.close();
}

int main()
{
    yuv420_grey(
        "../../resources/lena_256x256_yuv420p.yuv",
        256,
        256,
        1);
}