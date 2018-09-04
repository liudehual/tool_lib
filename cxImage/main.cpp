#include <iostream>
#include "ximage.h"
using namespace std;

int main()
{
  CxImage *newImage=new CxImage;
  newImage->Create(120,120,1,1);
  newImage->Load("test.jpg");
  newImage->Resample(200,200);
  newImage->Save("test2.bmp",CXIMAGE_FORMAT_BMP);
  cout << "Hello World!" << endl;
  return 0;
}

