#ifndef CHALLONGEREADER_H_
#define CHALLONGEREADER_H_
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "./curl/curl.h"
#include "rapidjson/document.h"

#define DEFAULT_API_KEY_PATH  "./apiKey.txt"

typedef struct {
  int player1, player2;
  int winner;
} Match;

class ChallongeReader {
 public:
  ChallongeReader();
  ~ChallongeReader();

  /**
   * @brief gets all match info from challonge
   * @details uses information from https://api.challonge.com/v1/tournaments/
   *
   * @param eventName name of event on url (https://api.challonge.com/v1/tournaments/<tourney name>/matches.json)
   * @return array of matches in cronological order for given event empty if failure
   *
   */
  Match* getEventMatches(std::string eventName, int *matchSize);

  /**
   * @brief gets map of participants and their id's
   * @details downloads the participants.json and stores info in a map
   *
   * @param eventName name of event on url (https://api.challonge.com/v1/tournaments/<tourney name>/participants.json)
   * @return map of player names and their id's
   */
  std::map<int, std::string> getParticipants(std::string eventName);
 private:
  std::string apiKey;
  /**
   * @brief gets the file from the interwebs
   * @param url address of file
   * @return plaintext of file
   */
  std::string downloadFile(std::string url);


  /**
   * @brief sets the apiKey to what was in the file given
   *
   * @param fileName path to apiKey.text
   */
  void setApiKey(std::string fileName);
};

#endif  // CHALLONGEREADER_H_
