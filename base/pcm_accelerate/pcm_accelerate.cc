#include <string>
#include <fstream>

using namespace std;

/**
 * accelerate pcm16
 */
void pcm_accelerate()
{
    /**
     * create fp
     */
    // 44.1kHz  16bit  LittleEndian
    ifstream pcm_file("../../resources/NocturneNo2inEflat_44.1k_s16le.pcm", ios::in | ios::binary);
    ofstream accelerated_file("./dist/accelerated.pcm", ios::out | ios::binary | ios::trunc);

    /**
     * read and write audio data
     */
    unsigned char *buffer = new unsigned char[4];
    int flag = 0;
    while (!pcm_file.eof())
    {
        pcm_file.read((char *)buffer, 4);

        // drop half samples
        if (flag % 2 == 0)
        {

            accelerated_file.write((char *)buffer, 2);
            accelerated_file.write((char *)buffer + 2, 2);
        }
        flag++;
    }

    /**
     * clean
     */
    delete buffer;
    pcm_file.close();
    accelerated_file.close();
}

int main()
{
    pcm_accelerate();
}