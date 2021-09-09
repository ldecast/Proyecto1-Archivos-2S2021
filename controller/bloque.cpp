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
#include "./admin_gu/mkgrp.cpp"
#include "./admin_gu/rmgrp.cpp"
#include "./admin_gu/mkusr.cpp"
#include "./admin_gu/rmusr.cpp"
#include "./file_system/chmod.cpp"
#include "./file_system/touch.cpp"
#include "./file_system/cat.cpp"
#include "./file_system/rm.cpp"
#include "./file_system/ren.cpp"
#include "./file_system/mv.cpp"
#include "./file_system/cp.cpp"
#include "./file_system/mkdir.cpp"
#include "./file_system/loss.cpp"
#include "./file_system/find.cpp"
#include "./file_system/recovery.cpp"
#include "./reports/classifier.cpp"

int bloque(struct command x)
{
    if (x.keyword == "__MKFS" || x.keyword == "__MKGRP" || x.keyword == "__RMGRP" || x.keyword == "__MKUSR" || x.keyword == "__RMUSR" ||
        x.keyword == "__CHMOD" || x.keyword == "__TOUCH" || x.keyword == "__RM" || x.keyword == "__EDIT" || x.keyword == "__MKDIR" || x.keyword == "__CAT" ||
        x.keyword == "__MV" || x.keyword == "__CP" || x.keyword == "__REN" || x.keyword == "__CHOWN" || x.keyword == "__CHGRP")
    {
        if (_user_logged.logged_in)
        {
            Superbloque sb;
            FILE *file = fopen(_user_logged.mounted.path.c_str(), "rb");
            int part_start = startByteSuperBloque(_user_logged.mounted);
            fseek(file, part_start, SEEK_SET);
            fread(&sb, sizeof(Superbloque), 1, file);
            if (sb.s_magic == -1)
                return coutError("El sistema de archivos se ha corrompido, intente recuperarlo usando Recovery.", file);
            fclose(file);
            file = NULL;
        }
        AddToJournaling(x);
    }

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

    if (x.keyword == "__LOGIN")
        return login(x.user, x.pwd, x.id);

    if (x.keyword == "__LOGOUT")
        return logout();

    if (x.keyword == "__MKGRP")
        return mkgrp(x.name);

    if (x.keyword == "__RMGRP")
        return rmgrp(x.name);

    if (x.keyword == "__MKUSR")
        return mkusr(x.user, x.pwd, x.grp);

    if (x.keyword == "__RMUSR")
        return rmusr(x.user);

    if (x.keyword == "__CHMOD")
        return chmod(x.path, x.ugo, x.r);

    if (x.keyword == "__TOUCH")
    {
        int pre_t = touch(x.path, x.r, x.size, x.cont, x._stdin, false);
        if (pre_t == 777)
        {
            if (!mkdir((x.path).substr(0, x.path.find_last_of('/')), "true"))
                return 0;
            return touch(x.path, x.r, x.size, x.cont, x._stdin, false);
        }
        return pre_t;
    }

    if (x.keyword == "__CAT")
        return cat(x.filen);

    if (x.keyword == "__RM")
        return rm(x.path);

    if (x.keyword == "__EDIT")
        return touch(x.path, x.r, x.size, x.cont, x._stdin, true);

    if (x.keyword == "__MKDIR")
        return mkdir(x.path, x.r);

    if (x.keyword == "__REN")
        return ren(x.path, x.name);

    if (x.keyword == "__MV")
        return mv(x.path, x.dest);

    if (x.keyword == "__CP")
        return cp(x.path, x.dest);

    if (x.keyword == "__FIND")
        return find(x.path, x.name);

    if (x.keyword == "__LOSS")
        return loss(x.id);

    if (x.keyword == "__RECOVERY")
        return recovery(x.id);

    if (x.keyword == "__REP")
        return classifier(x.name, x.path, x.id, x.ruta, x.root);

    return 0;
}