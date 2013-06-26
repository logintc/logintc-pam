#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>

struct pam_conv const conv = {
    misc_conv,
    NULL
};

int
main(int argc, char** argv)
{
    pam_handle_t* pamh = NULL;
    int retval;
    char const* user;

    if (argc != 2) {
        printf("Usage: ./logintctest [username]\n");
        exit(1);
    }

    user = argv[1];
    retval = pam_start("logintctest", user, &conv, &pamh);

    if (retval == PAM_SUCCESS) {
        printf("LoginTC PAM started successfully. Authenticating user...\n");
        retval = pam_authenticate(pamh, 0);

        if (retval == PAM_SUCCESS) {
            printf("User authenticated!\n");
        } else {
            printf("Error: %s\n", pam_strerror(pamh, retval));
        }
    } else {
        printf("Error: %s\n", pam_strerror(pamh, retval));
    }

    if (pam_end(pamh, retval) != PAM_SUCCESS) {
        pamh = NULL;
        printf("Error: failed to release authenticator.\n");
        exit(1);
    }

    return retval == PAM_SUCCESS ? 0 : 1;
}
