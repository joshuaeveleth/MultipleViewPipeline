#include <mvp/MVPWorkspace.h>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

std::vector<std::string> paths_from_pattern(boost::filesystem::path pattern) {
  namespace fs = boost::filesystem;

  fs::path dir(pattern.parent_path());
  boost::regex filter(pattern.filename().string());

  std::vector<std::string> result_paths;

  fs::directory_iterator end_itr;
  for(fs::directory_iterator i(dir); i != end_itr; i++) {
    if (!fs::is_regular_file(i->status())) {
      continue;
    }

    boost::smatch what;
    if (boost::regex_match(i->path().filename().string(), what, filter)) {
      result_paths.push_back(i->path().string());
    }  
  }

  sort(result_paths.begin(), result_paths.end());

  return result_paths;
}

namespace mvp {

#define PROPERTY_TO_PROTO(ptree, protobuf, type, proto_name, prop_name) \
{boost::optional<type> val = ptree.get_optional<type>(prop_name); \
if (val) (protobuf).set_##proto_name(*val);}

MVPWorkspaceRequest MVPWorkspace::load_workspace_request(std::string const& filename) {
  using boost::property_tree::ptree;
  ptree pt;

  read_ini(filename, pt);

  MVPWorkspaceRequest work_request;

  PROPERTY_TO_PROTO(pt, work_request, std::string, result_platefile, "output.result-platefile");
  PROPERTY_TO_PROTO(pt, work_request, std::string, internal_result_platefile, "output.internal-result-platefile");

  vw::cartography::Datum datum(pt.get<std::string>("platefile.datum"));
  vw::platefile::PlateGeoReference plate_georef(datum, pt.get<std::string>("platefile.map-projection"), pt.get<int>("platefile.tile-size"),
                                                vw::cartography::GeoReference::PixelAsPoint);
  *work_request.mutable_plate_georef() = plate_georef.build_desc(); 

  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, alt_min, "algorithm.alt-min");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, alt_max, "algorithm.alt-max");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, alt_search_range, "algorithm.alt-search-range");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, seed_window_smooth_size, "algorithm.seed-window-smooth-size");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, window_size, "algorithm.window-size");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, window_smooth_size, "algorithm.window-smooth-size");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), double, gauss_divisor, "algorithm.gauss-divisor");
  PROPERTY_TO_PROTO(pt, *work_request.mutable_user_settings(), int, max_iterations, "algorithm.max-iterations");

  // orbital_images
  {
    std::vector<std::string> image_paths = paths_from_pattern(pt.get<std::string>("dataset.orbital-image-pattern"));
    std::vector<std::string> camera_paths = paths_from_pattern(pt.get<std::string>("dataset.camera-pattern"));

    if (image_paths.size() != camera_paths.size()) {
      vw::vw_throw(vw::InputErr() << "The number of orbital images does not match the number of cameras");
    }

    std::vector<std::string>::const_iterator image_iter, camera_iter;
    for (image_iter = image_paths.begin(), camera_iter = camera_paths.begin();
         image_iter != image_paths.end();
         image_iter++, camera_iter++) {
      OrbitalImageFileDescriptor image;
      image.set_image_path(*image_iter);
      image.set_camera_path(*camera_iter);
      *work_request.add_orbital_images() = image;
    }
  }

  // render_bbox
  boost::optional<std::string> render_bbox_string = pt.get_optional<std::string>("render.render-bbox");
  if (render_bbox_string) {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",:");

    tokenizer tokens(*render_bbox_string, sep);

    for (tokenizer::iterator tok_iter = tokens.begin();
         tok_iter != tokens.end(); tok_iter++) {
      work_request.add_render_bbox(boost::lexical_cast<int>(*tok_iter));
    }
  }

  PROPERTY_TO_PROTO(pt, work_request, int, render_level, "render.render-level");
  PROPERTY_TO_PROTO(pt, work_request, bool, use_octave, "render.use-octave");
  PROPERTY_TO_PROTO(pt, work_request, bool, draw_footprints, "render.draw-footprints");

  std::cout << work_request.DebugString() << std::endl;

  return work_request;
}

} // namespace mvp
