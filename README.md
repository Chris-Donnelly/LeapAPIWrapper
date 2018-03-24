# LeapAPIWrapper
## C++ API Wrapper and Classes for Leap Motion Controller

*(Created for MSc Dissertation Project "Virtual Paintburhs", using Visual Studio 2015))*

A static wrapper for the Leap Motion API for use with Leap Motion controller. The wrapper will monitor connectivity and use a polling architecture to retrieve the latest data from the sensor/API. Enums for left and right hand are applied to retrieve hands individually.

*Please note this code was produced under constraints, and is therefore only partially complete and may require further architecturing etc, for improvements*

### Requirements
- Leap Motion Contoller
- Leap Motion software and SDK (version used was **3.2.0+45899**)
- GLM OpenGL Math libraries (GLM Website, or use NUGet package manager in VS, or your preferred package manager)

*Please ensure your leap motion SDK path is correctly addressed in your includes/libraries settings*

## Initialization

The wrapper is initialized using

```C++
bool LeapAPI::Initialize()
```

where true=success and false=failure

## Polling / Updating

The wrapper is instructed to check for new data using:

```C++
void Update(float dTime, float wTime);
```

passing a delta time and global application time to the method. Note that passing any other values than delta and world time may produce unspecified behaviour and data, especially when timing.

This method will check for new data frames int he API, and retrieve the new data accordingly. It also checks the connectivity state of the sensor, API and software services.

## Status

After updating the wrapper to the latest possible information, the state of the wrapper can be checked using

```C++
bool StatusIsWorking();
```

and a check for new data can be performed with

```C++
bool HasNewData()
```

returning true if there is a new data frame.

## Sensor/API feedback

The API will return values of details fromt he sensor and API, such as left or right hand detected (```bool LeftHandDetected()```, etc), and the number of hands the sensor detects. Operation modes such as HMD can also be checked and set using ``bool GetSensorHMDMode()```.

## Hand data

If the *HasNewData()* method returns true, the wrapper can provide the information (one hand at a time) by using 

```C++
bool GetHandData(HANDTYPE type, Leap::Hand &hand)
```
which outputs to the hand object provided as *&hand*. True return values indicate success. The output hand object is of internal Leap type; the Leap Motion SDK will explain these.

## Camera views

The controller can return the camera data (internally retrieved as UINT8 greyscale values, and converted by the wrapper to RGBA float triplets with no transparency in the alpha channel). The implementation stops short of performing any API specific data copying etc; this is removed to de-couple the code from the rendering system which was used in the dissertation. The member function *bool LeapAPI::RenderTo(LEAPCAMERA cam, Material * mtl)* starts this process. Simply add your own code to copy and use the data as needed.

