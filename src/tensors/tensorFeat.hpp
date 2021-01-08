#ifndef _TENSOR_FEAT_HPP
#define _TENSOR_FEAT_HPP

#ifdef HAVE_CONFIG_H
# include "config.hpp"
#endif

/// \file tensorFeat.hpp
///
/// \brief Implements the feature to detect a tensor

#include <metaProgramming/feature.hpp>

namespace maze
{
  DEFINE_FEATURE(IsTensor);
  
  DEFINE_FEATURE_GROUP(TensorFeat);
}

#endif
