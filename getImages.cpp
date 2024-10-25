#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


int main(){
    // Create a VideoCapture object and use camera to capture the video

    // CAMERAS
    VideoCapture cap0(6); 
    //VideoCapture cap1(12); 
    //VideoCapture cap1(18); 

    // Check if camera opened successfully
    if(!cap0.isOpened()) {
        std::cout << "cap0 doesn't work" << std::endl;
        return -1;
    }
    // if(!cap1.isOpened()) {
    //     std::cout << "cap1 doesn't work" << std::endl;
    //     return -1;
    // }
    // if(!cap2.isOpened()) {
    //     std::cout << "cap2 doesn't work" << std::endl;
    //     return -1;
    // }

    // Default resolutions of the frame are obtained.
    int frame_width = cap0.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap0.get(cv::CAP_PROP_FRAME_HEIGHT);

    bool recording = false;
    int videono = 1;

    VideoWriter video;

    while(1)
    {

        Mat frame0;
        //Mat frame1;

        // Capture frame-by-frame
        cap0 >> frame0;
        //cap1 >> frame1;

        // If the frame is empty, break immediately
        if (frame0.empty())
            break;

        // Display the resulting frame    
        imshow( "Frame 0", frame0 );
        //imshow( "Frame 1", frame1 );

        char c = (char)waitKey(1);
        if( c == 27 )  // Press  ESC on keyboard to  exit
            break;
        /*
        if( c == 115 and !recording)    // Press  s on keyboard to start recording
        {
            char path[100];
            sprintf(path, "%d.avi", videono);
            std::cout << "recording started for " << path << "\n";
            videono += 1;
            video.open(path, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
            recording = true;
        }       

        if( recording )
            video.write(frame);

        if( c == 'x')    // Press x on keyboard to stop recording
        {
            std::cout << "recording finished.\n";
            recording = false;
            video.release();
        }*/
    }
    cap0.release();// release the video capture and write object
    //cap1.release();
    destroyAllWindows();  // Closes all the frames
    return 0;
}
