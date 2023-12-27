#include "Node.hpp"

/**
 * @brief query identifier -> string
 * returns a string representation of a query identifier to be used as keys
 * 
*/
std::string Node::getQueryIdentifierString(Query_Identifier id) {
  std::stringstream ss;
  ss << id.source.hostName << ":" << id.timestamp << ":" << id.hash;
  return ss.str();
}

/**
 * @brief sends a ping to a peer
 * returns 0 successful, -1 otherwise failed
*/
int Node::sendPing(Peer_Identifier peer, Ping ping, int fd) {
  try {
    if (socketUtilHandler.sendMessage(fd, (char *)&ping, sizeof(ping), T_PING) < 0) {
      logger->logError("Error sending ping to " + std::string(peer.hostName));
      return -1;
    }
    else {
      logger->logEvent("Sent ping to " + std::string(peer.hostName));
      return 0;
    }
  }
  catch (std::exception & e) {
    logger->logError("Error sending ping to " + std::string(peer.hostName) + ": " +
                     std::string(e.what()));
    return -1;
  }
}

/**
 * @brief sends a pong to a peer
 * returns 0 if node allowed to add ping sender as a peer, 1 if not, -1 otherwise failed
*/
int Node::handlePing(Ping ping, int fd) {
  try {
    std::lock_guard<std::mutex> lock(peersMutex);
    if (peers.size() >= maxPeers) {
      Pong pong;
      pong.allowed = false;
      pong.timestamp = time(NULL);
      pong.num_peers = peers.size();
      int i = 0;
      for (std::map<std::string, Peer_Info>::iterator it = peers.begin();
           it != peers.end();
           ++it) {
        pong.peers[i] = it->second.id;
        i++;
      }
      sendPong(pong, fd);
      return 1;
    }
  }
  catch (std::exception & e) {
    logger->logError("Error handling ping: " + std::string(e.what()));
    return -1;
  }
}
