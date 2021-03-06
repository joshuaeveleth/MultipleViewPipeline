#include <mvp/ConvexPolygon.h>

#include <boost/foreach.hpp>

namespace mvp {

ConvexPolygon::ConvexPolygon(VertexList pts) {
  // Algorithm is the 'giftwrap' algorithm http://www.cse.unsw.edu.au/~lambert/java/3d/giftwrap.html
  // Probably not the most efficient implementation...
  VW_ASSERT(pts.size() >= 3, vw::ArgumentErr() << "Need at least 3 points to construct a polygon!");

  vw::Vector2 end = pts[0];
  BOOST_FOREACH(vw::Vector2 const& v, pts) {
    if (end.y() < v.y()) {
      end = v;
    }
  }

  bool done = false;
  vw::Vector2 curr = end;

  while (!done) {
    bool found_next = false;
    BOOST_FOREACH(vw::Vector2 const& next, pts) {
      if (curr != next) {
        found_next = true;
        BOOST_FOREACH(vw::Vector2 const& pt, pts) {
          if (circulation_direction(curr, next, pt) > 0 && pt != next) {
            found_next = false;
            break;
          }
        }
        if (found_next) {
          curr = next;
          break;
        }
      }
    }

    VW_ASSERT(found_next, vw::LogicErr() << "Unable to construct convex hull");

    m_vertices.push_back(curr);
    if (curr == end) {
      done = true;
    }
  }
}

vw::BBox2 ConvexPolygon::bounding_box() const {
  vw::BBox2 bbox;

  BOOST_FOREACH(vw::Vector2 const& v, m_vertices) {
    bbox.grow(v);
  }

  return bbox;
}

bool ConvexPolygon::contains(vw::Vector2 const& pt) const {
  // Use solution 3 from http://paulbourke.net/geometry/insidepoly/
  for (VertexList::const_iterator curr = m_vertices.begin(); curr != m_vertices.end(); curr++) {
    VertexList::const_iterator next = curr;
    if (++next == m_vertices.end()) {
      next = m_vertices.begin();
    }

    double circulation = circulation_direction(*curr, *next, pt);

    // Round to zero if close... (assume colinear)
    circulation = circulation * circulation < 1e-6 ? 0 : circulation;

    if (circulation > 0) {
      return false;
    }
  }

  return true;
}

bool ConvexPolygon::intersects(ConvexPolygon const& other) const {
  // Algorithm from http://www.gpwiki.org/index.php/Polygon_Collision
  // Straight up dumb check, no optimizations attempted...

  for (VertexList::const_iterator cursor = m_vertices.begin(); cursor != m_vertices.end(); cursor++) {
    VertexList::const_iterator next = cursor;
    if (++next == m_vertices.end()) {
      next = m_vertices.begin();
    }

    vw::Vector2 dir = *next - *cursor;
    vw::Vector2 perp_dir(-dir[1], dir[0]);

    double poly1_min = std::numeric_limits<double>::max();
    double poly1_max = std::numeric_limits<double>::min();
    BOOST_FOREACH(vw::Vector2 const& v, m_vertices) {
      double res = dot_prod(perp_dir, v);
      poly1_min = std::min(res, poly1_min);
      poly1_max = std::max(res, poly1_max);
    }

    double poly2_min = std::numeric_limits<double>::max();
    double poly2_max = std::numeric_limits<double>::min();
    BOOST_FOREACH(vw::Vector2 const& v, other.m_vertices) {
      double res = dot_prod(perp_dir, v);
      poly2_min = std::min(res, poly2_min);
      poly2_max = std::max(res, poly2_max);
    }

    if (poly1_min > poly2_max || poly1_max < poly2_min) {
      return false;
    }
  }

  return true;  
}

bool ConvexPolygon::intersects(vw::BBox2 const& bbox) const {
  ConvexPolygon bbox_poly;
  
  std::vector<vw::Vector2> bbox_verts(4);
  bbox_verts[0] = bbox.min();
  bbox_verts[1] = vw::Vector2(bbox.min().x(), bbox.max().y());
  bbox_verts[2] = bbox.max();
  bbox_verts[3] = vw::Vector2(bbox.max().x(), bbox.min().y());

  bbox_poly.m_vertices = bbox_verts;

  return intersects(bbox_poly);
}

} // namespace mvp
