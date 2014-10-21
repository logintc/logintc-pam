Introduction
============

LoginTC PAM allows for two-factor authentication to any PAM enabled system.

This package allows an administrator to configure two-factor authentication for SSH access into Unix based systems. 

Installation
============

(tested on CentOS 6.4)

C client dependencies:

    sudo yum install git gcc make curl-devel cmake
    
Get and install the client:

    $ git clone https://github.com/logintc/logintc-c.git
    $ pushd logintc-c
    $ CFLAGS=-O3 cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .
    $ make
    $ sudo make install
    $ popd logintc-c
    
PAM dependencies:

    sudo yum install pam-devel

Get and build LoginTC PAM:

    $ git clone https://github.com/logintc/logintc-pam.git
    $ pushd logintc-pam
    $ CFLAGS=-O3 cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr .
    $ make
    $ sudo make install
    $ popd

Configuring SSH:

Edit /etc/pam.d/sshd and add the following line to the second line (under #%PAM-1.0):

    auth required pam_logintc.so api_key=[api_key] domain_id=[domain_id]
    
Restart sshd:

    $ sudo service sshd restart

Example
=======

The following example will invoke a custom PAM program.

```c
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
        printf("Usage: app [username]\n");
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

```

This program can be compiled with:

    $ gcc -lpam -lpam_misc -o logintctest logintctest.c

Create a logintctest PAM configuration:

    vi /etc/pam.d/logintctest and add the following line to the second line (under #%PAM-1.0):

    #%PAM-1.0
    auth required pam_logintc.so api_key=[api_key] domain_id=[domain_id]

Execute the program with:

    $ ./logintctest [username]

This program invokes the PAM chain in /etc/pam.d/logintctest to authenticate the user.

Additional Notes
================

C client installs the following files:

    include/*.h (header files)
    lib/liblogintc.a (static library)
    lib/liblogintc.la (libtool metadata)
    lib/liblogintc.so (symlink)
    lib/liblogintc.so.0 (symlink)
    lib/liblogintc.so.0.0.0 (shared library)
    
LoginTC PAM installs the following files:

    /lib/security/pam_logintc.a (static library)
    /lib/security/pam_logintc.la (libtool metadata)
    /lib/security/pam_logintc.so (symlink)
    /lib/security/pam_logintc.so.0 (symlink)
    /lib/security/pam_logintc.so.0.0.0 (shared library)

You can also bypass the autotools process and compile the object manually.

For C client:

    $ pushd logintc-c
    $ gcc -Isrc -lcurl -fPIC -shared -o liblogintc.so src/logintc/*.c
    $ popd
    
For PAM:

    $ gcc -llogintc -fPIC -shared -o pam_logintc.so src/pam_logintc.c

Uninstall C client:

    $ pushd logintc-c
    $ sudo make uninstall
    $ popd logintc-c
    
Uninstall LoginTC PAC:

    $ pushd logintc-pam
    $ sudo make uninstall
    $ popd logintc-pam

Documentation
=============

See <https://www.logintc.com/docs>

Help
====

Email: <support@cyphercor.com>

<https://www.logintc.com>

[rest-api]: https://www.logintc.com/docs/rest-api
