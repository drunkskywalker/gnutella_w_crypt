#include "SocketUtilHandler.hpp"

/**
 * @brief Log error
 * logs an error. returns 0 if log was successful, -1 otherwise
 * @param msg the message to log
 * @param logger the logger object to do the logging
*/
int Socket_Util_Handler::logError(std::string msg) {
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
int Socket_Util_Handler::logEvent(std::string msg) {
  if (logger->logEvent(msg)) {
    return 0;
  }
  return -1;
}

/**
 * @brief establish server socket, listen for incoming connections
 * returns a socket file descriptor if successful, -1 otherwise
 * @param port the port to listen on
 */
int Socket_Util_Handler::initServerSocket(const char * port) {
  const char * hostname = NULL;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;
  int status, socket_fd;

  memset(&host_info, 0, sizeof(host_info));

  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;
  host_info.ai_flags = AI_PASSIVE;

  if (getaddrinfo(hostname, port, &host_info, &host_info_list) < 0) {
    logError("Error getting address info for host");
  }

  //generate a port
  if (std::string(port) == "") {
    ((struct sockaddr_in *)host_info_list->ai_addr)->sin_port = 0;
  }

  socket_fd = socket(host_info_list->ai_family,
                     host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd < 0) {
    logError("Error creating socket");
  }

  int yes = 1;
  if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) {
    logError("Error setting socket options");
  }

  if (bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) < 0) {
    logError("Error binding socket");
  }

  if (listen(socket_fd, 100) < 0) {
    logError("Error listening on socket");
  }

  freeaddrinfo(host_info_list);
  logEvent("Listening on port " + std::string(port));
  return socket_fd;
}

/**
   * @brief establish connection to server
   * returns a socket file descriptor if successful, -1 otherwise
  */
int Socket_Util_Handler::initClientSocket(const char * hostname, const char * port) {
  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo * host_info_list;

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family = AF_INET;
  host_info.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(hostname, port, &host_info, &host_info_list) < 0) {
    logError("Error getting address info for host");
  }

  socket_fd = socket(host_info_list->ai_family,
                     host_info_list->ai_socktype,
                     host_info_list->ai_protocol);
  if (socket_fd < 0) {
    logError("Error creating socket");
  }

  if (connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen) < 0) {
    logError("Error connecting to socket");
  }

  freeaddrinfo(host_info_list);
  logEvent("Connected to " + std::string(hostname) + ":" + std::string(port));
  return socket_fd;
}

/**
   * @brief handle incoming connection
   * returns a socket file descriptor if successful, -1 otherwise
  */
int Socket_Util_Handler::handleClientSocket(int socket_fd) {
  struct sockaddr_storage socket_addr;
  socklen_t socket_addr_len = sizeof(socket_addr);
  int client_fd = accept(socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
  if (client_fd < 0) {
    logError("Error accepting connection");
  }
  logEvent("Accepted connection from client");
  return client_fd;
}

/**
 * @brief send message to fd
 * returns 0 if successful, -1 otherwise
 * @param fd the file descriptor to send the message to
 * @param message the message to send
 * @param length the length of the message
*/
int Socket_Util_Handler::sendMessage(int fd, const char * message, int length, int type) {
  if (send(fd, &type, sizeof(int), 0) < 0) {
    logError("Error sending message type");
  }
  if (send(fd, &length, sizeof(int), 0) < 0) {
    logError("Error sending message length");
  }
  int bytes_sent = send(fd, message, length, 0);
  if (bytes_sent != length) {
    logError("Error sending message");
  }
  logEvent("sent " + std::to_string(bytes_sent) + " bytes to fd " + std::to_string(fd));
  return bytes_sent;
}

/**
 * @brief receive message from fd
 * returns 0 if successful, -1 otherwise
 * @param fd the file descriptor to receive the message from
 * @param message the message to receive
 * @param length will be set to the length of the message
 * @param type will be set to the type of the message
*/
int Socket_Util_Handler::recvMessage(int fd, char * message, int * length, int * type) {
  if (recv(fd, type, sizeof(int), 0) < 0) {
    logError("Error receiving message type");
  }
  if (recv(fd, length, sizeof(int), 0) < 0) {
    logError("Error receiving message length");
  }
  int bytes_received = recv(fd, message, *length, 0);
  if (bytes_received != *length) {
    logError("Error receiving message");
  }
  logEvent("received " + std::to_string(bytes_received) + " bytes from fd " +
           std::to_string(fd));
  return bytes_received;
}