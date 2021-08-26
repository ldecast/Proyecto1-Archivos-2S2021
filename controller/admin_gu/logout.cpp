#include "../../model/users_groups.h"
#include "../handler.h"

int logout()
{
    if (!_user_logged.logged_in)
        return coutError("Error: No existe ningún usuario logeado.", NULL);

    Users user;
    _user_logged = user;
    return 1;
}