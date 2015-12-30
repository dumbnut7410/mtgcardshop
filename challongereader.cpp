#include "./challongereader.h"

ChallongeReader::ChallongeReader() {
  this->setApiKey(DEFAULT_API_KEY_PATH);
}

ChallongeReader::~ChallongeReader() {
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::string ChallongeReader::downloadFile(std::string url) {
  CURL *curl;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  return readBuffer;
}

std::map<int, std::string> ChallongeReader::getParticipants(
  std::string eventName) {
  rapidjson::Document d;
  std::map<int, std::string> participants;
  std::string url = "https://" + this->apiKey +
                    "@api.challonge.com/v1/tournaments/" + eventName + "/participants.json";
  std::string json = downloadFile(url);
  d.Parse(json.c_str());

  for (int i = 0; i < d.Size(); i++) {

    participants[d[i]["participant"]["id"].GetInt()] =
      d[i]["participant"]["display_name"].GetString();
  }
  return participants;
}

Match* ChallongeReader::getEventMatches(std::string eventName, int *matchSize) {
  rapidjson::Document d;
  std::string url = "https://" + this->apiKey +
                    "@api.challonge.com/v1/tournaments/" + eventName + "/matches.json";
  std::string tourneyJson = downloadFile(url);

  d.Parse(tourneyJson.c_str());

  Match *matches = (Match*) malloc(sizeof(Match) * d.Size());
  *matchSize = d.Size();

  for (int i = 0; i < d.Size(); i++) {
    Match tmp;
    tmp.player1 = d[i]["match"]["player1_id"].GetInt();
    tmp.player2 = d[i]["match"]["player2_id"].GetInt();

    if (d[i]["match"]["player1_id"].GetInt() ==
        d[i]["match"]["winner_id"].GetInt()) {
      tmp.winner = 1;
    } else if (d[i]["match"]["player2_id"].GetInt() ==
               d[i]["match"]["winner_id"].GetInt()) {
      tmp.winner = 2;
    } else {
      tmp.winner = 0;
    }

    *(matches + i) = tmp;
  }
  return matches;
}


void ChallongeReader::setApiKey(std::string filename) {
  std::string lol;
  std::ifstream file(filename);
  if (file.is_open()) {
    getline(file, lol);
    this->apiKey = lol;
  } else {
    std::cout << "invalid api key file. paste api key into " <<
              filename << std::endl;
    return;
  }
}
