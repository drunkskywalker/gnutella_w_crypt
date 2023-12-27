#include <dirent.h>
#include <openssl/conf.h>
#include <openssl/crypto.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <vector>

#include "Logger.hpp"

class File_Util_Handler {
  Logger * logger;
  std::string fileDirectory;

 public:
  File_Util_Handler(Logger * logger, std::string fileDirectory) :
      logger(logger), fileDirectory(fileDirectory) {}

  /**
 * @brief Log error
 * logs an error. returns 0 if log was successful, -1 otherwise
 * @param msg the message to log
 * @param logger the logger object to do the logging
*/
  int logError(std::string msg);

  /**
 * @brief Log event
 * logs an event. returns 0 if log was successful, -1 otherwise
 * @param msg the message to log
 * @param logger the logger object to do the logging
*/
  int logEvent(std::string msg);

  /**
 * @brief sha256 hashes message
 * stores the hash of message in digest. returns 0 if successful, -1 otherwise
 * @param message the message to digest
 * @param message_len the length of the message
 * @param digest the digest of the message
 * @param digest_len the length of the digest
*/
  int digest_message(const unsigned char * message,
                     size_t message_len,
                     unsigned char ** digest,
                     unsigned int * digest_len);

  /**
 * @brief checks if input string is a valid sha256 hash in hex
 * returns true if hash is valid, false otherwise
 * @param hash the hash to check
*/
  bool isValidHash(std::string hash);

  /**
 * @brief hashes a file with given absolute file path.
 * returns the hash of the file at filePath
 * @param filePath the absolute path to the file
*/
  std::string hashFile(std::string filePath);

  /**
 * @brief hashes a char array with given size.
 * returns the hash of the char array
 * @param data the char array to hash
 * @param size the size of the char array
*/
  std::string hashCharArray(char * data, size_t size);

  /**
 * @brief checks if a file at filePath matches a given hash
 * returns true if file matches hash, false otherwise
 * @param filePath the absolute path to the file
 * @param hash the hash to check against
*/
  bool fileMatchHash(std::string filePath, std::string hash);

  /**
 * @brief gets the size of a file at filePath
 * returns the size of the file at filePath
 * @param filePath the absolute path to the file
*/
  bool charArrayMatchHash(char * data, size_t size, std::string hash);

  /**
 * @brief gets the size of a file at filePath
 * returns the size of the file at filePath
 * @param filePath the absolute path to the file
*/
  size_t getFileSize(std::string filePath);

  /**
 * @brief gets all files a directory.
 * returns a vector of all files in a directory
 * @param path the path to the directory
 * @param recursive whether to get files in subdirectories
*/
  std::vector<std::string> getAllFiles(std::string path, bool recursive);

  /**
 * @brief returns the file name from an absolute path
*/
  std::string getFileName(std::string filePath);

  /**
 * @brief checks if a file with a given hash exists in a directory
*/
  bool fileWithHashExists(std::string path, std::string hash);

  /**
 * @brief checks if a file with a given file name exists in a directory
*/
  bool fileWithNameExists(std::string path, std::string name);

  /**
 * @brief gets the file path of a file with a given hash in a directory
 * returns the file path of the file with the given hash in the directory
 * @param path the path to the directory to search in
 * @param hash the hash to check against
*/
  std::string getFilePathFromHash(std::string path, std::string hash);

  /**
 * @brief gets the hash of a file with a given path
 * returns the hash of the file at path
 * @param path the absolute path to the file
*/
  std::string getFileHashFromPath(std::string path);
};