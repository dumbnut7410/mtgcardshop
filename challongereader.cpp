#include "./challongereader.h"

ChallongeReader::ChallongeReader() {
  this->setApiKey(DEFAULT_API_KEY_PATH);
  std::cout << "api key: " << this->apiKey << std::endl;
}

ChallongeReader::~ChallongeReader() {

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
