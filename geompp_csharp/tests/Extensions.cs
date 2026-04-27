using GeomPP;

namespace Geompp.Extensions {

public static class Point3DListExtensions {
    public static bool AreCoplanar(this List<Point3D> points) =>
        GeomUtil.AreCoplanar(points);

    public static Plane ClosestWorldPlaneTo(this List<Point3D> points) =>
        GeomUtil.ClosestWorldPlaneTo(points);

    /// <summary>refPlane = null → closest world plane is auto-detected from the points.</summary>
    public static bool AreCCW(this List<Point3D> points, Plane? refPlane = null) =>
        GeomUtil.AreCCW(points, refPlane);

    /// <summary>refPlane = null → closest world plane is auto-detected from the points.</summary>
    public static bool AreCW(this List<Point3D> points, Plane? refPlane = null) =>
        GeomUtil.AreCW(points, refPlane);
}

}  // namespace Geompp.Extensions
