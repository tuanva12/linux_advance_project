/***************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ***************************************************************************/

#include "app_ftpupload.h"

#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

/* <DESC>
 * Performs an FTP upload and renames the file just after a successful
 * transfer.
 * </DESC>
 */

// #define LOCAL_FILE "uploadthis.txt"
// #define UPLOAD_FILE_AS "while-uploading.txt"
// #define REMOTE_URL "ftp://192.168.15.5//home//pi//"
// #define RENAME_FILE_TO "renamed-and-fine.txt"

static size_t read_callback(char *ptr, size_t size, size_t nmemb, void *stream)
{
    unsigned long nread;
    /* in real-world cases, this would probably get this data differently
       as this fread() stuff is exactly what the library already would do
       by default internally */
    size_t retcode = fread(ptr, size, nmemb, (FILE *)stream);

    if (retcode > 0)
    {
        nread = (unsigned long)retcode;
        fprintf(stderr, "*** We read %lu bytes from file\n", nread);
    }

    return retcode;
}


/// @brief
/// @param ipServer
/// @param usr
/// @param pwk
void FtpUpload::Init(std::string ipServer, std::string usr, std::string pwk)
{
    hostUrl = "ftp://" + ipServer + "//home//pi//";
    usrpwd = usr + ':' + pwk;
}

/// @brief
/// @param sFile
/// @param tFile
bool FtpUpload::UploadFile(std::string sFile, std::string tFile)
{
    bool result = false;
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    struct stat file_info;
    unsigned long fsize;

    std::string rmUrl = hostUrl + tFile;

    struct curl_slist *headerlist = NULL;
    // static const char buf_1[] = "RNFR " UPLOAD_FILE_AS;
    // static const char buf_2[] = "RNTO " RENAME_FILE_TO;

    /* get the file size of the local file */
    if (stat(sFile.c_str(), &file_info))
    {
        printf("Couldn't open '%s': %s\n", sFile, strerror(errno));
        result = false;
    }
    else
    {
        fsize = (unsigned long)file_info.st_size;

        printf("Local file size: %lu bytes.\n", fsize);

        /* get a FILE * of the same file */
        hd_src = fopen(sFile.c_str(), "rb");

        /* In windows, this will init the winsock stuff */
        curl_global_init(CURL_GLOBAL_ALL);

        /* get a curl handle */
        curl = curl_easy_init();
        if (curl)
        {
            /* build a list of commands to pass to libcurl */
            // headerlist = curl_slist_append(headerlist, buf_1);
            // headerlist = curl_slist_append(headerlist, buf_2);

            /* we want to use our own read function */
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);

            /* enable uploading */
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

            /* specify target */
            curl_easy_setopt(curl, CURLOPT_URL, rmUrl.c_str());

            /* pass in that last of FTP commands to run after the transfer */
            curl_easy_setopt(curl, CURLOPT_POSTQUOTE, headerlist);

            /* Set user pass */
            curl_easy_setopt(curl, CURLOPT_USERPWD, usrpwd.c_str());

            /* now specify which file to upload */
            curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

            /* Set the size of the file to upload (optional).  If you give a *_LARGE
               option you MUST make sure that the type of the passed-in argument is a
               curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
               make sure that to pass in a type 'long' argument. */
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);

            /* Now run off and do what you have been told! */
            res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            else
            {
                result = true;
            }

            /* clean up the FTP commands list */
            curl_slist_free_all(headerlist);

            /* always cleanup */
            curl_easy_cleanup(curl);
        }
        fclose(hd_src); /* close the local file */

        curl_global_cleanup();
    }

    return result;
}
