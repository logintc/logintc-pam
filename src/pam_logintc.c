#include <logintc/globals.h>
#include <logintc/logintc.h>
#include <logintc/session.h>
#include <security/pam_modules.h>
#include <time.h>
#include <unistd.h>

#define _PAM_LOGINTC_API_KEY       "api_key="
#define _PAM_LOGINTC_DOMAIN_ID     "domain_id="
#define _PAM_LOGINTC_TIMEOUT       "timeout="
#define _PAM_LOGINTC_POLL_INTERVAL "poll_interval="

double const static _DEFAULT_TIMEOUT = 60;
unsigned int const static _DEFAULT_POLL_INTERVAL = 1;
double const static _MIN_TIMEOUT = 0;
unsigned int const static _MIN_POLL_INTERVAL = 1;

PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc,
        char const** argv) {
    int rc;
    int i;
    char const* api_key = NULL;
    char const* domain_id = NULL;
    char const* user_name;
    double timeout = _DEFAULT_TIMEOUT;
    unsigned int poll_interval = _DEFAULT_POLL_INTERVAL;
    logintc_t * logintc;
    logintc_session_t* session = NULL;
    logintc_error_t* error;
    time_t start;
    time_t now;

    if (pam_get_item(pamh, PAM_USER, (void const**) &user_name) != PAM_SUCCESS) {
        return PAM_CRED_INSUFFICIENT;
    }

    for (i = 0; i < argc; ++i) {
        if (strncmp(argv[i], _PAM_LOGINTC_API_KEY,
                sizeof(_PAM_LOGINTC_API_KEY) - 1) == 0) {
            api_key = argv[i] + sizeof(_PAM_LOGINTC_API_KEY) - 1;
        } else if (strncmp(argv[i], _PAM_LOGINTC_DOMAIN_ID,
                sizeof(_PAM_LOGINTC_DOMAIN_ID) - 1) == 0) {
            domain_id = argv[i] + sizeof(_PAM_LOGINTC_DOMAIN_ID) - 1;
        } else if (strncmp(argv[i], _PAM_LOGINTC_TIMEOUT,
                sizeof(_PAM_LOGINTC_TIMEOUT) - 1) == 0) {
            timeout = atof(argv[i] + sizeof(_PAM_LOGINTC_TIMEOUT) - 1);
        } else if (strncmp(argv[i], _PAM_LOGINTC_POLL_INTERVAL,
                sizeof(_PAM_LOGINTC_POLL_INTERVAL) - 1) == 0) {
            poll_interval = atoi(
                    argv[i] + sizeof(_PAM_LOGINTC_POLL_INTERVAL) - 1);
        }
    }

    if (api_key == NULL || domain_id == NULL) {
        return PAM_AUTH_ERR;
    }

    if (timeout < _MIN_TIMEOUT) {
        timeout = _DEFAULT_TIMEOUT;
    }

    if (poll_interval < _MIN_POLL_INTERVAL) {
        poll_interval = _DEFAULT_POLL_INTERVAL;
    }

    logintc = logintc_logintc_new(api_key);

    error = logintc_error_new();

    session = logintc_create_session_with_username(logintc, domain_id,
            user_name, error);

    rc = PAM_AUTH_ERR;

    if (session == NULL) {
        fprintf(stderr,
                "Error creating session. Type: %d, Code: %d, Message %s\n",
                error->error_type, error->error_code, error->error_message);
    } else {

        start = time(NULL);
        while (1) {
            now = time(NULL);
            if (difftime(now, start) > timeout) {
                break;
            }

            logintc_get_session(logintc, domain_id, &session);

            if (session->state == LOGINTC_SESSION_PENDING) {
                break;
            }

            sleep(poll_interval);
        }

        if (session->state == LOGINTC_SESSION_APPROVED) {
            rc = PAM_SUCCESS;
        }
    }

    logintc_error_free(error);
    logintc_session_free(session);
    logintc_logintc_free(logintc);

    return rc;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t* pamh, int flags, int argc,
        char const** argv) {
    return PAM_SUCCESS;
}
