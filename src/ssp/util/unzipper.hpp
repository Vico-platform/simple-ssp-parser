
#ifndef SSP_UNZIPPER_HPP
#define SSP_UNZIPPER_HPP

#include "fs_portability.hpp"
#include "temp_dir.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <zip.h>

namespace
{

bool unzip(const ssp::fs::path& zip_file, const ssp::fs::path& tmp_path)
{
    int* err = nullptr;
    zip* za = zip_open(absolute(zip_file).string().c_str(), 0, err);
    if (za == nullptr) {
        return false;
    }

    struct zip_file* zf;
    // clang-format off
    struct zip_stat sb{};
    // clang-format on

    const int bufferSize = 1000;
    std::vector<char> contents(sizeof(char) * bufferSize);
    zip_int64_t sum;
    zip_int64_t len;
    for (int i = 0; i < zip_get_num_entries(za, 0); i++) {
        if (zip_stat_index(za, i, 0, &sb) == 0) {

            const ssp::fs::path newFile = tmp_path / sb.name;

            if (sb.size == 0) {
                ssp::fs::create_directories(newFile);
            } else {
                const auto containingDirectory = newFile.parent_path();
                if (!ssp::fs::exists(containingDirectory) && !ssp::fs::create_directories(containingDirectory)) {
                    return false;
                }
                zf = zip_fopen_index(za, i, 0);

                std::ofstream file;
                file.open(newFile, std::ios::out | std::ios::binary);

                sum = 0;
                while (sum != sb.size) {
                    len = zip_fread(zf, contents.data(), bufferSize);
                    file.write(contents.data(), len);
                    sum += len;
                }

                file.flush();
                file.close();

                zip_fclose(zf);
            }
        }
    }
    zip_close(za);

    return true;
}

std::optional<ssp::temp_dir> unzip(const ssp::fs::path& zip_file)
{
    auto temp = ssp::temp_dir("temp");
    if (unzip(zip_file, temp.path())) {
        return std::move(temp);
    } else {
        return std::nullopt;
    }
}

} // namespace

#endif // SSP_UNZIPPER_HPP