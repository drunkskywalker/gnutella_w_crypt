#include <nlohmann/json.hpp>

#include "Node.hpp"

nlohmann::json readConfig(std::string configPath) {
  try {
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
      std::cout
          << "====================\nError Opening Config File\n====================\n";
      return nullptr;
    }
    nlohmann::json config;
    configFile >> config;
    configFile.close();
    return config;
  }
  catch (std::exception & e) {
    std::cout << "====================\nError Parsing Config File\n"
              << e.what() << "\n====================\n";
    return nullptr;
  }
}

int main(int argc, char * argv[]) {
  std::string configPath = "config.json";
  if (argc == 2) {
    std::string configPath = argv[1];
  }
  else if (argc > 2) {
    std::cout << "Usage: ./run [configPath]\n";
    return 0;
  }

  std::cout << "========================================================================="
               "==========================="
               "=====================================================\n";
  std::cout
      << "   _____ _   _ _    _ _______ ______ _      _                          _ _   _ "
         "       _____                  _                              _           \n";
  std::cout
      << "  / ____| \\ | | |  | |__   __|  ____| |    | |        /\\              (_) | "
         "| |      / ____|                | |                            | |          \n";
  std::cout << " | |  __|  \\| | |  | |  | |  | |__  | |    | |       /  \\    __      "
               "___| |_| |__   | |     _ __ _   _ _ __ | |_ ___   __ _ _ __ __ _ _ __ | "
               "|__  _   _ \n";
  std::cout << " | | |_ | . ` | |  | |  | |  |  __| | |    | |      / /\\ \\   \\ \\ /\\ "
               "/ / | __| '_ \\  | |    | '__| | | | '_ \\| __/ _ \\ / _` | '__/ _` | '_ "
               "\\| '_ \\| | | |\n";
  std::cout << " | |__| | |\\  | |__| |  | |  | |____| |____| |____ / ____ \\   \\ V  V "
               "/| | |_| | | | | |____| |  | |_| | |_) | || (_) | (_| | | | (_| | |_) | "
               "| | | |_| |\n";
  std::cout << "  \\_____|_| \\_|\\____/   |_|  |______|______|______/_/    \\_\\   "
               "\\_/\\_/ |_|\\__|_| |_|  \\_____|_|   \\__, | .__/ \\__\\___/ \\__, |_|  "
               "\\__,_| .__/|_| |_|\\__, |\n";
  std::cout
      << "                                                                               "
         "                   __/ | |              __/ |         | |           __/ |\n";
  std::cout
      << "                                                                               "
         "                  |___/|_|             |___/          |_|          |___/\n";
  std::cout << "========================================================================="
               "==========================="
               "=====================================================\n";
  nlohmann::json config = readConfig(configPath);
  if (config == nullptr) {
    return 0;
  }
  Logger logger;

  try {
    std::string logFilePath = config["logFilePath"];
    std::string fileDirectory = config["fileDirectory"];
    int maxPeers = config["maxPeers"];
    int maxInitPeers = config["maxInitPeers"];
    unsigned short int messagePort = config["messagePort"];
    unsigned short int filePort = config["filePort"];
    unsigned short int userPort = config["userPort"];
    int queryTimeToLive = config["queryTimeToLive"];
    int cacheTimeToCheck = config["cacheTimeToCheck"];
    int chacheTimeToLive = config["cacheTimeToLive"];
    std::vector<Peer_Identifier> peers;
    for (nlohmann::json peer : config["famousNodes"]) {
      Peer_Identifier peerIdentifier;
      char * hostName = new char[256];
      strcpy(hostName, peer["hostName"].get<std::string>().c_str());
      unsigned short int port = peer["port"];
      char * id = new char[16];
      strcpy(id, peer["id"].get<std::string>().c_str());

      strcpy(peerIdentifier.hostName, hostName);
      peerIdentifier.port = port;
      strcpy(peerIdentifier.id, id);
      peers.push_back(peerIdentifier);
    }

    Logger logger(logFilePath);
    logger.init();
    Node node(&logger,
              fileDirectory,
              maxPeers,
              maxInitPeers,
              messagePort,
              filePort,
              userPort,
              queryTimeToLive,
              cacheTimeToCheck,
              chacheTimeToLive,
              peers);
    try {
      node.init();
      node.run();
    }
    catch (std::exception & e) {
      std::cout << "====================\nError Running Gnutella node\n"
                << e.what() << "\n====================\n";
      return 1;
    }
  }
  catch (std::exception & e) {
    std::cout << "====================\nError Initializing Gnutella node\n"
              << e.what() << "\n====================\n";
    return 1;
  }
}