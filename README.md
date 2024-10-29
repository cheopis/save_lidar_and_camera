# save_lidar_and_camera

1. Install opencv: 
TO DO

2. Install pcl: 
```
sudo apt install libpcl-dev
```

3. Install vtk: 
```
sudo apt-get install vtk7
```
or
```
sudo apt-get install vtk9
```

4. Change the cameras:

Show all cameras availables: ```v4l2-ctl --list-devices```

The cameras paths should be like: ```/dev/videox```, which ```x``` is the number of the camera. 
If you don't know which camera to choose, you can visualize the images with ```ffplay /dev/videox```.

Change the cameras numbers in the ```getImages.cpp``` file.

6. Change lidar adress:
   TO DO

7. Build the project:

```
mkdir build && cd build
cmake ..
make
```
