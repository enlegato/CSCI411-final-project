#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

struct pix
{
    double intensity = 0.0;
    double cornerValue = 0.0;
};

// return the intesity of the passed pixel with greyscale weighting
double intensity(cv::Vec3b pixel)
{
    return ((((double)pixel[0] * 0.114) + ((double)pixel[1] * 0.587) + ((double)pixel[2] * 0.299)) / 3.0);
}

double calculateIntensityGradient(vector<vector<double>> sk, vector<vector<pix>> pArray, int i, int j)
{
    return ((pArray[i + 1][j + 1].intensity * sk[2][2]) + (pArray[i + 1][j].intensity * sk[1][2]) + (pArray[i + 1][j - 1].intensity * sk[0][2]) + (pArray[i][j + 1].intensity * sk[2][1]) + (pArray[i][j].intensity * sk[1][1]) + (pArray[i][j - 1].intensity * sk[0][1]) + (pArray[i - 1][j + 1].intensity * sk[2][0]) + (pArray[i - 1][j].intensity * sk[1][0]) + (pArray[i - 1][j - 1].intensity * sk[0][0]));
}

double determinant(vector<vector<double>> matrix)
{
    return ((matrix[0][0] * matrix[1][1]) - (matrix[1][0] * matrix[0][1]));
}

double trace(vector<vector<double>> matrix)
{
    return (matrix[0][0] + matrix[1][1]);
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
            pArray[i][j].intensity = intensity(img.at<cv::Vec3b>(i, j));
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
    // sensitivity value
    double k = 0.04;
    // crops in by 1 and calculates gradient and corner value
    for (int i = 1; i < img.cols - 1; ++i)
    {
        for (int j = 1; j < img.rows - 1; ++j)
        {
            igx = calculateIntensityGradient(skx, pArray, i, j);
            igy = calculateIntensityGradient(sky, pArray, i, j);

            vector<vector<double>> M = {
                {igx * igx, igx * igy},
                {igx * igy, igy * igy}};

            pArray[i][j].cornerValue = determinant(M) - (k * pow(trace(M), 2));
        }
    }

    // show corners on image
    double threshold = 100000.0;
    for (int i = 0; i < img.cols; ++i)
    {
        for (int j = 0; j < img.rows; ++j)
        {
            if (pArray[i][j].cornerValue > threshold)
            {
                img.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
            }
        }
    }

    cv::imshow("Corners", img);
    cv::waitKey(0);
    return 0;
}
