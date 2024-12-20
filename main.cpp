#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/hdl_grabber.h>
#include <pcl/visualization/point_cloud_color_handlers.h>
#include <pcl/visualization/cloud_viewer.h>
#include <pcl/console/parse.h>

#include <boost/lexical_cast.hpp>

using namespace std::chrono_literals;
using namespace pcl::console;
using namespace pcl::visualization;

using namespace std;
using namespace cv;

class SimpleHDLViewer
{
  public:
    typedef pcl::PointCloud<pcl::PointXYZI> Cloud;
    typedef Cloud::ConstPtr CloudConstPtr;

    SimpleHDLViewer (pcl::Grabber& grabber,
        pcl::visualization::PointCloudColorHandler<pcl::PointXYZI> &handler) :
        cloud_viewer_ (new pcl::visualization::PCLVisualizer ("PCL HDL Cloud")),
        grabber_ (grabber),
        handler_ (handler)

    {
    }

    void cloud_callback (const CloudConstPtr& cloud)
    {
        std::lock_guard<std::mutex> lock (cloud_mutex_);
        cloud_ = cloud;
    }

    void run ()
    {
        cloud_viewer_->addCoordinateSystem (3.0);
        cloud_viewer_->setBackgroundColor (0, 0, 0);
        cloud_viewer_->initCameraParameters ();
        cloud_viewer_->setCameraPosition (0.0, 0.0, 30.0, 0.0, 1.0, 0.0, 0);
        cloud_viewer_->setCameraClipDistances (0.0, 50.0);

        std::function<void (const CloudConstPtr&)> cloud_cb =
            [this] (const CloudConstPtr& cloud) { cloud_callback (cloud); };
        boost::signals2::connection cloud_connection = grabber_.registerCallback (
            cloud_cb);

        grabber_.start ();

        // CAMERA
        VideoCapture cap0(3);
        VideoCapture cap1(6); 
        VideoCapture cap2(9); 

        if(!cap0.isOpened()) {
            std::cout << "cap0 doesn't work" << std::endl;
        }
        if(!cap1.isOpened()) {
            std::cout << "cap1 doesn't work" << std::endl;
        }
        if(!cap2.isOpened()) {
            std::cout << "cap2 doesn't work" << std::endl;
        }

        // Default resolutions of the frame are obtained.
        int frame_width = cap0.get(cv::CAP_PROP_FRAME_WIDTH);
        int frame_height = cap0.get(cv::CAP_PROP_FRAME_HEIGHT);

        bool recording = false;
        int videono = 1;

        VideoWriter video0;
        VideoWriter video1;
        VideoWriter video2;
        
        while (!cloud_viewer_->wasStopped ())
        {
            CloudConstPtr cloud;
            Mat frame0;
            Mat frame1;
            Mat frame2;

            cap0 >> frame0;
            cap1 >> frame1;
            cap2 >> frame2;

            if (frame0.empty())
                break;
            if (frame1.empty())
                break;
            if (frame2.empty())
                break;
            
            // DESCOMENTAR CASO QUEIRA VISUALIZAR IMAGENS
            // imshow( "Frame 0", frame0 );
            // imshow( "Frame 1", frame0 );
            // imshow( "Frame 2", frame0 );

            // SALVAR EM IMAGENS
            imwrite("test01.jpg", frame0);
            imwrite("test02.jpg", frame0);
            imwrite("test03.jpg", frame0);

            // SALVAR EM VIDEO
            if(!recording)
            {
                char path0[100];
                sprintf(path0, "%d.avi", videono);
                videono += 1;
                video0.open(path0, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
                
                char path1[100];
                sprintf(path1, "%d.avi", videono);
                videono += 1;
                video1.open(path1, cv::VideoWriter::fourcc('M','J','P','G'), 10, Size(frame_width,frame_height));
                
                char path2[100];
                sprintf(path2, "%d.avi", videono);
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

            // See if we can get a cloud
            if (cloud_mutex_.try_lock ())
            {
                cloud_.swap (cloud);
                cloud_mutex_.unlock ();
            }

            if (cloud)
            {
                pcl::io::savePCDFileASCII ("../point_clouds/test_pcd.pcd", *cloud);
                //FPS_CALC("drawing cloud");
                handler_.setInputCloud (cloud);
                if (!cloud_viewer_->updatePointCloud (cloud, handler_, "HDL"))
                    cloud_viewer_->addPointCloud (cloud, handler_, "HDL");

                cloud_viewer_->spinOnce ();
            }

            if (!grabber_.isRunning ())
                cloud_viewer_->spin ();

            std::this_thread::sleep_for(100us);
        }

      grabber_.stop ();
      cloud_connection.disconnect ();
    }

    pcl::visualization::PCLVisualizer::Ptr cloud_viewer_;

    pcl::Grabber& grabber_;
    std::mutex cloud_mutex_;

    CloudConstPtr cloud_;
    pcl::visualization::PointCloudColorHandler<pcl::PointXYZI> &handler_;
};


int main (int argc, char ** argv)
{
    // std::string hdlCalibration, ipaddress, port;

    // parse_argument (argc, argv, "-calibrationFile", hdlCalibration);
    // parse_argument (argc, argv, "-ipAdress", ipaddress);
    // parse_argument (argc, argv, "-port", port);

    std::string ipaddress( "192.168.1.70" );
    std::string port( "2368" );
   // std::string pcap;

    pcl::HDLGrabber grabber (boost::asio::ip::address::from_string( ipaddress ), boost::lexical_cast<unsigned short>( port ));

    pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> color_handler ("intensity");

    SimpleHDLViewer v (grabber, color_handler);
    v.run ();
    return (0);
}
