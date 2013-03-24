// Copyright (C) 2011  Davis E. King (davis@dlib.net)
// License: Boost Software License   See LICENSE.txt for the full license.
#ifndef DLIB_SETUP_HAShED_FEATURES_H__
#define DLIB_SETUP_HAShED_FEATURES_H__

#include "setup_hashed_features_abstract.h"
#include "scan_image_pyramid.h"
#include "scan_image_boxes.h"
#include "../lsh.h"
#include "../statistics.h"
#include "../image_keypoint.h"
#include "../geometry.h"

namespace dlib
{

// ----------------------------------------------------------------------------------------

    class image_hash_construction_failure : public error
    {
    public:
        image_hash_construction_failure(
            const std::string& a
        ): error(a) {}
    };

// ----------------------------------------------------------------------------------------

    template <
        typename image_array,
        typename pyramid,
        typename feature_extractor
        >
    void setup_hashed_features (
        scan_image_pyramid<pyramid, hashed_feature_image<feature_extractor, projection_hash> >& scanner,
        const image_array& images,
        const feature_extractor& fe,
        int bits,
        unsigned long num_samples = 200000
    )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(0 < bits && bits <= 32 &&
                    num_samples > 1 && 
                    images.size() > 0,
            "\t void setup_hashed_features()"
            << "\n\t Invalid inputs were given to this function. "
            << "\n\t bits:          " << bits 
            << "\n\t num_samples:   " << num_samples 
            << "\n\t images.size(): " << images.size() 
            );

        pyramid pyr;

        const random_subset_selector<typename feature_extractor::descriptor_type>& samps = 
            randomly_sample_image_features(images, pyr, fe, num_samples);

        if (samps.size() <= 1)
            throw dlib::image_hash_construction_failure("Images too small, not able to gather enough samples to make hash");

        projection_hash phash = create_random_projection_hash(samps, bits);

        hashed_feature_image<feature_extractor, projection_hash> hfe;
        hfe.set_hash(phash);
        hfe.copy_configuration(fe);
        scanner.copy_configuration(hfe);
    }

// ----------------------------------------------------------------------------------------

    template <
        typename image_array,
        typename pyramid,
        typename feature_extractor
        >
    void setup_hashed_features (
        scan_image_pyramid<pyramid, hashed_feature_image<feature_extractor, projection_hash> >& scanner,
        const image_array& images,
        int bits,
        unsigned long num_samples = 200000
    )
    {
        // make sure requires clause is not broken
        DLIB_ASSERT(0 < bits && bits <= 32 &&
                    num_samples > 1 && 
                    images.size() > 0,
            "\t void setup_hashed_features()"
            << "\n\t Invalid inputs were given to this function. "
            << "\n\t bits:          " << bits 
            << "\n\t num_samples:   " << num_samples 
            << "\n\t images.size(): " << images.size() 
            );

        feature_extractor fe;
        setup_hashed_features(scanner, images, fe, bits, num_samples);
    }

// ----------------------------------------------------------------------------------------

}

#endif // DLIB_SETUP_HAShED_FEATURES_H__


