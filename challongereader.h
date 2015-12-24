#ifndef CHALLONGEREADER_H_
#define CHALLONGEREADER_H_
#include <iostream>
#include <fstream>
#include <string>


#define DEFAULT_API_KEY_PATH  "./apiKey.txt"



class ChallongeReader {
 public:
  ChallongeReader();
  ~ChallongeReader();

 private:
  std::string apiKey;

  /**
   * @brief sets the apiKey to what was in the file given
   *
   * @param fileName path to apiKey.text
   */
  void setApiKey(std::string fileName);
};

#endif  // CHALLONGEREADER_H_
