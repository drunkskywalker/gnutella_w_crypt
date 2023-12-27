#include "FileUtilHandler.hpp"

/**
 * @brief Log error
 * logs an error. returns 0 if log was successful, -1 otherwise
 * @param msg the message to log
 * @param logger the logger object to do the logging
*/
int File_Util_Handler::logError(std::string msg) {
  if (logger->logError(msg)) {
    return 0;
  }
  return -1;
}

/**
 * @brief Log event
 * logs an event. returns 0 if log was successful, -1 otherwise
 * @param msg the message to log
 * @param logger the logger object to do the logging
*/
int File_Util_Handler::logEvent(std::string msg) {
  if (logger->logEvent(msg)) {
    return 0;
  }
  return -1;
}

/**
 * @brief sha256 hashes message
 * stores the hash of message in digest. returns 0 if successful, -1 otherwise
 * @param message the message to digest
 * @param message_len the length of the message
 * @param digest the digest of the message
 * @param digest_len the length of the digest
*/
int File_Util_Handler::digest_message(const unsigned char * message,
                                      size_t message_len,
                                      unsigned char ** digest,
                                      unsigned int * digest_len) {
  EVP_MD_CTX * mdctx;

  if ((mdctx = EVP_MD_CTX_new()) == NULL) {
    return logError("Error creating context in digest_message");
  }
  if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
    return logError("Error initializing digest in digest_message");
  }

  if (1 != EVP_DigestUpdate(mdctx, message, message_len)) {
    return logError("Error updating digest in digest_message");
  }

  if ((*digest = (unsigned char *)OPENSSL_malloc(EVP_MD_size(EVP_sha256()))) == NULL) {
    return logError("Error allocating digest in digest_message");
  }

  if (1 != EVP_DigestFinal_ex(mdctx, *digest, digest_len)) {
    return logError("Error finalizing digest in digest_message");
  }

  EVP_MD_CTX_free(mdctx);
  return 0;
}

/**
 * @brief checks if input string is a valid sha256 hash in hex
 * returns true if hash is valid, false otherwise
 * @param hash the hash to check
*/
bool File_Util_Handler::isValidHash(std::string hash) {
  try {
    if (hash.length() != 64) {
      // wrong length
      return false;
    }
    for (int i = 0; i < 64; i++) {
      if (!isxdigit(hash[i])) {
        // not a hex digit
        return false;
      }
    }
    return true;
  }
  catch (std::exception & e) {
    logError("Error checking if hash is valid");
    return false;
  }
}

/**
 * @brief hashes a file with given absolute file path.
 * returns the hash of the file at filePath
 * @param filePath the absolute path to the file
*/
std::string File_Util_Handler::hashFile(std::string filePath) {
  try {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
      logError("Error opening file");
      return "";
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    file.close();
    const std::string & s = ss.str();
    unsigned char * digest;
    unsigned int digest_len;
    digest_message((unsigned char *)s.c_str(), s.length(), &digest, &digest_len);
    std::ostringstream hashs;
    hashs << std::hex << std::setfill('0');
    for (int i = 0; i < digest_len; i++) {
      hashs << std::setw(2) << (int)digest[i];
    }
    OPENSSL_free(digest);
    logEvent("Hashed file " + filePath + " to " + hashs.str());
    return hashs.str();
  }
  catch (std::exception & e) {
    logError("Error hashing file " + filePath);
    return "";
  }
}

/**
 * @brief hashes a char array with given size.
 * returns the hash of the char array
 * @param data the char array to hash
 * @param size the size of the char array
*/
std::string File_Util_Handler::hashCharArray(char * data, size_t size) {
  try {
    const unsigned char * message = (unsigned char *)data;
    unsigned char * digest;
    unsigned int digest_len;
    digest_message(message, size, &digest, &digest_len);
    std::ostringstream hashs;
    hashs << std::hex << std::setfill('0');
    for (int i = 0; i < digest_len; i++) {
      hashs << std::setw(2) << (int)digest[i];
    }
    OPENSSL_free(digest);
    return hashs.str();
  }
  catch (std::exception & e) {
    logError("Error hashing char array");
    return "";
  }
}

/**
 * @brief checks if a file at filePath matches a given hash
 * returns true if file matches hash, false otherwise
 * @param filePath the absolute path to the file
 * @param hash the hash to check against
*/
bool File_Util_Handler::fileMatchHash(std::string filePath, std::string hash) {
  std::string fileHash = hashFile(filePath);
  return fileHash.compare(hash) == 0;
}

/**
 * @brief checks if a char array matches a given hash
 * returns true if char array matches hash, false otherwise
 * @param data the char array to check
 * @param size the size of the char array
 * @param hash the hash to check against
*/
bool File_Util_Handler::charArrayMatchHash(char * data, size_t size, std::string hash) {
  std::string charArrayHash = hashCharArray(data, size);
  return charArrayHash.compare(hash) == 0;
}

/**
 * @brief gets the size of a file at filePath
 * returns the size of the file at filePath
 * @param filePath the absolute path to the file
*/
size_t File_Util_Handler::getFileSize(std::string filePath) {
  try {
    FILE * fptr = fopen(filePath.c_str(), "rb");
    if (fptr == NULL) {
      logError("Error opening file " + filePath);
      return -1;
    }
    // get file size
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
  }
  catch (std::exception & e) {
    logError("Error getting file size of file " + filePath);
    return -1;
  }
}

/**
 * @brief gets all files a directory.
 * returns a vector of all files in a directory
 * @param path the path to the directory
 * @param recursive whether to get files in subdirectories
*/
std::vector<std::string> File_Util_Handler::getAllFiles(std::string path,
                                                        bool recursive) {
  try {
    std::vector<std::string> files;
    DIR * dir;
    struct dirent * ent;
    if ((dir = opendir(path.c_str())) != NULL) {
      while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type == DT_REG) {
          files.push_back(path + "/" + ent->d_name);
        }
        if (recursive && ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 &&
            strcmp(ent->d_name, "..") != 0) {
          std::vector<std::string> subFiles =
              getAllFiles(path + "/" + ent->d_name, recursive);
          files.insert(files.end(), subFiles.begin(), subFiles.end());
        }
      }
      closedir(dir);
    }
    else {
      logError("Error opening directory " + path);
    }
    return files;
  }
  catch (std::exception & e) {
    logError("Error getting all files in directory " + path);
    return std::vector<std::string>();
  }
}

/**
 * @brief returns the file name from an absolute path
*/
std::string File_Util_Handler::getFileName(std::string filePath) {
  try {
    size_t lastSlash = filePath.find_last_of("/");
    // no slash - return the path
    if (lastSlash == std::string::npos) {
      return filePath;
    }
    return filePath.substr(lastSlash + 1);
  }
  catch (std::exception & e) {
    logError("Error getting file name from path " + filePath);
    return "";
  }
}

/**
 * @brief checks if a file with a given hash exists in a directory
*/
bool File_Util_Handler::fileWithHashExists(std::string path, std::string hash) {
  try {
    std::vector<std::string> files = getAllFiles(path, true);
    for (std::string file : files) {
      if (fileMatchHash(file, hash)) {
        return true;
      }
    }
    return false;
  }
  catch (std::exception & e) {
    logError("Error checking if file with hash " + hash + " exists in directory " + path);
    return false;
  }
}

/**
 * @brief checks if a file with a given file name exists in a directory
*/
bool File_Util_Handler::fileWithNameExists(std::string path, std::string name) {
  try {
    std::vector<std::string> files = getAllFiles(path, true);
    for (std::string file : files) {
      if (getFileName(file).compare(name) == 0) {
        return true;
      }
    }
    return false;
  }
  catch (std::exception & e) {
    logError("Error checking if file with name " + name + " exists in directory " + path);
    return false;
  }
}

/**
 * @brief gets the file path of a file with a given hash in a directory
 * returns the file path of the file with the given hash in the directory
 * @param path the path to the directory to search in
 * @param hash the hash to check against
*/
std::string File_Util_Handler::getFilePathFromHash(std::string path, std::string hash) {
  try {
    std::vector<std::string> files = getAllFiles(path, true);
    for (std::string file : files) {
      if (fileMatchHash(file, hash)) {
        return file;
      }
    }
    return "";
  }
  catch (std::exception & e) {
    logError("Error getting file path from hash " + hash + " in directory " + path);
    return "";
  }
}

/**
 * @brief gets the hash of a file with a given path
 * returns the hash of the file at path
 * @param path the absolute path to the file
*/
std::string File_Util_Handler::getFileHashFromPath(std::string path) {
  try {
    std::vector<std::string> files = getAllFiles(path, true);
    for (std::string file : files) {
      if (file.compare(path) == 0) {
        return hashFile(file);
      }
    }
    return "";
  }
  catch (std::exception & e) {
    logError("Error getting file hash from path " + path);
    return "";
  }
}