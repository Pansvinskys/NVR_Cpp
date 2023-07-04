#include <iostream>
#include <thread>
#include <atomic>
#include <ncurses.h>
#include "stdio.h"
#include <vector>
#include <fstream>
#include <regex>
#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

class NVR{
    public:
        vector<Mat> frames;
        vector<VideoCapture> caps;
        vector<VideoCapture> writers;
        vector<string> files;
};

class YAMLFormat{
    public:
        regex key{"^\\w+:$", regex_constants::ECMAScript};
        regex value{"^\\s+.*$", regex_constants::ECMAScript};

        YAMLFormat(){}

        vector<string> getLineType(string line){
            if(regex_search(line, this->key)){
                return vector<string>{"key", line};
            }
            else if(regex_search(line, this->value)){
                return vector<string>{"value", line};
            }
            else{
                return vector<string>{"error", line};
            }
        }
};

bool setupVideoCapture(VideoCapture &cap){
    int APIID = cv::CAP_ANY;
    cap.open("rtsp://pansvinsky:Jagmaster1Tapo@192.168.1.133:554/stream2", APIID);
    if(!cap.isOpened()){
        cerr << "Unable to open de camera\n";
        return false;
    }
    return true;
}

bool setupWriter(string filename, VideoWriter &writer, Mat &frame){
    double fps = 15.0; 
    int codec = VideoWriter::fourcc('M', 'J', 'P', 'G');
    int deviceID = 0;
    bool isColor = (frame.type() == CV_8UC3);
    writer.open(filename, codec, fps, frame.size(), isColor);
    if (!writer.isOpened()) {
        cerr << "Could not open the output video file for write\n";
        return false;
    }
    return true;
}

bool setupCameras(Mat &frame, VideoCapture &cap, VideoWriter &writer, string filename){
    if(!setupVideoCapture(cap))
        return false;

    cap >> frame;

    if(!setupWriter(filename, writer, frame))
        return false;
    return true;
}

void videoProcessing(VideoCapture &cap, VideoWriter &writer, Mat &frame, int &stop){
    printw("streaming started");
    printw("press q to exit");
    refresh();
    for(;;){
        if(stop)
            break;
        cap.read(frame);
        if(frame.empty()){
            cerr << "the frame was empty\n";
            break;
        }
        writer.write(frame);
        if (waitKey(1) >= 0)
            break;
    }
}

void getUserInputs(int &stop){
    while(true){
        if(getch() == 'q'){
            stop = 1;
            endwin();
            break;
        }
    }
}

template <class T>
void getAppConfig(string cfg_path, NVR &nvr, vector<T> &prop = vector<int>{}){
    ifstream cfg_file(cfg_path, ifstream::in);
    string line;
    YAMLFormat yamlf = YAMLFormat();
    
    while(getline(cfg_file, line)){
        vector<string> format = yamlf.getLineType(line);
        if(format[0] == "key"){
            
        }else if(format[0] == "value"){

        }
        else{

        }
    }
}

bool checkTerminalCMDs(int argc, char** argv){
    if(argc == 1){
        cout << "Usage:" << endl;
        cout << "   nvr <cfg_file_path>" << endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv){
    NVR nvr;
    vector<int> x{};

    if(!checkTerminalCMDs(argc, argv)){
        return -1;
    }

    getAppConfig(argv[1], nvr);
    
    Mat frame;
    VideoCapture cap;
    VideoWriter writer;
    string filename = "./live.avi";

    if(!setupCameras(frame, cap, writer, filename))
        return -1;

    int stop = 0;

    initscr();

    thread videoTh(videoProcessing, ref(cap), ref(writer), ref(frame), ref(stop));
    thread userInputs(getUserInputs, ref(stop));
    
    videoTh.join();
    userInputs.join();

    return 0;
}

