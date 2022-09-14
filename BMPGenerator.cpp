#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>
#include "C:/Users/mario/CppStuff/Graphing/gregstring.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

#pragma pack(push, 1)

typedef struct {
    unsigned char b;
    unsigned char g;
    unsigned char r;
} colour;

std::ostream &operator<<(std::ostream &out, const colour &col) {
    return out << "R: " << +col.r << ", G: " << +col.g << ", B: " << +col.b;
}

#define PADDING(w) (3*(w) % 4 == 0 ? 0 : 4 - (3*(w) % 4))

template <typename... pack> requires ((std::is_fundamental<pack>::value) && ...)
auto average(pack... args) {
    if constexpr (sizeof...(pack) == 0) {
        return 0;
    }
    else {
        return (args + ...)/sizeof...(args);
    }
}

int main(int argc, char **argv) {

    if (argc < 5 || argc > 7) {
        std::cerr << "Invalid number of command-line arguments (5-7 expected).\n";
        return -1;
    }

    const char *ptr1 = *(argv + 2);
    const char *ptr2 = *(argv + 3);
    const char *ptr3 = *(argv + 4);

    int width = 0;
    int height = 0;
    size_t num_images = 0;

    while (*ptr1) {
        if (!(*ptr1 >= 48 && *ptr1 <= 57)) {
            std::cerr << "3rd command-line argument is not numeric.\n";
            return -1;
        }
        width *= 10;
        width += *ptr1 - 48;
        ++ptr1;
    }
    while (*ptr2) {
        if (!(*ptr2 >= 48 && *ptr2 <= 57)) {
            std::cerr << "4th command-line argument is not numeric.\n";
            return -1;
        }
        height *= 10;
        height += *ptr2 - 48;
        ++ptr2;
    }
    while (*ptr3) {
        if (!(*ptr3 >= 48 && *ptr3 <= 57)) {
            std::cerr << "5th command-line argument is not numeric.\n";
            return -1;
        }
        num_images *= 10;
        num_images += *ptr3 - 48;
        ++ptr3;
    }
    size_t num_reps = width*height*10;
    unsigned int starting_colour = (255 << 16) + (255 << 8) + 255;
    if (argc >= 6) {
        num_reps = 0;
        const char *ptr4 = *(argv + 5);
        while (*ptr4) {
            if (!(*ptr4 >= 48 && *ptr4 <= 57)) {
                std::cerr << "6th command-line argument is not numeric.\n";
                return -1;
            }
            num_reps *= 10;
            num_reps += *ptr4 - 48;
            ++ptr4;
        }
        if (argc == 7) {
            starting_colour = 0;
            const char *ptr5 = *(argv + 6);
            while (*ptr5) {
                if (!(*ptr5 >= 48 && *ptr5 <= 57)) {
                    std::cerr << "6th command-line argument is not numeric.\n";
                    return -1;
                }
                starting_colour *= 10;
                starting_colour += *ptr5 - 48;
                ++ptr5;
            }
        }
    }
    colour initial_colour = {static_cast<unsigned char>(starting_colour & 255),
                             static_cast<unsigned char>((starting_colour >> 8) & 255),
                             static_cast<unsigned char>((starting_colour >> 16) & 255)};
#ifdef _WIN32
    DWORD fileAttributes = GetFileAttributesA(*(argv + 1));
    if (fileAttributes == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "Specified file does not exist or a reading error occurred.\n";
        return -1;
    }
    if ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
        std::cerr << "Specified file is not a directory.\n";
        return -1;
    }
#else
    struct stat64 buff{};
    if (stat64(*(argv + 1), &buff) == -1) {
        std::cerr << "Specified file does not exist or a reading error occurred.\n";
        return -1;
    }
    if (!S_ISDIR(buff.st_mode)) {
        std::cerr << "Specified file is not a directory.\n";
        return -1;
    }
#endif

    gtd::String path(*(argv + 1));
#ifdef _WIN32
    path.append_back("\\SampleBMPImage");
#else
    path.append_back("/SampleBMPImage");
#endif
    unsigned char pad = PADDING(width);
    int fileSize = 54 + width*height*3 + height*pad;

    std::vector<unsigned char> header;
    header.push_back('B');
    header.push_back('M');

    header.push_back(fileSize);
    header.push_back(fileSize >> 8);
    header.push_back(fileSize >> 16);
    header.push_back(fileSize >> 24);

    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);

    header.push_back(54);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);


    header.push_back(40);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(width);
    header.push_back(width >> 8);
    header.push_back(width >> 16);
    header.push_back(width >> 24);

    header.push_back(height);
    header.push_back(height >> 8);
    header.push_back(height >> 16);
    header.push_back(height >> 24);

    header.push_back(1);
    header.push_back(0);

    header.push_back(24);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    header.push_back(0);
    header.push_back(0);
    header.push_back(0);
    header.push_back(0);

    std::ofstream out;
    std::random_device r;
    unsigned char padding[4]{};
    size_t count = 1;
    std::vector<std::vector<colour>> image(height, std::vector<colour>(width));
    size_t x;
    size_t y;
    colour avg;
    size_t nr_cpy;
    size_t log = static_cast<size_t>(std::log10(num_images));
    size_t temp_log;
    while (count <= num_images) {
        temp_log = log - static_cast<size_t>(std::log10(count++));
        for (size_t i = 0; i < temp_log; ++i) {
            path.append_back("0");
        }
        path.append_back(count - 1).append_back(".bmp");
        out.open(path.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
        if (!out.good()) {
            std::cerr << "File path: " << path << std::endl;
            std::cerr << "Error opening BMP file number: " << count << '\n';
            return -1;
        }
        for (std::vector<colour> &vec : image) {
            for (colour &col : vec) {
                col = initial_colour;
            }
        }
        nr_cpy = num_reps;
        while (nr_cpy --> 0) {
            x = r() % width;
            y = r() % height;
            if (x == 0 && y == 0) {
                avg.b = average(image[0][1].b, image[1][1].b, image[1][0].b);
                avg.g = average(image[0][1].g, image[1][1].g, image[1][0].g);
                avg.r = average(image[0][1].r, image[1][1].r, image[1][0].r);
            }
            else if (x == width - 1 && y == height - 1) {
                avg.b = average(image[y - 1][x].b, image[y - 1][x - 1].b, image[y][x - 1].b);
                avg.g = average(image[y - 1][x].g, image[y - 1][x - 1].g, image[y][x - 1].g);
                avg.r = average(image[y - 1][x].r, image[y - 1][x - 1].r, image[y][x - 1].r);
            }
            else if (x == width - 1 && y == 0) {
                avg.b = average(image[y + 1][x - 1].b, image[y + 1][x].b, image[y][x - 1].b);
                avg.g = average(image[y + 1][x - 1].g, image[y + 1][x].g, image[y][x - 1].g);
                avg.r = average(image[y + 1][x - 1].r, image[y + 1][x].r, image[y][x - 1].r);
            }
            else if (y == height - 1 && x == 0) {
                avg.b = average(image[y - 1][x].b, image[y - 1][x + 1].b, image[y][x + 1].b);
                avg.g = average(image[y - 1][x].g, image[y - 1][x + 1].g, image[y][x + 1].g);
                avg.r = average(image[y - 1][x].r, image[y - 1][x + 1].r, image[y][x + 1].r);
            }
            else if (x == width - 1) {
                avg.b = average(image[y - 1][x].b, image[y - 1][x - 1].b, image[y][x - 1].b, image[y + 1][x - 1].b, image[y + 1][x].b);
                avg.g = average(image[y - 1][x].g, image[y - 1][x - 1].g, image[y][x - 1].g, image[y + 1][x - 1].g, image[y + 1][x].g);
                avg.r = average(image[y - 1][x].r, image[y - 1][x - 1].r, image[y][x - 1].r, image[y + 1][x - 1].r, image[y + 1][x].r);
            }
            else if (y == height - 1) {
                avg.b = average(image[y - 1][x].b, image[y - 1][x - 1].b, image[y][x - 1].b, image[y - 1][x + 1].b, image[y][x + 1].b);
                avg.g = average(image[y - 1][x].g, image[y - 1][x - 1].g, image[y][x - 1].g, image[y - 1][x + 1].g, image[y][x + 1].g);
                avg.r = average(image[y - 1][x].r, image[y - 1][x - 1].r, image[y][x - 1].r, image[y - 1][x + 1].r, image[y][x + 1].r);
            }
            else if (x == 0) {
                avg.b = average(image[y + 1][x].b, image[y - 1][x].b, image[y + 1][x + 1].b, image[y][x + 1].b, image[y - 1][x + 1].b);
                avg.g = average(image[y + 1][x].g, image[y - 1][x].g, image[y + 1][x + 1].g, image[y][x + 1].g, image[y - 1][x + 1].g);
                avg.r = average(image[y + 1][x].r, image[y - 1][x].r, image[y + 1][x + 1].r, image[y][x + 1].r, image[y - 1][x + 1].r);
            }
            else if (y == 0) {
                avg.b = average(image[y + 1][x - 1].b, image[y][x - 1].b, image[y + 1][x].b, image[y + 1][x + 1].b, image[y][x + 1].b);
                avg.g = average(image[y + 1][x - 1].g, image[y][x - 1].g, image[y + 1][x].g, image[y + 1][x + 1].g, image[y][x + 1].g);
                avg.r = average(image[y + 1][x - 1].r, image[y][x - 1].r, image[y + 1][x].r, image[y + 1][x + 1].r, image[y][x + 1].r);
            }
            else {
                avg.b = average(image[y - 1][x].b, image[y - 1][x - 1].b, image[y][x - 1].b, image[y + 1][x - 1].b, image[y + 1][x].b,
                                image[y - 1][x + 1].b, image[y][x + 1].b, image[y + 1][x + 1].b);
                avg.g = average(image[y - 1][x].g, image[y - 1][x - 1].g, image[y][x - 1].g, image[y + 1][x - 1].g, image[y + 1][x].g,
                                image[y - 1][x + 1].g, image[y][x + 1].g, image[y + 1][x + 1].g);
                avg.r = average(image[y - 1][x].r, image[y - 1][x - 1].r, image[y][x - 1].r, image[y + 1][x - 1].r, image[y + 1][x].r,
                                image[y - 1][x + 1].r, image[y][x + 1].r, image[y + 1][x + 1].r);
            }
            image[y][x].b = avg.b + (avg.b == 0 ? 10 : (avg.b == 255 ? -10 : (r() % 2 == 0 ? 10 : -10)));
            image[y][x].g = avg.g + (avg.g == 0 ? 10 : (avg.g == 255 ? -10 : (r() % 2 == 0 ? 10 : -10)));
            image[y][x].r = avg.r + (avg.r == 0 ? 10 : (avg.r == 255 ? -10 : (r() % 2 == 0 ? 10 : -10)));
        }
        out.write((char *) &header[0], 54);
        for (const std::vector<colour> &v : image) {
            for (const colour &c : v) {
                out.write(reinterpret_cast<const char*>(&c), 3);
            }
            out.write(reinterpret_cast<char*>(padding), pad);
        }
        path.erase_chars(path.r_find('e') + 1);
        out.close();
    }
    size_t total_size = ((size_t) fileSize)*(num_images);
    std::cout << "Total size occupied on disk: " << total_size << " bytes\n";
    return 0;
}
#pragma pack(pop)