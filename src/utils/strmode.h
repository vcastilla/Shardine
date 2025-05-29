#ifndef STRMODE_H
#define STRMODE_H

#include <array>
#include <cstdint>
#include <string>
#include <utility>

namespace ino {

// clang-format off
constexpr std::uint16_t type          = 0170000; // this field gives inode type
constexpr std::uint16_t unix_socket   = 0140000; // unix domain socket
constexpr std::uint16_t symbolic_link = 0120000; // file is a symbolic link
constexpr std::uint16_t regular       = 0100000; // regular file, not dir or special
constexpr std::uint16_t block_special = 0060000; // block special file
constexpr std::uint16_t directory     = 0040000; // file is a directory
constexpr std::uint16_t char_special  = 0020000; // character special file
constexpr std::uint16_t named_pipe    = 0010000; // named pipe (FIFO)
constexpr std::uint16_t set_uid       = 0004000; // set effective uid_t on exec
constexpr std::uint16_t set_gid       = 0002000; // set effective gid_t on exec
constexpr std::uint16_t set_stcky     = 0001000; // sticky bit
constexpr std::uint16_t all_modes     = 0007777; // all bits for user, group and others
constexpr std::uint16_t rwx_modes     = 0000777; // mode bits for RWX only
constexpr std::uint16_t r_usr         = 0000400; // Rwx user
constexpr std::uint16_t w_usr         = 0000200; // rWx user
constexpr std::uint16_t x_usr         = 0000100; // rwX user
constexpr std::uint16_t r_grp         = 0000040; // Rwx group
constexpr std::uint16_t w_grp         = 0000020; // rWx group
constexpr std::uint16_t x_grp         = 0000010; // rwX group
constexpr std::uint16_t r_oth         = 0000004; // Rwx others
constexpr std::uint16_t w_oth         = 0000002; // rWx others
constexpr std::uint16_t x_oth         = 0000001; // rwX others
constexpr std::uint16_t not_alloc     = 0000000; // this inode is free

constexpr auto perms= std::to_array<std::pair<std::uint16_t, char>>({
        {r_usr, 'r'}, {w_usr, 'w'}, {x_usr, 'x'},
        {r_grp, 'r'}, {w_grp, 'w'}, {x_grp, 'x'},
        {r_oth, 'r'}, {w_oth, 'w'}, {x_oth, 'x'}
});
// clang-format on

constexpr char type_to_str(const std::uint16_t mode) {
    switch (mode & type) {
        case unix_socket:
            return 's';
        case symbolic_link:
            return 'l';
        case regular:
            return '-';
        case block_special:
            return 'b';
        case directory:
            return 'd';
        case char_special:
            return 'c';
        case named_pipe:
            return 'p';
        default:
            return '?';
    }
}

inline std::string mode_to_str(const std::uint16_t mode) {
    if (mode == not_alloc)
        return {};

    std::string flags(10, '-');

    flags[0] = type_to_str(mode);

    for (unsigned i = 0; i < perms.size(); i++) {
        if (mode & perms[i].first)
            flags[i + 1] = perms[i].second;
    }

    if (mode & set_uid)
        flags[3] = mode & x_usr ? 's' : 'S';

    if (mode & set_gid)
        flags[6] = mode & x_grp ? 's' : 'S';

    if (mode & set_stcky)
        flags[9] = mode & x_oth ? 't' : 'T';

    return flags;
}

} // namespace ino

#endif // STRMODE_H
