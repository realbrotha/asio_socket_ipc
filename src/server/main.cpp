//#include "UnixDomainSocketInterface.h"

#include "MessageDefine.hpp"

#include <array>
#include <vector>

void ReadbackPtr(std::vector<char> message) {

}
void ConnectCallbackPtr(std::vector<char> message) {
  printf ("connect recv [%s]\n",message.data());
}
void DisConnectCallbackPtr(std::vector<char> message) {
  printf ("disconnect recv [%s]\n",message.data());
}

int main(int argc, char *argv[]) {

}
