#include <string>
#include <fstream>

using namespace std;

/**
 * pcm16 to .wav file
 */
void pcm16_to_wav(int channel_num, int sample_rate)
{
    // pcm16
    const int BITS_PER_SAMPLE = 16;
    /**
     * header of .wav file
     * @ref https://zhuanlan.zhihu.com/p/456281177
     * @ref https://docs.fileformat.com/audio/wav/
     */
    struct WAV_HEADER
    {
        char chunkId[4] = {'R', 'I', 'F', 'F'}; // "RIFF"
        uint32_t chunkSize;                     // size of the whole file (should be filled at the end)
        char format[4] = {'W', 'A', 'V', 'E'};  // "WAVE"
    };
    struct WAV_FMT_HEADER
    {
        // @ATTENTION the remain byte is not '\0' but ' ' !!!
        char subchunk1Id[4] = {'f', 'm', 't', ' '}; // "fmt "
        uint32_t subchunk1Size;                     // 16
        uint16_t audioFormat;                       // 1 => PCM
        uint16_t channelNum;                        // 1 or 2
        uint32_t samplesPerSec;                     // samples/s (like 44100)
        uint32_t bytesPerSec;                       // channelNum * samplePerSec * bitsPerSample / 8
        uint16_t blockAlign;                        // channelNum * bitsPerSample / 8
        uint16_t bitsPerSample;                     // bits/sample
    };
    struct WAV_DATA_HEADER
    {
        char subchunk2Id[4] = {'d', 'a', 't', 'a'}; // "data"
        uint32_t subchunk2Size;                     // audio data size
    };

    /**
     * create fp
     */
    // 44.1kHz  16bit  LittleEndian
    ifstream pcm_file("../../resources/NocturneNo2inEflat_44.1k_s16le.pcm", ios::in | ios::binary);
    ofstream wav_file("./dist/music.wav", ios::out | ios::binary | ios::trunc);

    // set header info
    WAV_HEADER header;
    WAV_FMT_HEADER fmt_header;
    WAV_DATA_HEADER data_header;

    // header
    wav_file.seekp(sizeof(header), ios::cur);
    // fmt header
    fmt_header.subchunk1Size = 16;
    fmt_header.audioFormat = 1;
    fmt_header.channelNum = channel_num;
    fmt_header.samplesPerSec = sample_rate;
    fmt_header.bytesPerSec = channel_num * sample_rate * BITS_PER_SAMPLE / 8;
    fmt_header.blockAlign = channel_num * BITS_PER_SAMPLE / 8;
    fmt_header.bitsPerSample = BITS_PER_SAMPLE;

    wav_file.seekp(sizeof(fmt_header), ios::cur);
    // data header
    wav_file.seekp(sizeof(data_header), ios::cur);
    /**
     * read and write audio data
     */
    unsigned char *sample = new unsigned char[2];
    while (!pcm_file.eof())
    {
        pcm_file.read((char *)sample, 2);
        wav_file.write((char *)sample, 2);
        data_header.subchunk2Size += 2;
    }
    // finish all header info
    header.chunkSize = data_header.subchunk2Size + 36;
    wav_file.seekp(ios::beg);
    wav_file.write((char *)&header, sizeof(header));
    wav_file.write((char *)&fmt_header, sizeof(fmt_header));
    wav_file.write((char *)&data_header, sizeof(data_header));

    /**
     * clean
     */
    delete sample;
    pcm_file.close();
    wav_file.close();
}

int main()
{
    pcm16_to_wav(2, 44100);
}