//============================================================================
// Name        : Dip2.cpp
// Author      : Ronny Haensch
// Version     : 2.0
// Copyright   : -
// Description :
//============================================================================

#include "Dip2.h"

namespace dip2
{

    /**
     * @brief Convolution in spatial domain.
     * @details Performs spatial convolution of image and filter kernel.
     * @params src Input image
     * @params kernel Filter kernel
     * @returns Convolution result
     */
    cv::Mat_<float> spatialConvolution(const cv::Mat_<float> &src, const cv::Mat_<float> &kernel)
    {
        cv::Mat result = src.clone();
        int borderSize = 0;
        // pls only use odd kernels and kernals > 1x1
        if (kernel.rows <= 1 && kernel.cols <= 1)
        {
            throw std::runtime_error("Kernel size must be greater than 1 and of odd size");
        }
        else
        {
            int borderSize = (kernel.rows - 1) / 2;
            // Add the border to the result image
            cv::copyMakeBorder(result, result, borderSize, borderSize, borderSize, borderSize, cv::BORDER_REPLICATE);
            std::cout << result.rows << " " << result.cols << std::endl;
            std::cout << src.rows << " " << src.cols << std::endl;
        }

        // rotate the kernel by 180 degrees
        cv::flip(kernel, kernel, -1);
        // std::cout << kernel << std::endl;

        // Perform the convolution operation
        for (int i = borderSize; i < src.rows + borderSize; i++)
        {
            for (int j = borderSize; j < src.cols + borderSize; j++)
            {
                float averagePixelValue = 0;
                for (int k = 0; k < kernel.rows; k++)
                {
                    for (int l = 0; l < kernel.cols; l++)
                    {
                        averagePixelValue += kernel.at<float>(k, l) * src.at<float>(i + k - borderSize, j + l - borderSize);
                    }
                }
                float &pixel = result.at<float>(i, j);
                // set the pixel at i,j to the average pixel value of the kernel
                pixel = averagePixelValue;
            }
        }

        // Remove the border from the result image
        result = result(cv::Rect(borderSize, borderSize, src.cols, src.rows));
        std::cout << result.rows << " " << result.cols << std::endl;
        return result;
    }

    /**
     * @brief Moving average filter (aka box filter)
     * @note: you might want to use Dip2::spatialConvolution(...) within this function
     * @param src Input image
     * @param kSize Window size used by local average
     * @returns Filtered image
     */
    cv::Mat_<float> averageFilter(const cv::Mat_<float> &src, int kSize)
    {
        cv::Mat kernel = cv::Mat::ones(kSize, kSize, CV_32FC1) / (float)(kSize * kSize); // Create a kernel with all elements equal to 1/(kSize*kSize)
        // std::cout << kernel << std::endl;
        return spatialConvolution(src, kernel);
    }

    /**
     * @brief Median filter
     * @param src Input image
     * @param kSize Window size used by median operation
     * @returns Filtered image
     */
    cv::Mat_<float> medianFilter(const cv::Mat_<float> &src, int kSize)
    {
        // TO DO !!
        cv::Mat kernel = cv::Mat::ones(kSize, kSize, CV_32FC1) / (float)(kSize * kSize); // Create a kernel with all elements equal to 1/(kSize*kSize)
        // std::cout << kernel << std::endl;
        return spatialConvolution(src, kernel);
    }

    /**
     * @brief Bilateral filer
     * @param src Input image
     * @param kSize Size of the kernel
     * @param sigma_spatial Standard-deviation of the spatial kernel
     * @param sigma_radiometric Standard-deviation of the radiometric kernel
     * @returns Filtered image
     */
    cv::Mat_<float> bilateralFilter(const cv::Mat_<float> &src, int kSize, float sigma_spatial, float sigma_radiometric)
    {
        cv::Mat kernel = cv::Mat::ones(kSize, kSize, CV_32FC1) / (float)(kSize * kSize); // Create a kernel with all elements equal to 1/(kSize*kSize)
        // std::cout << kernel << std::endl;
        return spatialConvolution(src, kernel);
    }

    /**
     * @brief Non-local means filter
     * @note: This one is optional!
     * @param src Input image
     * @param searchSize Size of search region
     * @param sigma Optional parameter for weighting function
     * @returns Filtered image
     */
    cv::Mat_<float> nlmFilter(const cv::Mat_<float> &src, int searchSize, double sigma)
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
        return (NoiseReductionAlgorithm)-1;
    }

    cv::Mat_<float> denoiseImage(const cv::Mat_<float> &src, NoiseType noiseType, dip2::NoiseReductionAlgorithm noiseReductionAlgorithm)
    {
        // TO DO !!

        // for each combination find reasonable filter parameters

        switch (noiseReductionAlgorithm)
        {
        case dip2::NR_MOVING_AVERAGE_FILTER:
            switch (noiseType)
            {
            case NOISE_TYPE_1:
                return dip2::averageFilter(src, 33);
            case NOISE_TYPE_2:
                return dip2::averageFilter(src, 33);
            default:
                throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_MEDIAN_FILTER:
            switch (noiseType)
            {
            case NOISE_TYPE_1:
                return dip2::medianFilter(src, 33);
            case NOISE_TYPE_2:
                return dip2::medianFilter(src, 33);
            default:
                throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_BILATERAL_FILTER:
            switch (noiseType)
            {
            case NOISE_TYPE_1:
                return dip2::bilateralFilter(src, 33, 1.0f, 1.0f);
            case NOISE_TYPE_2:
                return dip2::bilateralFilter(src, 33, 1.0f, 1.0f);
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
