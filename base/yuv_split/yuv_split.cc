#include <string>
#include <fstream>

using namespace std;

/**
 * split yuv420 file to y/u/v files
 * @param path: .yuv file path
 * @param w:    frame width
 * @param h:    frame height
 * @param c:    frame count
 */
void yuv420_split(string path, int w, int h, int c)
{
    /**
     * create fp
     */
    ifstream yuv_file(path, ios::in | ios::binary);
    // split files are not [yuv420] any more!
    // we use [y] to present them
    // and u/v data is 128x128 not 256x256
    ofstream y_file("./dist/y_256x256_y.yuv", ios::out | ios::binary | ios::trunc);
    ofstream u_file("./dist/u_128x128_y.yuv", ios::out | ios::binary | ios::trunc);
    ofstream v_file("./dist/v_128x128_y.yuv", ios::out | ios::binary | ios::trunc);

    /**
     * read and write video data
     */
    // yuv420 save half place
    const std::streamsize YUV_SIZE = w * h * 3 / 2;
    char *buffer = new char[YUV_SIZE];

    for (int i = 0; i < c; ++i)
    {
        // read all the bytes
        yuv_file.read(buffer, YUV_SIZE);
        // write to split files
        std::streamsize p = 0;
        // y
        const std::streamsize Y_SIZE = w * h;
        y_file.write(buffer + p, Y_SIZE);
        p += Y_SIZE;
        // u
        const std::streamsize U_SIZE = w * h / 4;
        u_file.write(buffer + p, U_SIZE);
        p += U_SIZE;
        // v
        const std::streamsize V_SIZE = w * h / 4;
        v_file.write(buffer + p, V_SIZE);
    }

    /**
     * clean
     */
    delete buffer;
    yuv_file.close();
    y_file.close();
    u_file.close();
    v_file.close();
}

int main()
{
    yuv420_split(
        "../../resources/lena_256x256_yuv420p.yuv",
        256,
        256,
        1);
}