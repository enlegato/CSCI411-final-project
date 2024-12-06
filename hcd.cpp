#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;

struct pix
{
    cv::Vec3b p;
    double intensity;
    vector<vector<double>> M = vector<vector<double>>(2, vector<double>(2, 0.0));
    double cornerValue;
};

// return the intesity of the passed pixel
double intensity(cv::Vec3b pixel)
{
    return ((pixel[0] + pixel[1] + pixel[2]) / 3);
}

double calculateIntensityGradient(vector<vector<double>> sk, vector<vector<pix>> pArray, int i, int j)
{
    return ((pArray[i + 1][j + 1].intensity * sk[2][2]) + (pArray[i + 1][j].intensity * sk[1][2]) + (pArray[i + 1][j - 1].intensity * sk[0][2]) + (pArray[i][j + 1].intensity * sk[2][1]) + (pArray[i][j].intensity * sk[1][1]) + (pArray[i][j - 1].intensity * sk[0][1]) + (pArray[i - 1][j + 1].intensity * sk[2][0]) + (pArray[i - 1][j].intensity * sk[1][0]) + (pArray[i - 1][j - 1].intensity * sk[0][0]));
}

double determinant(vector<vector<double>> matrix)
{
    return ((matrix[0][0] * matrix[1][1]) - (matrix[1][0] * matrix[0][1]));
}

int main(int argc, char **argv)
{
    // check arguments and load image
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << std::endl;
        return -1;
    }
    cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);
    if (img.empty())
    {
        std::cerr << "Could not open or find the image" << std::endl;
        return -1;
    }

    // take the image and make a 2d array of its pixels
    vector<vector<pix>> pArray(img.cols, vector<pix>(img.rows));
    for (int i = 0; i < img.cols; ++i)
    {
        for (int j = 0; j < img.rows; ++j)
        {
            pArray[i][j].p = img.at<cv::Vec3b>(i, j);
            pArray[i][j].intensity = intensity(pArray[i][j].p);
        }
    }

    // go through and populate each pixels intensity gradient matrix
    // sobel kernel for x
    vector<vector<double>> skx = {
        {-1.0, 0.0, 1.0},
        {-2.0, 0.0, 2.0},
        {-1.0, 0.0, 1.0}};
    // sobel kernel for y
    vector<vector<double>> sky = {
        {-1.0, -2.0, -1.0},
        {0.0, 0.0, 0.0},
        {1.0, 2.0, 1.0}};

    double igx = 0.0;
    double igy = 0.0;
    // crops in by 1 and calculates gradient
    for (int i = 1; i < img.cols - 1; ++i)
    {
        for (int j = 1; j < img.rows - 1; ++j)
        {
            igx = calculateIntensityGradient(skx, pArray, i, j);
            igy = calculateIntensityGradient(sky, pArray, i, j);
            pArray[i][j].M[0][0] = igx * igx;
            pArray[i][j].M[0][1] = igx * igy;
            pArray[i][j].M[1][0] = igx * igy;
            pArray[i][j].M[1][1] = igy * igy;
        }
    }

    return 0;
}
