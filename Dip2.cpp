#include "Dip2.h"
#include <opencv2/opencv.hpp>

namespace dip2 {


cv::Mat_<float> spatialConvolution(const cv::Mat_<float>& src, const cv::Mat_<float>& kernel) {
    // Calculate padding size
    int padSize = kernel.rows / 2;

    // Create a padded version of the image
    cv::Mat_<float> paddedSrc;
    cv::copyMakeBorder(src, paddedSrc, padSize, padSize, padSize, padSize, cv::BORDER_CONSTANT, 0);

    // Create an output image initialized with zeros
    cv::Mat_<float> output = cv::Mat::zeros(src.size(), CV_32FC1);

    // Perform convolution
    for (int y = padSize; y < paddedSrc.rows - padSize; ++y) {
        for (int x = padSize; x < paddedSrc.cols - padSize; ++x) {
            float sum = 0.0;
            for (int ky = -padSize; ky <= padSize; ++ky) {
                for (int kx = -padSize; kx <= padSize; ++kx) {
                    // Multiply kernel value with corresponding image pixel and accumulate
                    sum += kernel.at<float>(padSize + ky, padSize + kx) * paddedSrc.at<float>(y + ky, x + kx);
                }
            }
            // Assign computed sum to the corresponding pixel in the output image
            output.at<float>(y - padSize, x - padSize) = sum;
        }
    }

    return output;
}


cv::Mat_<float> averageFilter(const cv::Mat_<float>& src, int kSize) {
    // Create an averaging kernel
    cv::Mat_<float> kernel = cv::Mat::ones(kSize, kSize, CV_32F) / static_cast<float>(kSize * kSize);

    // Use the spatialConvolution function
    return dip2::spatialConvolution(src, kernel);
}


cv::Mat_<float> medianFilter(const cv::Mat_<float>& src, int kSize) {
    // Ensure kSize is odd and greater than 1
    if (kSize % 2 == 0 || kSize < 3) {
        std::cerr << "Kernel size should be odd and greater than 1" << std::endl;
        return src.clone();
    }

    // Create a padded version of the image
    cv::Mat_<float> paddedSrc;
    int padSize = kSize / 2;
    cv::copyMakeBorder(src, paddedSrc, padSize, padSize, padSize, padSize, cv::BORDER_REPLICATE);

    // Create an output image
    cv::Mat_<float> output = src.clone(); // Clone src for output dimensions

    // Apply the median filter
    for (int y = padSize; y < paddedSrc.rows - padSize; y++) {
        for (int x = padSize; x < paddedSrc.cols - padSize; x++) {
            std::vector<float> window;
            for (int ky = -padSize; ky <= padSize; ky++) {
                for (int kx = -padSize; kx <= padSize; kx++) {
                    window.push_back(paddedSrc.at<float>(y + ky, x + kx));
                }
            }
            std::nth_element(window.begin(), window.begin() + window.size() / 2, window.end());
            output.at<float>(y - padSize, x - padSize) = window[window.size() / 2];
        }
    }

    return output;
}



cv::Mat_<float> bilateralFilter(const cv::Mat_<float>& src, int kSize, float sigma_spatial, float sigma_radiometric) {
    // Ensure kernel size is odd
    if (kSize % 2 == 0) kSize++;

    int d = kSize / 2;
    cv::Mat_<float> output(src.size(), CV_32FC1);
    
    // Iterate through each pixel
    for (int i = d; i < src.rows - d; i++) {
        for (int j = d; j < src.cols - d; j++) {
            float sum = 0.0f;
            float norm = 0.0f;

            // Apply the filter
            for (int k = -d; k <= d; k++) {
                for (int l = -d; l <= d; l++) {
                    float spatial = exp(-(k * k + l * l) / (2 * sigma_spatial * sigma_spatial));
                    float radiometric = exp(-pow(src.at<float>(i + k, j + l) - src.at<float>(i, j), 2) / (2 * sigma_radiometric * sigma_radiometric));
                    float weight = spatial * radiometric;
                    sum += src.at<float>(i + k, j + l) * weight;
                    norm += weight;
                }
            }

            output.at<float>(i, j) = sum / norm;
        }
    }

    return output;
}


/**
 * @brief Non-local means filter
 * @param src Input image
 * @param searchSize Size of search region
 * @param sigma Optional parameter for weighting function
 * @returns Filtered image
 */
cv::Mat_<float> nlmFilter(const cv::Mat_<float>& src, int searchSize, double sigma)
{
    return src.clone();
}



dip2::NoiseReductionAlgorithm chooseBestAlgorithm(dip2::NoiseType noiseType) {
    switch (noiseType) {
        case dip2::NOISE_TYPE_1:
            // Use median filter for salt-and-pepper noise
            return dip2::NR_MEDIAN_FILTER;
        case dip2::NOISE_TYPE_2:
            // Use bilateral filter for Gaussian noise
            return dip2::NR_BILATERAL_FILTER;
        default:
            throw std::runtime_error("Unhandled noise type!");
    }
}




cv::Mat_<float> denoiseImage(const cv::Mat_<float> &src, dip2::NoiseType noiseType, dip2::NoiseReductionAlgorithm noiseReductionAlgorithm) {
    switch (noiseReductionAlgorithm) {
        case dip2::NR_MOVING_AVERAGE_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    return dip2::averageFilter(src, 3); // Adjusted kernel size
                case NOISE_TYPE_2:
                    return dip2::averageFilter(src, 5); // Adjusted kernel size
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_MEDIAN_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    return dip2::medianFilter(src, 3); // Adjusted kernel size
                case NOISE_TYPE_2:
                    return dip2::medianFilter(src, 5); // Adjusted kernel size
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        case dip2::NR_BILATERAL_FILTER:
            switch (noiseType) {
                case NOISE_TYPE_1:
                    // Adjusted kernel size, sigmaSpatial, and sigmaRadiometric
                    return dip2::bilateralFilter(src, 5, 15.0f, 75.0f);
                case NOISE_TYPE_2:
                    // Adjusted kernel size, sigmaSpatial, and sigmaRadiometric
                    return dip2::bilateralFilter(src, 7, 30.0f, 150.0f);
                default:
                    throw std::runtime_error("Unhandled noise type!");
            }
        default:
            throw std::runtime_error("Unhandled filter type!");
    }
}






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
