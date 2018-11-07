#include <d2d1.h>
#include <map>
#include <vector>
#include "defs.h"
#include "utility.h"
#include <windows.h>
#include <wincodec.h>
using namespace std;

int currentStage;
ID2D1Factory *d2dFactory = NULL;
IWICImagingFactory *imageFactory = NULL;
vector<buttonUI> buttons;
map<int, int> buttonID;