#include "./lib/reqRep.h"
#include "./lib/audioStream.h"

#define PORT_SVC 5000
#define INADDR_SVC "127.0.0.1"

int main(){
    streamAudioServer("./playlist/Radiohead_Creep.mp3");

}