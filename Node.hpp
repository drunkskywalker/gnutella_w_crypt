#include <map>
#include <shared_mutex>
#include <string>

#include "FileUtilHandler.hpp"
#include "Protocol.hpp"
#include "SocketUtilHandler.hpp"

class Node {
  Logger * logger;                           //
  File_Util_Handler fileUtilHandler;         //
  Socket_Util_Handler socketUtilHandler;     //
  Peer_Identifier selfInfo;                  // info of this node
                                             //
  int maxPeers;                              // maximum number of total peers
  int maxInitPeers;                          // maximum number of initial peers
  unsigned short int messagePort;            // port to listen for messages
  unsigned short int filePort;               // port to listen for file transfers
  unsigned short int userPort;               // port to listen for user commands
  int queryTimeToLive;                       // ttl of queries initiated by this node
  int cacheTimeToCheck;                      // time to check cache
  int chacheTimeToLive;                      // time to live of cache entries
  std::vector<Peer_Identifier> famousPeers;  // a vector of peers
  std::map<std::string,                      //
           Peer_Info>                        //
      peers;                                 // hostname-> peer info (peer id, fd)
  std::map<std::string,                      //
           Query>                            //
      queries;                               // string(query id)-> query
  std::map<std::string,                      //
           Query_Status>                     //
      queryStatuses;                         // hash -> query status
  std::map<std::string,                      //
           std::string>                      //
      filePaths;                             // hash -> file path
                                             //
  std::mutex peersMutex;                     // mutex for peers map
  std::mutex queriesMutex;                   // mutex for queries map
  std::mutex queryStatusesMutex;             // mutex for query statuses map
  std::mutex filePathsMutex;                 // mutex for file paths map

 public:
  Node(Logger * logger,
       std::string filePath,
       int maxPeers,
       int maxInitPeers,
       unsigned short int messagePort,
       unsigned short int filePort,
       unsigned short int userPort,
       int queryTimeToLive,
       int cacheTimeToCheck,
       int chacheTimeToLive,
       std::vector<Peer_Identifier> famousPeers) :
      logger(logger),
      fileUtilHandler(logger, filePath),
      socketUtilHandler(logger),
      maxPeers(maxPeers),
      maxInitPeers(maxInitPeers),
      messagePort(messagePort),
      filePort(filePort),
      userPort(userPort),
      queryTimeToLive(queryTimeToLive),
      cacheTimeToCheck(cacheTimeToCheck),
      chacheTimeToLive(chacheTimeToLive),
      famousPeers(famousPeers),
      queries(),
      queryStatuses(),
      filePaths(),
      peersMutex(),
      queriesMutex(),
      queryStatusesMutex(),
      filePathsMutex(){};

  /**
 * @brief query identifier -> string
 * returns a string representation of a query identifier to be used as keys
 * 
*/
  std::string getQueryIdentifierString(Query_Identifier id);

  /**
 * @brief sends a ping to a peer
 * returns 0 successful, -1 otherwise failed
*/
  int sendPing(Peer_Identifier peer, Ping ping, int fd);

  int sendPong(Pong pong, int fd);

  /**
 * @brief sends a pong to a peer
 * returns 0 if node allowed to add ping sender as a peer, 1 if not, -1 otherwise failed
*/
  int handlePing(Ping ping, int fd);

  /**
   * @brief handles a pong from a peer
   * returns 0 if allowed to add peer, 1 if not, -1 otherwise failed
*/
  int handlePong(Pong pong);

  /**
 * @brief joins the network
 * tries to connect to peers and connect to maximum of maxInitPeers peers.
 * returns 0 if successful, -1 otherwise failed
 * @param peers a vector of peers to connect to
*/
  int joinNetwork(std::vector<Peer_Identifier> & peers);

  /**
 * @brief generates and sends a query to all peers
 * returns a query if successful, nullptr otherwise failed
 * @param query the query to send
*/
  Query initQuery(std::string hash);

  /**
 * @brief sends a query to a peer
 * returns 0 if successful, -1 otherwise failed
 * @param query the query to send
 * @param fd the file descriptor of the peer to send the query to
*/
  int sendQuery(Query query, int fd);

  /**
 * @brief sends a query hit back
 * returns 0 if successful, -1 otherwise failed
 * @param query the query to send
 * @param fd the file descriptor of the peer to send the query hit to
*/
  int sendQueryHit(Query query, int fd);

  /**
 * @brief handles a query from a peer
 * returns 0 if has the file, 1 if not, -1 otherwise failed 
 * if has the file, sends a query hit back along the path
 * if doesn't have the file, sends the query to all peers except the previous peer
 * cache the query accordingly
 * @param fd the file descriptor that received the query
*/
  int handleQuery(int fd);

  /**
 * @brief handles a query hit from a peer
 * returns 0 if successful, -1 otherwise failed
 * if the query hit was sent by the node, send a file request
 * otherwise, send the query back
 * @param fd the file descriptor that received the query hit
*/
  int handleQueryHit(int fd);

  /**
 * sends query identifier to the file holder
 * returns 0 if successful, -1 otherwise failed
 * @param queryHit the query hit contains file owner address and the query identifier to request
*/
  int initFileRequest(Query_Hit queryHit);

  /**
   * @brief handles a file request from a peer
   * returns 0 if successful, -1 otherwise failed
   * @param fd the file descriptor that received the file request
  */
  int handleFileRequest(int fd);

  int messageThread();

  int fileThread();

  int userThread();

  //TODO

  /**
 * @brief sends a name search to all peers
 * returns 0 if successful, -1 otherwise failed
 * @param fileName the name of the file to search for
 * returns -1 if filename too long as well
*/
  int sendSearch(std::string fileName);

  /**
 * @brief initializes the node
 * necessary initialization steps of the node
 * 
*/
  void init();

  /**
   * @brief runs the node
  */
  void run();
};