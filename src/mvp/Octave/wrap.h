/// \file wrap.h
///
/// List all the wraps available.
///
/// These declarations are needed to avoid self-immolation:
/// http://stackoverflow.com/questions/15125316/
///     do-all-compilers-ignore-generated-template-code-when-explicit-specializations
///

#ifndef __MVP_OCTAVE_WRAP_H__
#define __MVP_OCTAVE_WRAP_H__

#include <vw/Core/Exception.h>

#define DECLARE_MVP_WRAPPER(IMPLT) template <> \
  octave_value mvp_wrapper<IMPLT>(IMPLT *impl, std::string const& func, octave_value_list const& args);

#define DECLARE_OCTAVE_WRAPPER(IMPLT) template <> \
  IMPLT octave_wrapper<IMPLT>(octave_value const& impl);

template <class ImplT>
octave_value mvp_wrapper(ImplT *impl, std::string const& func, octave_value_list const& args) {
  vw::vw_throw(vw::LogicErr() << "No mvp wrap defined for type");
}

template <class ImplT>
ImplT octave_wrapper(octave_value const& impl) {
  vw::vw_throw(vw::LogicErr() << "No octave wrap defined for type");
}

/// Forward declarations

namespace mvp {
  namespace algorithm {
    class Dummy;
    class Stepper;
    class Seeder;
  }
  namespace image {
    class OrbitalImageCollection;
  }
}

namespace vw {
  namespace cartography {
    class Datum;
    class GeoReference;
  }
}

/// Wrapper declarations

DECLARE_MVP_WRAPPER(mvp::algorithm::Dummy)
DECLARE_MVP_WRAPPER(mvp::image::OrbitalImageCollection)
DECLARE_MVP_WRAPPER(vw::cartography::Datum)
DECLARE_MVP_WRAPPER(vw::cartography::GeoReference)

DECLARE_OCTAVE_WRAPPER(mvp::algorithm::Dummy)

#endif
