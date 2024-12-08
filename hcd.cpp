#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

struct pix
{
    double intensity = 0.0;
    double cornerValue = 0.0;
};

// return the intensity of the passed pixel with grayscale weighting
double intensityGray(Vec3b pixel)
{
    return ((((double)pixel[0] * 0.114) + ((double)pixel[1] * 0.587) + ((double)pixel[2] * 0.299)) / 3.0);
}

// returns the intesity of the passed pixel
double intensity(Vec3b pixel)
{
    return ((pixel[0] + pixel[1] + pixel[2]) / 3.0);
}

// returns intensity gradient for matrix
double calculateIntensityGradient(const Mat &intensity, const vector<vector<double>> &kernel, int i, int j)
{
    double result = 0.0;
    for (int ki = -1; ki <= 1; ++ki)
    {
        for (int kj = -1; kj <= 1; ++kj)
        {
            result += intensity.at<double>(i + ki, j + kj) * kernel[ki + 1][kj + 1];
        }
    }
    return result;
}

// gets the determinant of a 2x2 matrix (2d array)
double determinant(const vector<vector<double>> &matrix)
{
    return ((matrix[0][0] * matrix[1][1]) - (matrix[1][0] * matrix[0][1]));
}

// gets the trace of a 2x2 matrix (2d array)
double trace(const vector<vector<double>> &matrix)
{
    return (matrix[0][0] + matrix[1][1]);
}

int main(int argc, char **argv)
{
    // check arguments and load image
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <image_path>" << endl;
        return -1;
    }
    Mat img = imread(argv[1], IMREAD_COLOR);
    if (img.empty())
    {
        cerr << "Could not open or find the image" << endl;
        return -1;
    }

    // convert to grayscale and double precision
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    gray.convertTo(gray, CV_64F);

    // initialize and fillpixel array
    vector<vector<pix>> pArray(gray.rows, vector<pix>(gray.cols));
    for (int i = 0; i < gray.rows; ++i)
    {
        for (int j = 0; j < gray.cols; ++j)
        {
            pArray[i][j].intensity = gray.at<double>(i, j);
        }
    }

    // sobel kernels for x and y gradients
    vector<vector<double>> skx = {
        {-1.0, 0.0, 1.0},
        {-2.0, 0.0, 2.0},
        {-1.0, 0.0, 1.0}};
    vector<vector<double>> sky = {
        {-1.0, -2.0, -1.0},
        {0.0, 0.0, 0.0},
        {1.0, 2.0, 1.0}};

    double igx = 0.0;
    double igy = 0.0;
    double k = 0.05;

    // calculate gradients and corner values
    for (int i = 1; i < gray.rows - 1; ++i)
    {
        for (int j = 1; j < gray.cols - 1; ++j)
        {
            igx = calculateIntensityGradient(gray, skx, i, j);
            igy = calculateIntensityGradient(gray, sky, i, j);

            vector<vector<double>> M = {
                {igx * igx, igx * igy},
                {igx * igy, igy * igy}};

            pArray[i][j].cornerValue = determinant(M) - (k * pow(trace(M), 2));
        }
    }

    // threshold and mark corners
    double threshold = 75.0;
    for (int i = 1; i < gray.rows - 1; ++i)
    {
        for (int j = 1; j < gray.cols - 1; ++j)
        {
            if (pArray[i][j].cornerValue > threshold)
            {
                circle(img, Point(j, i), 5, Scalar(0, 0, 255), 1);
            }
        }
    }

    // display the image with marked corners
    imshow("Corners", img);
    waitKey(0);

    return 0;
}
