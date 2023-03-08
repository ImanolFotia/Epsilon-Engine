#pragma once

#include "JoystickWrapper.hpp"
namespace framework {
namespace Input
{

#define GLFW_GAMEPAD_BUTTON_A 0
#define GLFW_GAMEPAD_BUTTON_B 1
#define GLFW_GAMEPAD_BUTTON_X 2
#define GLFW_GAMEPAD_BUTTON_Y 3
#define GLFW_GAMEPAD_BUTTON_LEFT_BUMPER 4
#define GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER 5
#define GLFW_GAMEPAD_BUTTON_BACK 6
#define GLFW_GAMEPAD_BUTTON_START 7
#define GLFW_GAMEPAD_BUTTON_GUIDE 8
#define GLFW_GAMEPAD_BUTTON_LEFT_THUMB 9
#define GLFW_GAMEPAD_BUTTON_RIGHT_THUMB 10
#define GLFW_GAMEPAD_BUTTON_DPAD_UP 11
#define GLFW_GAMEPAD_BUTTON_DPAD_RIGHT 12
#define GLFW_GAMEPAD_BUTTON_DPAD_DOWN 13
#define GLFW_GAMEPAD_BUTTON_DPAD_LEFT 14
#define GLFW_GAMEPAD_BUTTON_LAST GLFW_GAMEPAD_BUTTON_DPAD_LEFT
#define GLFW_GAMEPAD_BUTTON_CROSS GLFW_GAMEPAD_BUTTON_A
#define GLFW_GAMEPAD_BUTTON_CIRCLE GLFW_GAMEPAD_BUTTON_B
#define GLFW_GAMEPAD_BUTTON_SQUARE GLFW_GAMEPAD_BUTTON_X
#define GLFW_GAMEPAD_BUTTON_TRIANGLE GLFW_GAMEPAD_BUTTON_Y

namespace Joystick
{

enum Type
{
    XBOX360 = 0,
    XBOXONE,
    DUALSHOCK3,
    DUALSHOCK4,
    GENERIC
};

namespace Mappings
{

//Pure abstract class, do not instance
class Mapping
{
    public:
    virtual int A() = 0;
    virtual int B() = 0;
    virtual int X() = 0;
    virtual int Y() = 0;
    virtual int LEFT_BUMPER() = 0;
    virtual int RIGHT_BUMPER() = 0;
    virtual int BACK() = 0;
    virtual int START() = 0;
    virtual int GUIDE() = 0;
    virtual int LEFT_THUMB() = 0;
    virtual int RIGHT_THUMB() = 0;
    virtual int UP() = 0;
    virtual int RIGHT() = 0;
    virtual int DOWN() = 0;
    virtual int LEFT() = 0;
    virtual int CROSS() = 0;
    virtual int CIRCLE() = 0;
    virtual int SQUARE() = 0;
    virtual int TRIANGLE() = 0;
    
    virtual int DPAD_RIGHT_X() = 0;
    virtual int DPAD_RIGHT_Y() = 0;
    virtual int DPAD_LEFT_X () = 0;
    virtual int DPAD_LEFT_Y () = 0;
    virtual int DPAD_LEFT_TRIGGER() = 0;
    virtual int DPAD_RIGHT_TRIGGER() = 0;

private:
    int mA = 0;
    int mB = 0;
    int mX = 0;
    int mY = 0;
    int mLEFT_BUMPER = 0;
    int mRIGHT_BUMPER = 0;
    int mBACK = 0;
    int mSTART = 0;
    int mGUIDE = 0;
    int mLEFT_THUMB = 0;
    int mRIGHT_THUMB = 0;
    int mUP = 0;
    int mRIGHT = 0;
    int mDOWN = 0;
    int mLEFT = 0;
    int mCROSS = 0;
    int mCIRCLE = 0;
    int mSQUARE = 0;
    int mTRIANGLE = 0;

    int mDPAD_RIGHT_X = 0;
    int mDPAD_RIGHT_Y = 0;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 0;
    int mDPAD_LEFT_TRIGGER = 0;
    int mDPAD_RIGHT_TRIGGER = 0;
};

class Xbox : public Mapping
{
public:
    virtual int A() { return mA; }
    virtual int B() { return mB; }
    virtual int X() { return mX; }
    virtual int Y() { return mY; }
    virtual int LEFT_BUMPER() { return mLEFT_BUMPER; }
    virtual int RIGHT_BUMPER() { return mRIGHT_BUMPER; }
    virtual int BACK() { return mBACK; }
    virtual int START() { return mSTART; }
    virtual int GUIDE() { return mGUIDE; }
    virtual int LEFT_THUMB() { return mLEFT_THUMB; }
    virtual int RIGHT_THUMB() { return mRIGHT_THUMB; }
    virtual int UP() { return mUP; }
    virtual int RIGHT() { return mRIGHT; }
    virtual int DOWN() { return mDOWN; }
    virtual int LEFT() { return mLEFT; }
    virtual int CROSS() { return mCROSS; }
    virtual int CIRCLE() { return mCIRCLE; }
    virtual int SQUARE() { return mSQUARE; }
    virtual int TRIANGLE() { return mTRIANGLE; }
    
    virtual int DPAD_RIGHT_X() { return mDPAD_RIGHT_X; }
    virtual int DPAD_RIGHT_Y() { return mDPAD_RIGHT_Y; }
    virtual int DPAD_LEFT_X () { return mDPAD_LEFT_X; }
    virtual int DPAD_LEFT_Y () { return mDPAD_LEFT_Y; }
    virtual int DPAD_LEFT_TRIGGER() { return mDPAD_LEFT_TRIGGER; }
    virtual int DPAD_RIGHT_TRIGGER() { return mDPAD_RIGHT_TRIGGER; }

private:
    int mA = GLFW_GAMEPAD_BUTTON_A;
    int mB = GLFW_GAMEPAD_BUTTON_B;
    int mX = GLFW_GAMEPAD_BUTTON_X;
    int mY = GLFW_GAMEPAD_BUTTON_Y;
    int mLEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
    int mRIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
    int mBACK = GLFW_GAMEPAD_BUTTON_BACK;
    int mSTART = GLFW_GAMEPAD_BUTTON_START;
    int mGUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
    int mLEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
    int mRIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
    int mUP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
    int mRIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    int mDOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
    int mLEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
    int mCROSS = GLFW_GAMEPAD_BUTTON_A;
    int mCIRCLE = GLFW_GAMEPAD_BUTTON_B;
    int mSQUARE = GLFW_GAMEPAD_BUTTON_X;
    int mTRIANGLE = GLFW_GAMEPAD_BUTTON_Y;
    
    int mDPAD_RIGHT_X = 2;
    int mDPAD_RIGHT_Y = 3;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 1;
    int mDPAD_LEFT_TRIGGER = 5;
    int mDPAD_RIGHT_TRIGGER = 6;
};

class DualShock4 : public Mapping
{

public:
    virtual int A() { return mA; }
    virtual int B() { return mB; }
    virtual int X() { return mX; }
    virtual int Y() { return mY; }
    virtual int LEFT_BUMPER() { return mLEFT_BUMPER; }
    virtual int RIGHT_BUMPER() { return mRIGHT_BUMPER; }
    virtual int BACK() { return mBACK; }
    virtual int START() { return mSTART; }
    virtual int GUIDE() { return mGUIDE; }
    virtual int LEFT_THUMB() { return mLEFT_THUMB; }
    virtual int RIGHT_THUMB() { return mRIGHT_THUMB; }
    virtual int UP() { return mUP; }
    virtual int RIGHT() { return mRIGHT; }
    virtual int DOWN() { return mDOWN; }
    virtual int LEFT() { return mLEFT; }
    virtual int CROSS() { return mCROSS; }
    virtual int CIRCLE() { return mCIRCLE; }
    virtual int SQUARE() { return mSQUARE; }
    virtual int TRIANGLE() { return mTRIANGLE; }

    virtual int DPAD_RIGHT_X() { return mDPAD_RIGHT_X; }
    virtual int DPAD_RIGHT_Y() { return mDPAD_RIGHT_Y; }
    virtual int DPAD_LEFT_X () { return mDPAD_LEFT_X; }
    virtual int DPAD_LEFT_Y () { return mDPAD_LEFT_Y; }
    virtual int DPAD_LEFT_TRIGGER() { return mDPAD_LEFT_TRIGGER; }
    virtual int DPAD_RIGHT_TRIGGER() { return mDPAD_RIGHT_TRIGGER; }

private:
    int mA = 1;
    int mB = 2;
    int mX = 0;
    int mY = 3;
    int mLEFT_BUMPER = 4;
    int mRIGHT_BUMPER = 5;
    int mBACK = 8;
    int mSHARE = 8;
    int mSTART = 9;
    int mGUIDE = 13;
    int mLEFT_THUMB = 10;
    int mRIGHT_THUMB = 11;
    int mUP = 14;
    int mRIGHT = 15;
    int mDOWN = 16;
    int mLEFT = 17;
    int mCROSS = 1;
    int mCIRCLE = 2;
    int mSQUARE = 0;
    int mTRIANGLE = 3;

    int mDPAD_RIGHT_X = 2;
    int mDPAD_RIGHT_Y = 5;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 1;
    int mDPAD_LEFT_TRIGGER = 3;
    int mDPAD_RIGHT_TRIGGER = 4;
};

class DualShock3 : public Mapping
{
public:
    virtual int A() { return mA; }
    virtual int B() { return mB; }
    virtual int X() { return mX; }
    virtual int Y() { return mY; }
    virtual int LEFT_BUMPER() { return mLEFT_BUMPER; }
    virtual int RIGHT_BUMPER() { return mRIGHT_BUMPER; }
    virtual int BACK() { return mBACK; }
    virtual int START() { return mSTART; }
    virtual int GUIDE() { return mGUIDE; }
    virtual int LEFT_THUMB() { return mLEFT_THUMB; }
    virtual int RIGHT_THUMB() { return mRIGHT_THUMB; }
    virtual int UP() { return mUP; }
    virtual int RIGHT() { return mRIGHT; }
    virtual int DOWN() { return mDOWN; }
    virtual int LEFT() { return mLEFT; }
    virtual int CROSS() { return mCROSS; }
    virtual int CIRCLE() { return mCIRCLE; }
    virtual int SQUARE() { return mSQUARE; }
    virtual int TRIANGLE() { return mTRIANGLE; }
    
    virtual int DPAD_RIGHT_X() { return mDPAD_RIGHT_X; }
    virtual int DPAD_RIGHT_Y() { return mDPAD_RIGHT_Y; }
    virtual int DPAD_LEFT_X () { return mDPAD_LEFT_X; }
    virtual int DPAD_LEFT_Y () { return mDPAD_LEFT_Y; }
    virtual int DPAD_LEFT_TRIGGER() { return mDPAD_LEFT_TRIGGER; }
    virtual int DPAD_RIGHT_TRIGGER() { return mDPAD_RIGHT_TRIGGER; }

private:
    int mA = 0;
    int mB = 0;
    int mX = 0;
    int mY = 0;
    int mLEFT_BUMPER = 0;
    int mRIGHT_BUMPER = 0;
    int mBACK = 0;
    int mSTART = 0;
    int mGUIDE = 0;
    int mLEFT_THUMB = 0;
    int mRIGHT_THUMB = 0;
    int mUP = 0;
    int mRIGHT = 0;
    int mDOWN = 0;
    int mLEFT = 0;
    int mCROSS = 0;
    int mCIRCLE = 0;
    int mSQUARE = 0;
    int mTRIANGLE = 0;
    
    int mDPAD_RIGHT_X = 0;
    int mDPAD_RIGHT_Y = 0;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 0;
    int mDPAD_LEFT_TRIGGER = 0;
    int mDPAD_RIGHT_TRIGGER = 0;
};

class Generic : public Mapping
{
public:
    virtual int A() { return mA; }
    virtual int B() { return mB; }
    virtual int X() { return mX; }
    virtual int Y() { return mY; }
    virtual int LEFT_BUMPER() { return mLEFT_BUMPER; }
    virtual int RIGHT_BUMPER() { return mRIGHT_BUMPER; }
    virtual int BACK() { return mBACK; }
    virtual int START() { return mSTART; }
    virtual int GUIDE() { return mGUIDE; }
    virtual int LEFT_THUMB() { return mLEFT_THUMB; }
    virtual int RIGHT_THUMB() { return mRIGHT_THUMB; }
    virtual int UP() { return mUP; }
    virtual int RIGHT() { return mRIGHT; }
    virtual int DOWN() { return mDOWN; }
    virtual int LEFT() { return mLEFT; }
    virtual int CROSS() { return mCROSS; }
    virtual int CIRCLE() { return mCIRCLE; }
    virtual int SQUARE() { return mSQUARE; }
    virtual int TRIANGLE() { return mTRIANGLE; }

    virtual int DPAD_RIGHT_X() { return mDPAD_RIGHT_X; }
    virtual int DPAD_RIGHT_Y() { return mDPAD_RIGHT_Y; }
    virtual int DPAD_LEFT_X () { return mDPAD_LEFT_X; }
    virtual int DPAD_LEFT_Y () { return mDPAD_LEFT_Y; }
    virtual int DPAD_LEFT_TRIGGER() { return mDPAD_LEFT_TRIGGER; }
    virtual int DPAD_RIGHT_TRIGGER() { return mDPAD_RIGHT_TRIGGER; }
private:
    int mA = 0;
    int mB = 0;
    int mX = 0;
    int mY = 0;
    int mLEFT_BUMPER = 0;
    int mRIGHT_BUMPER = 0;
    int mBACK = 0;
    int mSTART = 0;
    int mGUIDE = 0;
    int mLEFT_THUMB = 0;
    int mRIGHT_THUMB = 0;
    int mUP = 0;
    int mRIGHT = 0;
    int mDOWN = 0;
    int mLEFT = 0;
    int mCROSS = 0;
    int mCIRCLE = 0;
    int mSQUARE = 0;
    int mTRIANGLE = 0;

    int mDPAD_RIGHT_X = 0;
    int mDPAD_RIGHT_Y = 0;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 0;
    int mDPAD_LEFT_TRIGGER = 0;
    int mDPAD_RIGHT_TRIGGER = 0;
};

class Custom : public Mapping
{
public:
    virtual int A() { return mA; }
    virtual int B() { return mB; }
    virtual int X() { return mX; }
    virtual int Y() { return mY; }
    virtual int LEFT_BUMPER() { return mLEFT_BUMPER; }
    virtual int RIGHT_BUMPER() { return mRIGHT_BUMPER; }
    virtual int BACK() { return mBACK; }
    virtual int START() { return mSTART; }
    virtual int GUIDE() { return mGUIDE; }
    virtual int LEFT_THUMB() { return mLEFT_THUMB; }
    virtual int RIGHT_THUMB() { return mRIGHT_THUMB; }
    virtual int UP() { return mUP; }
    virtual int RIGHT() { return mRIGHT; }
    virtual int DOWN() { return mDOWN; }
    virtual int LEFT() { return mLEFT; }
    virtual int CROSS() { return mCROSS; }
    virtual int CIRCLE() { return mCIRCLE; }
    virtual int SQUARE() { return mSQUARE; }
    virtual int TRIANGLE() { return mTRIANGLE; }

    virtual int DPAD_RIGHT_X() { return mDPAD_RIGHT_X; }
    virtual int DPAD_RIGHT_Y() { return mDPAD_RIGHT_Y; }
    virtual int DPAD_LEFT_X () { return mDPAD_LEFT_X; }
    virtual int DPAD_LEFT_Y () { return mDPAD_LEFT_Y; }
    virtual int DPAD_LEFT_TRIGGER() { return mDPAD_LEFT_TRIGGER; }
    virtual int DPAD_RIGHT_TRIGGER() { return mDPAD_RIGHT_TRIGGER; }
private:
    int mA = 0;
    int mB = 0;
    int mX = 0;
    int mY = 0;
    int mLEFT_BUMPER = 0;
    int mRIGHT_BUMPER = 0;
    int mBACK = 0;
    int mSTART = 0;
    int mGUIDE = 0;
    int mLEFT_THUMB = 0;
    int mRIGHT_THUMB = 0;
    int mUP = 0;
    int mRIGHT = 0;
    int mDOWN = 0;
    int mLEFT = 0;
    int mCROSS = 0;
    int mCIRCLE = 0;
    int mSQUARE = 0;
    int mTRIANGLE = 0;
    
    int mDPAD_RIGHT_X = 0;
    int mDPAD_RIGHT_Y = 0;
    int mDPAD_LEFT_X = 0;
    int mDPAD_LEFT_Y = 0;
    int mDPAD_LEFT_TRIGGER = 0;
    int mDPAD_RIGHT_TRIGGER = 0;
};

} // namespace Mappings

namespace Mappings
{
//In my testing XB360 and XBONE shared the same button mapping
namespace XBOX
{

static constexpr int A = GLFW_GAMEPAD_BUTTON_A;
static constexpr int B = GLFW_GAMEPAD_BUTTON_B;
static constexpr int X = GLFW_GAMEPAD_BUTTON_X;
static constexpr int Y = GLFW_GAMEPAD_BUTTON_Y;
static constexpr int LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
static constexpr int RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
static constexpr int BACK = GLFW_GAMEPAD_BUTTON_BACK;
static constexpr int START = GLFW_GAMEPAD_BUTTON_START;
static constexpr int GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
static constexpr int LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;

static constexpr int RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
static constexpr int UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
static constexpr int RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
static constexpr int DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
static constexpr int LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
static constexpr int CROSS = GLFW_GAMEPAD_BUTTON_A;
static constexpr int CIRCLE = GLFW_GAMEPAD_BUTTON_B;
static constexpr int SQUARE = GLFW_GAMEPAD_BUTTON_X;
static constexpr int TRIANGLE = GLFW_GAMEPAD_BUTTON_Y;

} // namespace XBOX
namespace DUALSHOCK4
{
static constexpr int A = GLFW_GAMEPAD_BUTTON_A;
static constexpr int B = GLFW_GAMEPAD_BUTTON_B;
static constexpr int X = GLFW_GAMEPAD_BUTTON_X;
static constexpr int Y = GLFW_GAMEPAD_BUTTON_Y;
static constexpr int LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
static constexpr int RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
static constexpr int BACK = GLFW_GAMEPAD_BUTTON_BACK;
static constexpr int START = GLFW_GAMEPAD_BUTTON_START;
static constexpr int GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE;
static constexpr int LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB;

static constexpr int RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
static constexpr int UP = GLFW_GAMEPAD_BUTTON_DPAD_UP;
static constexpr int RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
static constexpr int DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
static constexpr int LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
static constexpr int CROSS = GLFW_GAMEPAD_BUTTON_A;
static constexpr int CIRCLE = GLFW_GAMEPAD_BUTTON_B;
static constexpr int SQUARE = GLFW_GAMEPAD_BUTTON_X;
static constexpr int TRIANGLE = GLFW_GAMEPAD_BUTTON_Y;

} // namespace DUALSHOCK4

//Don't personally own a PS3, can't test the layout for now
namespace DUALSHOCK3
{
} // namespace DUALSHOCK3

} // namespace Mappings

} // namespace Joystick
} // namespace Input
}