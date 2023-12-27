#include <cstddef>
#include <cstdint>

#define T_PEER_IDENTIFIER 100
#define T_PEER_INFO 101
#define T_PING 200
#define T_PONG 201
#define T_SPLASH 202
#define T_QUERY_IDENTIFIER 300
#define T_QUERY 301
#define T_QUERY_HIT 302
#define T_QUERY_STATUS 303
#define T_FILE_META 400
#define T_NAME_SEARCH 500
#define T_SEARCH_MATCH_IDENTIFIER 501
#define T_NAME_SEARCH_HIT 502
#define T_SECURE_CHECK 600

// message used to identify a peer
// 100
struct Peer_Identifier_t {
  char hostName[256];   //
  unsigned short port;  //
  char id[16];          //
};
typedef struct Peer_Identifier_t Peer_Identifier;

// 101
struct Peer_Info_t {
  Peer_Identifier id;  //
  int fd;              // file descriptor of the peer
};
typedef struct Peer_Info_t Peer_Info;

// 200
// message used to connect to a network
struct Ping_t {
  Peer_Identifier selfInfo;  // the info of the sender
  unsigned int timestamp;    //
};
typedef struct Ping_t Ping;

// 201
// message used to respond to a ping
struct Pong_t {
  bool allowed;               // is the sender allowed to add receiver as a peer
  unsigned int timestamp;     //
  int num_peers;              //
  Peer_Identifier peers[10];  // max 10 additional peers known to the receiver
};
typedef struct Pong_t Pong;

// 202
// message for disconnecting from network
struct Splash_t {
  unsigned int timestamp;  //
};
typedef struct Splash_t Splash;

// 300
// message used to identify a query
struct Query_Identifier_t {
  Peer_Identifier source;  // initiator of query
  unsigned char hash[32];  // hash of the file being queried
  unsigned int timestamp;  //
};
typedef struct Query_Identifier_t Query_Identifier;

// 301
// message used to query a file
struct Query_t {
  Query_Identifier id;   //
  Peer_Identifier prev;  // previous peer in the query path
  int ttl;               // time to live
};
typedef struct Query_t Query;

// 302
// message used to respond to a query
struct Query_Hit_t {
  Query_Identifier id;          //
  Peer_Identifier prev;         // previous peer in the query path
  Peer_Identifier destination;  // owner of file being queried
};
typedef struct Query_Hit_t Query_Hit;

// 303
// message used to identify if a query has been successfully handled
struct Query_Status_t {
  bool success;   // was the query responded with corresponding file
  int timestamp;  //
};
typedef struct Query_Status_t Query_Status;

// 400
// message used to identify a file transfer
struct File_Meta_t {
  unsigned char hash[32];  // hash of the file
  char name[256];          // name of the file
  bool available;          // is the file available for download
  size_t fileSize;         // size of the file
  char iv[16];             // initialization vector for encryption
  char tag[16];            // tag for encryption
};
typedef struct File_Meta_t File_Meta;

// 500
// message used to search for a file by name
struct Name_Search_t {
  Peer_Identifier source;  // initiator of search
  char name[256];          // name of the file
  unsigned int timestamp;  //
};
typedef struct Name_Search_t Name_Search;

// 501
// message used to identify a file search match
struct Search_Match_Identifier_t {
  char name[256];
  unsigned char hash[32];
};
typedef struct Search_Match_Identifier_t Search_Match_Identifier;

// 502
// message used to respond to a name search
struct Name_Search_Hit_t {
  Search_Match_Identifier matchId;  // identifier of the search match
  Peer_Identifier source;           // initiator of search
  Peer_Identifier destination;      // owner of file being searched
  unsigned int timestamp;           //
};
typedef struct Name_Search_Hit_t Name_Search_Hit;

// 600
// message used to inform transmission security settings
struct Secure_Check_t {
  unsigned short type;  //
  bool secure;          // sender allows secure transmissionz
};
typedef struct Secure_Check_t Secure_Check;
