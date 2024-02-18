
//  IMPLEMENTATION OF CONCEPT
/*
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;



Mat imgHSV, mask;
int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;  


int main()
{
	string path = "Resources/shapes.png";
	Mat img = imread(path);

	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	namedWindow("Trackbar", (640, 200));
	createTrackbar("hue min", "Trackbar", &hmin, 179);
	createTrackbar("hue max", "Trackbar", &hmax, 179);
	createTrackbar("sat min", "Trackbar", &smin, 255);
	createTrackbar("sat max", "Trackbar", &smax, 255);
	createTrackbar("val min", "Trackbar", &vmin, 255);
	createTrackbar("val max", "Trackbar", &vmax, 255);

	while (true)
	{

		Scalar lower(hmin, smin, vmin);
		Scalar upper(hmax, smax, vmax);

		inRange(imgHSV, lower, upper, mask);

		imshow("image", img);
		imshow("image HSV", imgHSV);
		imshow("image mask", mask);

		waitKey(1);
	}
}
*/






                           ////////project 2  COLOR   DETECTION //////////////////

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;




Mat img;
vector<vector<int>> newPoints;

vector<vector<int>> myColors{ {124,48,117,143,170,255}, //purple
	{68,72,156,102,126,255} //green
    // orange
};

vector<Scalar>myColorValues{ {255,0,255}, //purple
	{0,255,0} // orange
};



Point getContours( Mat imgDil)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	// drawContours(img, contours, -1, Scalar(255,0,255), 2);


	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	Point myPoint(0, 0);

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;

		
		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
			myPoint.x = boundRect[i].x + boundRect[i].width / 2;
			myPoint.y = boundRect[i].y;

			drawContours(img, conPoly, i, Scalar(255, 0, 255), 2);
			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);

		}
	}
	return myPoint;

}



vector<vector<int>> findColor(Mat img)
{
	Mat imgHSV;

	cvtColor(img, imgHSV, COLOR_BGR2HSV);

	for (int i = 0; i < myColors.size(); i++)
	{
		Scalar lower(myColors[i][0], myColors[i][1], myColors[i][2]);
		Scalar upper(myColors[i][3], myColors[i][4], myColors[i][5]);
		Mat mask;
		inRange(imgHSV, lower, upper, mask);
		// imshow(to_string(i), mask);
		Point myPoint = getContours(mask);

		if (myPoint.x != 0 && myPoint.y != 0) {
			newPoints.push_back({ myPoint.x , myPoint.y , i });
		}
	}
	return newPoints;
}

void drawOnCanvas(vector<vector<int>> newPoints, vector<Scalar> myColorValues)
{
	for (int i = 0; i < newPoints.size(); i++)
	{
		circle(img, Point(newPoints[i][0],newPoints[i][1]), 10, myColorValues[newPoints[i][2]], FILLED); 
	}
}

void main()
{
	VideoCapture cap(1);

	while (true)
	{

		cap.read(img);
		newPoints = findColor(img);
		drawOnCanvas(newPoints, myColorValues);
		imshow("Image", img);
		waitKey(1);

	}
}





/*
//////project3   Scanner//////////////////////////////


#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;


Mat imgOriginal, imgGray, imgCanny, imgThre, imgBlur, imgDil, imgErode ;
vector<Point> initialPoints;
Mat preProcessing(Mat img)
{

	cvtColor(img, imgGray, COLOR_BGR2GRAY);
	GaussianBlur(imgGray, imgBlur, Size(3, 3), 3, 0);
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(imgCanny, imgGray, kernel);
	// erode(imgDil, imgErode, kernel);
	return imgDil;
}



vector<Point> getContours(Mat imgDil)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	// drawContours(img, contours, -1, Scalar(255,0,255), 2);


	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());

	vector<Point> biggest;
	int maxArea = 0;

	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << area << endl;


		string objectType;

		if (area > 1000)
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

			if(area > maxArea && conPoly[i].size() == 4 )
			{
				drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 5);
				biggest = { conPoly[i][0], conPoly[i][1] ,conPoly[i][2], conPoly[i][3] };
				maxArea = area;
			}

			//drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 2);
			//rectangle(imgOriginal, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 255, 0), 5);

		}
	}
	return biggest;

}


void drawPoints(vector<Point> points, Scalar color)
{

	for (int i = 0; i < points.size(); i++)
	{
		circle(imgOriginal ,points[i], 5, color, FILLED);
		putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 2, color, 2);
	}
}

int main()
{
	string path = "Resources/paper.jpg";
	imgOriginal = imread(path);
	resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5);

	// preprocessing
	imgThre = preProcessing(imgOriginal);

	initialPoints =  getContours(imgThre);
	drawPoints(initialPoints, Scalar(0, 0, 255));

	imshow("Image", imgOriginal);
	imshow("Image Dilation", imgThre);

	waitKey(0);

}

*/