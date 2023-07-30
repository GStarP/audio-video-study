#include <iostream>
extern "C"
{
#include <libavformat/avformat.h>
}

int main()
{
    av_register_all();
    avcodec_register_all();

    std::cout << ">>> ffmpeg hello world program <<<" << std::endl;

    const char *dir_name = ".";
    AVIODirContext *dir_context = nullptr;

    int ret = avio_open_dir(&dir_context, dir_name, nullptr);
    if (ret < 0)
    {
        std::cerr << "Error opening directory" << std::endl;
        return 1;
    }

    AVIODirEntry *entry = nullptr;
    while ((ret = avio_read_dir(dir_context, &entry)) == 0)
    {
        if (!entry)
            break;

        std::cout << entry->name << std::endl;
        avio_free_directory_entry(&entry);
    }

    avio_close_dir(&dir_context);

    return 0;
}