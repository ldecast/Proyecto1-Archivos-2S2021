#include <iostream>
#include <string>
#include "../model/builder.h"
#include "./disks/mkdisk.cpp"
#include "./disks/rmdisk.cpp"
#include "./disks/fdisk.cpp"
#include "./disks/mount.cpp"
#include "./disks/umount.cpp"
#include "./file_system/mkfs.cpp"
#include "./admin_gu/login.cpp"
#include "./admin_gu/logout.cpp"
#include "./reports/classifier.cpp"

int bloque(struct command x)
{
    if (x.keyword == "__MKDISK")
        return makeDisk(x.size, x.fit, x.unit, x.path);

    if (x.keyword == "__RMDISK")
        return removeDisk(x.path);

    if (x.keyword == "__FDISK")
        return formatDisk(x.size, x.unit, x.path, x.type, x.fit, x.delet, x.name, x.add);

    if (x.keyword == "__MOUNT")
        return mount(x.path, x.name);

    if (x.keyword == "__UMOUNT")
        return umount(x.id);

    if (x.keyword == "__MKFS")
        return mkfs(x.id, x.type, x.fs);

    if (x.keyword == "__MKFS")
        return mkfs(x.id, x.type, x.fs);

    if (x.keyword == "__LOGIN")
        return login(x.user, x.pwd, x.id);

    if (x.keyword == "__LOGOUT")
        return logout();

    return 0;
}