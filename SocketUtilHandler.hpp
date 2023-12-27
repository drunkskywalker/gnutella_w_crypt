#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>

#include "Logger.hpp"
class Socket_Util_Handler {
  Logger * logger;

 public:
  Socket_Util_Handler(Logger * logger) : logger(logger) {}

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
 * @brief establish server socket, listen for incoming connections
 * returns a socket file descriptor if successful, -1 otherwise
 * @param port the port to listen on
 */
  int initServerSocket(const char * port);

  /**
   * @brief establish connection to server
   * returns a socket file descriptor if successful, -1 otherwise
  */
  int initClientSocket(const char * hostname, const char * port);

  /**
   * @brief handle incoming connection
   * returns a socket file descriptor if successful, -1 otherwise
  */
  int handleClientSocket(int socket_fd);

  /**
 * @brief send message to fd
 * returns 0 if successful, -1 otherwise
 * @param fd the file descriptor to send the message to
 * @param message the message to send
 * @param length the length of the message
 * @param type the type of the message
*/
  int sendMessage(int fd, const char * message, int length, int type);

  /**
 * @brief receive message from fd
 * returns 0 if successful, -1 otherwise
 * @param fd the file descriptor to receive the message from
 * @param message the message to receive
 * @param length will be set to the length of the message
 * @param type will be set to the type of the message
*/
  int recvMessage(int fd, char * message, int * length, int * type);
};