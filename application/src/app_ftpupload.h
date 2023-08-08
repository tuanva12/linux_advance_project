/**
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
 */
#ifndef __APP_FTPUPLOAD_H__
#define __APP_FTPUPLOAD_H__

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>

#include <curl/curl.h>


class FtpUpload
{
private:

    CURL *curl;
public:

    std::string usrpwd;
    std::string hostUrl;
    /// @brief Constructer for specify ftp server.
    /// @param hostname
    /// @param usr
    /// @param pwk
    void Init(std::string hostname, std::string usr, std::string pwk);

    /// @brief Upload file to specific directory.
    /// @param sFile Local file must be upload
    /// @param tFile Where file upload to.
    bool UploadFile(std::string sFile, std::string tFile);

};
#endif // __APP_FTPUPLOAD_H__