#include "Dip2.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <fstream>
using namespace std;
using namespace cv;

cv::Mat_<float> tryLoadImage(const std::string &filename)
{
    cv::Mat img = cv::imread(filename, 0);
    if (!img.data){
        cout << "ERROR: file " << filename << " not found" << endl;
        cout << "Press enter to exit"  << endl;
        cin.get();
        exit(-3);
    }

    // convert to floating point precision
    img.convertTo(img, CV_32FC1);
    return cv::Mat_<float>(img);
}

Mat resizeImage(const Mat& img) {
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    double screenScale = min((double)screenWidth / img.cols, (double)screenHeight / img.rows);
    double scale = screenScale * 0.5;
    Mat resized;
    resize(img, resized, Size(), scale, scale, INTER_LINEAR);
    return resized;
}

void displayImage(const Mat& img, const string& windowName) {
    namedWindow(windowName, WINDOW_AUTOSIZE);
    imshow(windowName, img);
}

Mat convertToDisplayableImage(const Mat& img) {
    Mat displayable;
    img.convertTo(displayable, CV_8U); // Assuming image is in floating point format
    return resizeImage(displayable);
}

cv::Mat_<float> generateNoisyImage(const cv::Mat_<float> &img, dip2::NoiseType noiseType)
{
    // generate images with different types of noise
    switch (noiseType) {
        case dip2::NOISE_TYPE_1: {
            // some temporary images
            Mat tmp1(img.rows, img.cols, CV_32FC1);
            Mat tmp2(img.rows, img.cols, CV_32FC1);
            // first noise operation
            float noiseLevel = 0.15;
            randu(tmp1, 0, 1);
            threshold(tmp1, tmp2, noiseLevel, 1, THRESH_BINARY);
            multiply(tmp2,img,tmp2);
            threshold(tmp1, tmp1, 1-noiseLevel, 1, THRESH_BINARY);
            tmp1 *= 255;
            tmp1 = tmp2 + tmp1;
            threshold(tmp1, tmp1, 255, 255, THRESH_TRUNC);
            return tmp1;
        } break;
        case dip2::NOISE_TYPE_2: {
            // some temporary images
            Mat tmp1(img.rows, img.cols, CV_32FC1);
            Mat tmp2(img.rows, img.cols, CV_32FC1);
            // second noise operation
            float noiseLevel = 50;
            randn(tmp1, 0, noiseLevel);
            tmp1 = img + tmp1;
            threshold(tmp1,tmp1,255,255,THRESH_TRUNC);
            threshold(tmp1,tmp1,0,0,THRESH_TOZERO);
            return tmp1;
        } break;
        default:
            throw std::runtime_error("Unhandled noise type!");
    }
}

float calculatePSNR(const cv::Mat &original, const cv::Mat &processed) {
    // Ensure the images are of the same size and type
    if (original.size() != processed.size() || original.type() != processed.type()) {
        std::cerr << "Original and processed images must have the same size and type" << std::endl;
        return -1;
    }

    cv::Mat diff;
    cv::absdiff(original, processed, diff); // Absolute difference between images
    diff.convertTo(diff, CV_32F); // Convert to float
    diff = diff.mul(diff); // Squaring each element

    double mse = cv::mean(diff)[0]; // Mean Squared Error
    if (mse == 0) {
        return std::numeric_limits<float>::infinity(); // Avoid division by zero
    }

    double psnr = 10.0 * log10((255 * 255) / mse); // Calculate PSNR
    return static_cast<float>(psnr);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "C:\\Users\\Juli\\Downloads\\dip02\\testimage.jpg" << endl;
        cout << "Press enter to exit" << endl;
        cin.get();
        return -1;
    }

    // Load original image
    cout << "Loading original image" << endl;
    Mat originalImage = imread(argv[1], IMREAD_COLOR);
    if (originalImage.empty()) {
        cerr << "Error: Unable to open test image!" << endl;
        return -1;
    }

    std::ofstream outFile("C:\\Users\\Juli\\Downloads\\dip02\\psnr_results.txt");
    if (!outFile) {
        std::cerr << "Error creating file!" << std::endl;
        return -1;
    }

    // Convert original image to grayscale and resize it
    Mat grayOriginal;
    cvtColor(originalImage, grayOriginal, COLOR_BGR2GRAY);
    Mat resizedGrayOriginal = resizeImage(grayOriginal); 
    displayImage(resizedGrayOriginal, "Original Grayscale Image");

    // Convert the resized grayscale image to float for filtering
    Mat floatGrayOriginal;
    resizedGrayOriginal.convertTo(floatGrayOriginal, CV_32F);

    // Process each noise type
    for (unsigned i = 0; i < dip2::NUM_NOISE_TYPES; i++) {
        cout << "Processing noise type: " << dip2::noiseTypeNames[i] << endl;

        // Generate noisy image
        Mat_<float> noisyImage = generateNoisyImage(floatGrayOriginal, (dip2::NoiseType)i);
        Mat displayNoisy = convertToDisplayableImage(noisyImage);
        displayImage(displayNoisy, string("Noisy Image - ") + dip2::noiseTypeNames[i]);

        // Denoise image
        Mat_<float> denoisedImage = denoiseImage(noisyImage, (dip2::NoiseType)i, chooseBestAlgorithm((dip2::NoiseType)i));
        Mat displayDenoised = convertToDisplayableImage(denoisedImage);
        displayImage(displayDenoised, string("Denoised Image - ") + dip2::noiseTypeNames[i]);

        // Calculate and display PSNR
        float psnrValue = calculatePSNR(floatGrayOriginal, denoisedImage);
        // Write the PSNR value to the file
        outFile << "PSNR value for " << dip2::noiseTypeNames[i] << ": " << psnrValue << " dB" << std::endl;
    }
    // Close the file
    outFile.close();
    if (outFile.fail()) {
        std::cerr << "Error: Failed to write to the file." << std::endl;
    } else {
        std::cout << "PSNR values written to C:\\Users\\Juli\\Downloads\\dip02\\psnr_results.txt successfully." << std::endl;
    }

    cout << "Processing completed." << endl;
    waitKey(0);
    return 0;
}


