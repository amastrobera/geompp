#include "view2d.hpp"

#include "geompp_log.hpp"

#include <exception>

namespace geompp {

double View2D::x(Point3D const& p) const {
  switch (TYPE) {
    case ProjectionType::XY:
      return p.x();

    case ProjectionType::YZ:
      return p.y();

    case ProjectionType::ZX:
      return p.z();

    case ProjectionType::Custom:
      return (p - ORIGIN)
          .Dot(AXIS_U);  // I could have used PLANE.ProjectInto(), but I need one for each getter x() and
                         // x() - which would have been slower, so I re-wrote the ProjectInto() formula here.
                         // -> Object Oriented Programming lovers don't have me.
                         // -> Also, LTO (link time optiomization) is enabled, and will recognize (p-Origin) as a common
                         //    sub-expression when calling x() and y() separately, thus solving our performance issue
                         // -> Also, consider xy()->(double, double) an alternative if you need both coordinates but
                         //    don't want the overhead of a Point2D
  }

  throw std::runtime_error("invalid ProjectionType, cannot get x()");
}

double View2D::y(Point3D const& p) const {
  switch (TYPE) {
    case ProjectionType::XY:
      return p.y();

    case ProjectionType::YZ:
      return p.z();

    case ProjectionType::ZX:
      return p.x();

    case ProjectionType::Custom: {
      return (p - ORIGIN).Dot(AXIS_V);  // please read the comment I made in x() function (above)
    }
  }

  throw std::runtime_error("invalid ProjectionType, cannot get y()");
}

std::pair<double, double> View2D::xy(Point3D const& p) const {
  switch (TYPE) {
    case ProjectionType::XY:
      return {p.x(), p.y()};

    case ProjectionType::YZ:
      return {p.y(), p.z()};

    case ProjectionType::ZX:
      return {p.z(), p.x()};

    case ProjectionType::Custom: {
      auto dp = p - ORIGIN;
      return {dp.Dot(AXIS_U), dp.Dot(AXIS_V)};
    }
  }

  throw std::runtime_error("invalid ProjectionType, cannot project xy()");
}

Point3D View2D::xyz(double x, double y) const {
  switch (TYPE) {
    case ProjectionType::XY:
      return Point3D(x, y, PLANE_OFFSET);

    case ProjectionType::YZ:
      return Point3D(PLANE_OFFSET, x, y);

    case ProjectionType::ZX:
      return Point3D(y, PLANE_OFFSET, x);

    case ProjectionType::Custom:
      return ORIGIN + AXIS_U * x + AXIS_V * y;
  }

  throw std::runtime_error("invalid ProjectionType, cannot compute xyz()");
}

Point3D View2D::xyz(Point2D const& p) const { return xyz(p.x(), p.y()); }

}  // namespace geompp
