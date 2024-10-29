#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;


int main(){
    // Create a VideoCapture object and use camera to capture the video

    // CAMERAS
    VideoCapture cap0(2); 
    VideoCapture cap1(5); 
    VideoCapture cap2(8); 

    // Check if camera opened successfully
    if(!cap0.isOpened()) {
        std::cout << "cap0 doesn't work" << std::endl;
        return -1;
    }
    if(!cap1.isOpened()) {
        std::cout << "cap1 doesn't work" << std::endl;
        return -1;
    }
    if(!cap2.isOpened()) {
        std::cout << "cap2 doesn't work" << std::endl;
        return -1;
    }

    // Default resolutions of the frame are obtained.
    int frame_width = cap0.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap0.get(cv::CAP_PROP_FRAME_HEIGHT);

    bool recording = false;
    int videono = 1;

    VideoWriter video0;
    VideoWriter video1;
    VideoWriter video2;

    while(1)
    {

        Mat frame0;
        Mat frame1;
        Mat frame2;

        // Capture frame-by-frame
        cap0 >> frame0;
        cap1 >> frame1;
        cap2 >> frame2;

        // If the frame is empty, break immediately
        if (frame0.empty())
            break;

        // Display the resulting frame    
        imshow( "Frame 0", frame0 );
        imshow( "Frame 1", frame1 );
        imshow( "Frame 2", frame2 );

        char c = (char)waitKey(1);
        if( c == 27 )  // Press  ESC on keyboard to  exit
            break;
        
        if( c == 115 and !recording)    // Press  s on keyboard to start recording
        {
            char path0[100];
            sprintf(path0, "video%d.avi", videono);
            videono += 1;
            video0.open(path0, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
            
            char path1[100];
            sprintf(path1, "video%d.avi", videono);
            videono += 1;
            video1.open(path1, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
            
            char path2[100];
            sprintf(path2, "video%d.avi", videono);
            videono += 1;
            video2.open(path2, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
            
            recording = true;
        }
            
        if( recording )
        {
            video0.write(frame0);
            video1.write(frame1);
            video2.write(frame2);
        }

        if( c == 'x')    // Press x on keyboard to stop recording
        {
            std::cout << "recording finished.\n";
            recording = false;
            video0.release();
            video1.release();
            video2.release();
        }
        
    }
    cap0.release();// release the video capture and write object
    cap1.release();
    cap2.release();
    destroyAllWindows();  // Closes all the frames
    return 0;
}
