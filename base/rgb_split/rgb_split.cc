#include <string>
#include <fstream>

using namespace std;

/**
 * split rgb24 file to r/g/b files
 * @param path: rgb file path
 * @param w:    frame width
 * @param h:    frame height
 * @param c:    frame count
 */
void rgb24_split(string path, int w, int h, int c)
{
    /**
     * create fp
     */
    ifstream rgb_file(path, ios::in | ios::binary);
    // use [y] to present single r/g/b pixels
    ofstream r_file("./dist/r_500x500_y.yuv", ios::out | ios::binary | ios::trunc);
    ofstream g_file("./dist/g_500x500_y.yuv", ios::out | ios::binary | ios::trunc);
    ofstream b_file("./dist/b_500x500_y.yuv", ios::out | ios::binary | ios::trunc);
    /**
     * read and write video data
     */
    const std::streamsize RGB_SIZE = w * h * 3;
    char *buffer = new char[RGB_SIZE];
    for (int i = 0; i < c; ++i)
    {
        rgb_file.read(buffer, RGB_SIZE);
        // for each w*h, read r->g->b
        for (int p = 0; p < RGB_SIZE;)
        {
            r_file.write(buffer + p, 1);
            ++p;
            g_file.write(buffer + p, 1);
            ++p;
            b_file.write(buffer + p, 1);
            ++p;
        }
    }

    /**
     * clean
     */
    delete buffer;
    rgb_file.close();
    r_file.close();
    g_file.close();
    b_file.close();
}

int main()
{
    rgb24_split(
        "../../resources/cie1931_500x500_rgb24p.yuv",
        500,
        500,
        1);
}