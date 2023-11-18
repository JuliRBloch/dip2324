//============================================================================
// Name        : Dip2.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description : 
//============================================================================

#include "Dip2.h"
#include <iostream>
#include <algorithm>

using namespace std;
using namespace cv;

namespace dip2 {


/**
 * @brief Convolution in spatial domain.
 * @details Performs spatial convolution of image and filter kernel.
 * @params src Input image
 * @params kernel Filter kernel
 * @returns Convolution result
 */
cv::Mat_<float> spatialConvolution(const cv::Mat_<float>& src, const cv::Mat_<float>& kernel)
{
    //flip kerel
    int ksize = kernel.rows;
    int km = ksize+1/2; // kernel middle

        Mat k_flipped = kernel;

        for (int i=0; i <= (km); i++)
        {
            for (int j=0; j <= (km); j++)
            {
                k_flipped.at<float>((2*km-i),(2*km-j)) = kernel(i,j);

            }

        }

        int imgcols = src.cols;
        int imgrows = src.rows;

        //Mat output(src.rows, src.cols, CV_32FC1);
        Mat output = src;

        // padded picture
        int border = km-1;
        Mat src_gray(src.rows, src.cols, CV_32FC1);

        src_gray = src;

        imwrite("test0.jpg", src_gray); // is grayscale

        // constructs a larger image to fit both the image and the border
        Mat src_wb(src.rows + border*2, src.cols + border*2, src.depth());
        // make border with copied nearest defined pixel
        copyMakeBorder(src_gray, src_wb, border, border, border, border, BORDER_REPLICATE);

        imwrite("test2.jpg", src_wb); // is grayscale has border


        for (int xp = km; xp <= (imgrows-km); xp++)
        {
            for(int yp =km; yp <= (imgcols-km); yp++)
            {
                float sum = 0.0;
                int cnt = 0;

                for (int xk = 0; xk <= ksize-1; xk++)
                {
                    for (int yk=0; yk <= ksize-1; yk++)
                    {

                        sum += src_wb.at<float>((xp-km+xk),(yp-km+yk)) * k_flipped.at<float>(xk, yk);

                        cnt +=1;

                    }
                }
                output.at<float>(xp-km,yp-km) = (sum/cnt);

            }

        }

        return output;
}

/**
 * @brief Moving average filter (aka box filter)
 * @note: you might want to use Dip2::spatialConvolution(...) within this function
 * @param src Input image
 * @param kSize Window size used by local average
 * @returns Filtered image
 */
cv::Mat_<float> averageFilter(const cv::Mat_<float>& src, int kSize)
{
    float weight = 1 / (kSize * kSize);
    Mat kernel(kSize, kSize, CV_32FC1, Scalar::all(weight));

    return spatialConvolution(src, kernel);
}

/**
 * @brief Median filter
 * @param src Input image
 * @param kSize Window size used by median operation
 * @returns Filtered image
 */
cv::Mat_<float> medianFilter(const cv::Mat_<float>& src, int kSize)
{

    int km = kSize+1/2; // kernel middle
    int border = km-1;

    Mat src_gray = src;

    Mat src_wb(src.rows + border*2, src.cols + border*2, src.depth());

   copyMakeBorder(src_gray, src_wb, border, border, border, border, BORDER_REPLICATE);

    Mat output = src;

    for (int xp = 0; xp <= src.rows-1; xp++)
    {
        for (int yp= 0; yp <= src.cols-1; yp++)
        {
            vector<float> values;

            for (int xk = 0; xk <= kSize-1; xk++)
            {
                for (int yk=0; yk <= kSize-1; yk++)
                {

                    values.push_back(src_wb.at<float>(xp+xk,yp+yk));

                }
            }
            std::sort(values.begin(), values.end());
            // cout << " MedFilter value = " << values[(((kSize*kSize)-1)/2)]  << endl; // is 1 ?
            output.at<float>(xp,yp) = values[(((kSize*kSize)-1)/2)];

        }
    }


  //  cout << " MedFilter kSize= " << kSize  << endl; // is 1 ?
    imwrite("test_med.jpg", output); // why is pic so dark?
   return output;
}

/**
 * @brief Bilateral filer
 * @param src Input image
 * @param kSize Size of the kernel
 * @param sigma_spatial Standard-deviation of the spatial kernel
 * @param sigma_radiometric Standard-deviation of the radiometric kernel
 * @returns Filtered image
 */
cv::Mat_<float> bilateralFilter(const cv::Mat_<float>& src, int kSize, float sigma_spatial, float sigma_radiometric)
{
    // TO DO !!

    return src.clone();
}

/**
 * @brief Non-local means filter
 * @note: This one is optional!
 * @param src Input image
 * @param searchSize Size of search region
 * @param sigma Optional parameter for weighting function
 * @returns Filtered image
 */
cv::Mat_<float> nlmFilter(const cv::Mat_<float>& src, int searchSize, double sigma)
{
    return src.clone();
}



/**
 * @brief Chooses the right algorithm for the given noise type
 * @note: Figure out what kind of noise NOISE_TYPE_1 and NOISE_TYPE_2 are and select the respective "right" algorithms.
 */
NoiseReductionAlgorithm chooseBestAlgorithm(NoiseType noiseType)
{
    // TO DO !!
    return (NoiseReductionAlgorithm) -1;
}



cv::Mat_<float> denoiseImage(const cv::Mat_<float> &src, NoiseType noiseType, dip2::NoiseReductionAlgorithm noiseReductionAlgorithm)
{
    // TO DO !!

    // for each combination find reasonable filter parameters

    switch (noiseReductionAlgorithm) {
        case dip2::NR_MOVING_AVERAGE_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    return dip2::averageFilter(src, 1);
                case NOISE_TYPE_2:
                    return dip2::averageFilter(src, 1);
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_MEDIAN_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    return dip2::medianFilter(src, 1);
                case NOISE_TYPE_2:
                    return dip2::medianFilter(src, 1);
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_BILATERAL_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    return dip2::bilateralFilter(src, 1, 1.0f, 1.0f);
                case NOISE_TYPE_2:
                    return dip2::bilateralFilter(src, 1, 1.0f, 1.0f);
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        default:
            throw std::runtime_error("Unhandled filter type!");
    }
}





// Helpers, don't mind these

const char *noiseTypeNames[NUM_NOISE_TYPES] = {
    "NOISE_TYPE_1",
    "NOISE_TYPE_2",
};

const char *noiseReductionAlgorithmNames[NUM_FILTERS] = {
    "NR_MOVING_AVERAGE_FILTER",
    "NR_MEDIAN_FILTER",
    "NR_BILATERAL_FILTER",
};


}
