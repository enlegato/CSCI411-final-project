#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

// load image and convert to gray matrix of doubles
Mat loadAndConvertToGray(const char *imagePath)
{
    Mat img = imread(imagePath, IMREAD_COLOR);
    if (img.empty())
    {
        cerr << "Could not open or find the image" << endl;
        exit(-1);
    }

    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    gray.convertTo(gray, CV_64F);
    return gray;
}

// calculates gradient, crops by 1 pixel then goes through 3x3 matrix to get convolution
Mat calculateGradient(const Mat &src, const vector<vector<double>> &kernel)
{
    // matrix the same size as image
    Mat grad = Mat::zeros(src.size(), CV_64F);
    double sum;
    for (int i = 1; i < src.rows - 1; i++)
    {
        for (int j = 1; j < src.cols - 1; j++)
        {
            // get convolution of image at (i, j) and kernel
            sum = 0.0;
            for (int k = -1; k <= 1; k++)
            {
                for (int l = -1; l <= 1; l++)
                {
                    sum += src.at<double>(i + k, j + l) * kernel[k + 1][l + 1];
                }
            }
            grad.at<double>(i, j) = sum;
        }
    }
    return grad;
}

// guassian blur to smoth products
Mat applyGaussian(const Mat &src, int ksize, double sigma)
{
    Mat dst;
    GaussianBlur(src, dst, Size(ksize, ksize), sigma);
    return dst;
}

// returns matrix of harris responses(corner value)
Mat computeHarrisResponse(const Mat &gradXX, const Mat &gradYY, const Mat &gradXY, double k)
{
    Mat response = Mat::zeros(gradXX.size(), CV_64F);
    for (int i = 0; i < gradXX.rows; ++i)
    {
        for (int j = 0; j < gradXX.cols; ++j)
        {
            double xx = gradXX.at<double>(i, j);
            double yy = gradYY.at<double>(i, j);
            double xy = gradXY.at<double>(i, j);
            double det = xx * yy - xy * xy;
            double trace = xx + yy;
            response.at<double>(i, j) = det - k * trace * trace;
        }
    }
    return response;
}

// put a circle around each response greater than threshold
void markCorners(Mat &img, const Mat &response, double threshold)
{
    for (int i = 0; i < response.rows; ++i)
    {
        for (int j = 0; j < response.cols; ++j)
        {
            if (response.at<double>(i, j) > threshold)
            {
                circle(img, Point(j, i), 5, Scalar(0, 0, 255), 1);
            }
        }
    }
}

int main(int argc, char **argv)
{
    // check arguments and load image
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <image_path>" << " <threshold>" << std::endl;
        return -1;
    }

    double threshold = atof(argv[2]);

    // matrix of intensity (gray scale)
    Mat gray = loadAndConvertToGray(argv[1]);

    // sobel kernel in the x direction
    vector<vector<double>> skx = {
        {-1.0, 0.0, 1.0},
        {-2.0, 0.0, 2.0},
        {-1.0, 0.0, 1.0}};

    // sobel kernel in the y direction
    vector<vector<double>> sky = {
        {-1.0, -2.0, -1.0},
        {0.0, 0.0, 0.0},
        {1.0, 2.0, 1.0}};

    // gets gradient for each pixel in x and y directions
    Mat gradX = calculateGradient(gray, skx);
    Mat gradY = calculateGradient(gray, sky);

    // use gradients to get gradient matrix for image
    Mat gradXX = gradX.mul(gradX);
    Mat gradYY = gradY.mul(gradY);
    Mat gradXY = gradX.mul(gradY);

    // gradients with guassian filter for image
    Mat gaussXX = applyGaussian(gradXX, 3, 1.5);
    Mat gaussYY = applyGaussian(gradYY, 3, 1.5);
    Mat gaussXY = applyGaussian(gradXY, 3, 1.5);

    // matrix of harris responses using guassian filtered matrixes
    Mat harrisResponse = computeHarrisResponse(gaussXX, gaussYY, gaussXY, 0.05);

    // get regular image and overlay circles around corners
    Mat img = imread(argv[1], IMREAD_COLOR);
    markCorners(img, harrisResponse, threshold);

    // display the image with marked corners and resize the window
    Size windowSize(800, 600);
    Mat resizedImg;
    resize(img, resizedImg, windowSize);
    cv::imshow("Corners", resizedImg);
    cv::waitKey(0);

    return 0;
}
