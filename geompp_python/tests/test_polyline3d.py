"""
polyline3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPolyline3D:
    def test_length(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(0, 0, 5)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.length(), 5.0)

    def test_to_segments(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        segs = pl.to_segments()
        assert len(segs) == 2  # 3 knots → 2 segments (open)
        assert all(isinstance(s, geompp.LineSegment3D) for s in segs)
        assert approx(segs[0].length(), 3.0)
        assert approx(segs[1].length(), 4.0)
        assert segs[0].first.almost_equals(geompp.Point3D(0, 0, 0))
        assert segs[0].last.almost_equals(geompp.Point3D(3, 0, 0))

    def test_contains(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.contains(geompp.Point3D(1,0,0))
        assert not pl.contains(geompp.Point3D(1,1,0))

    def test_location(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.location(geompp.Point3D(0,0,0)), 0.0)
        assert approx(pl.location(geompp.Point3D(3,4,0)), 1.0)
        assert math.isinf(pl.location(geompp.Point3D(1,1,0)))

    def test_distance_to(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert approx(pl.distance_to(geompp.Point3D(0,0,0)), 0.0)
        assert approx(pl.distance_to(geompp.Point3D(1,0,0)), 0.0)
        assert approx(pl.distance_to(geompp.Point3D(0,1,0)), 1.0)

    def test_interpolate(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.interpolate(0.0).almost_equals(geompp.Point3D(0,0,0))
        assert pl.interpolate(1.0).almost_equals(geompp.Point3D(3,4,0))
        with pytest.raises(Exception):
            pl.interpolate(1.2)
        with pytest.raises(Exception):
            pl.interpolate(-0.1)

    def test_project_onto(self):
        pts = [geompp.Point3D(0,0,0), geompp.Point3D(3,0,0), geompp.Point3D(3,4,0)]
        pl = geompp.Polyline3D.make(pts)
        assert pl.project_onto(geompp.Point3D(2,3,0)).almost_equals(geompp.Point3D(3,3,0))
        assert pl.project_onto(geompp.Point3D(6,2,0)).almost_equals(geompp.Point3D(3,2,0))

    def test_almost_equals(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl1 = geompp.Polyline3D.make(pts)
        pl2 = geompp.Polyline3D.make(pts)
        assert pl1.almost_equals(pl2)
        other = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0)])
        assert not pl1.almost_equals(other)

    def test_wkt_roundtrip(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        wkt = pl.to_wkt()
        pl2 = geompp.Polyline3D.from_wkt(wkt)
        assert pl.almost_equals(pl2)

    def test_from_wkt_whitespace(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0), geompp.Point3D(1, 1, 0)])
        assert pl.almost_equals(geompp.Polyline3D.from_wkt("LINESTRING (0 0 0, 1 0 0, 1 1 0)"))
        assert pl.almost_equals(geompp.Polyline3D.from_wkt("LINESTRING (  0 0 0  ,  1 0 0  ,  1 1 0  )"))

    def test_to_file_from_file(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            pl.to_file(path)
            assert os.path.exists(path)
            pl2 = geompp.Polyline3D.from_file(path)
            assert pl.almost_equals(pl2)
        finally:
            os.unlink(path)

    def test_len_and_getitem(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(3, 4, 0)]
        pl = geompp.Polyline3D.make(pts)
        assert len(pl) == 3
        assert pl[0].almost_equals(geompp.Point3D(0, 0, 0))
        assert pl[2].almost_equals(geompp.Point3D(3, 4, 0))
        assert pl[-1].almost_equals(geompp.Point3D(3, 4, 0))
        with pytest.raises(IndexError):
            _ = pl[10]

    def test_intersects_line3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        l_hit = geompp.Line3D.make(geompp.Point3D(2, -1, 0), geompp.Point3D(2, 1, 0))
        assert pl.intersects(l_hit)
        hit = pl.intersection(l_hit)
        assert hit is not None
        l_miss = geompp.Line3D.make(geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0))
        assert not pl.intersects(l_miss)
        assert pl.intersection(l_miss) is None

    def test_intersects_ray3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        r_hit = geompp.Ray3D.make(geompp.Point3D(2, -3, 0), geompp.Vector3D(0, 1, 0))
        assert pl.intersects(r_hit)
        hit = pl.intersection(r_hit)
        assert hit is not None
        r_miss = geompp.Ray3D.make(geompp.Point3D(2, 3, 0), geompp.Vector3D(0, 1, 0))
        assert not pl.intersects(r_miss)
        assert pl.intersection(r_miss) is None

    def test_intersects_segment3d(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)]
        pl = geompp.Polyline3D.make(pts)
        s_hit = geompp.LineSegment3D.make(geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0))
        assert pl.intersects(s_hit)
        hit = pl.intersection(s_hit)
        assert hit is not None
        s_miss = geompp.LineSegment3D.make(geompp.Point3D(5, -2, 0), geompp.Point3D(5, 2, 0))
        assert not pl.intersects(s_miss)
        assert pl.intersection(s_miss) is None

    def test_intersects_polyline3d(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        pl2_hit = geompp.Polyline3D.make([geompp.Point3D(2, -2, 0), geompp.Point3D(2, 2, 0)])
        assert pl1.intersects(pl2_hit)
        hit = pl1.intersection(pl2_hit)
        assert hit is not None
        pl2_miss = geompp.Polyline3D.make([geompp.Point3D(5, -2, 0), geompp.Point3D(5, 2, 0)])
        assert not pl1.intersects(pl2_miss)
        assert pl1.intersection(pl2_miss) is None

class TestPolyline3DOverlap:
    def test_overlap_line3d_collinear_segment(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(4, 0, 3)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert pl.overlaps(line=line)
        result = pl.overlap(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)))

    def test_overlap_ray3d_partial(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0)])
        ray = geompp.Ray3D.make(geompp.Point3D(2, 0, 0), geompp.Vector3D(1, 0, 0))
        assert pl.overlaps(ray=ray)
        result = pl.overlap(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(2, 0, 0), geompp.Point3D(6, 0, 0)))

    def test_overlap_segment3d_partial(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0)])
        seg = geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(7, 0, 0))
        assert pl.overlaps(segment=seg)
        result = pl.overlap(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.LineSegment3D.make(geompp.Point3D(3, 0, 0), geompp.Point3D(5, 0, 0)))

    def test_overlap_polyline3d_no_overlap(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(0, 1, 0), geompp.Point3D(4, 1, 0)])
        assert not pl1.overlaps(other=pl2)
        assert pl1.overlap(other=pl2) is None

class TestPolyline3DTouch:
    def test_touch_line3d_endpoint_on_line(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(2, 0, 0), geompp.Point3D(2, 0, 3)])
        line = geompp.Line3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0))
        assert pl.touches(line=line)
        result = pl.touch(line=line)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(2, 0, 0))

    def test_touch_ray3d_endpoint_on_ray(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 2)])
        ray = geompp.Ray3D.make(geompp.Point3D(0, 0, 0), geompp.Vector3D(1, 0, 0))
        assert pl.touches(ray=ray)
        result = pl.touch(ray=ray)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_segment3d_t_junction(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3)])
        seg = geompp.LineSegment3D.make(geompp.Point3D(0, 0, 0), geompp.Point3D(5, 0, 0))
        assert pl.touches(segment=seg)
        result = pl.touch(segment=seg)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_polyline3d_shared_endpoint(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(3, 0, 0), geompp.Point3D(3, 0, 3)])
        assert pl1.touches(other=pl2)
        result = pl1.touch(other=pl2)
        assert result is not None
        assert len(result) == 1
        assert result[0].almost_equals(geompp.Point3D(3, 0, 0))

    def test_touch_polyline3d_disjoint(self):
        pl1 = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0)])
        pl2 = geompp.Polyline3D.make([geompp.Point3D(5, 0, 0), geompp.Point3D(5, 0, 3)])
        assert not pl1.touches(other=pl2)
        assert pl1.touch(other=pl2) is None

class TestPolyline3DPlanarConvex:
    def test_is_planar_xy(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(0,1,0)])
        assert pl.is_planar()

    def test_is_planar_nonplanar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        assert not pl.is_planar()

    def test_is_simple_planar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        assert pl.is_simple()

    def test_is_convex_planar(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        assert pl.is_convex()

    def test_is_convex_not_planar_throws(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        with pytest.raises(Exception):
            pl.is_convex()

    def test_convex_hull_returns_polyline(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        hull = pl.convex_hull()
        assert isinstance(hull, geompp.Polyline3D)

    def test_convex_hull_to_polygon(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(2,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(2,2,0), geompp.Point3D(0,2,0)])
        polygon = pl.convex_hull().to_polygon()
        assert isinstance(polygon, geompp.Polygon3D)

    def test_to_polygon_not_planar_throws(self):
        pl = geompp.Polyline3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,1), geompp.Point3D(0,1,2)])
        with pytest.raises(Exception):
            pl.to_polygon()

class TestPolyline3DReduce:
    def test_two_point_polyline_returns_unchanged(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(1, 1, 1)])
        assert pl.almost_equals(pl.reduce())

    def test_radial_distance(self):
        # Note: a "peak" shape (not a straight line) is deliberate — Polyline3D.make() prunes exactly
        # collinear knots at construction time, so a flat clustered dataset would collapse to its 2
        # endpoints regardless of what reduce() does, defeating the test.
        pl = geompp.Polyline3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(0.1, 0, 0.05), geompp.Point3D(0.2, 0, -0.05),
             geompp.Point3D(5, 0, 5), geompp.Point3D(5.1, 0, 5.05), geompp.Point3D(10, 0, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.RadialDistance, 1.0))
        assert reduced.size() == 3
        assert reduced[0].almost_equals(geompp.Point3D(0, 0, 0))
        assert reduced[1].almost_equals(geompp.Point3D(5, 0, 5))
        assert reduced[2].almost_equals(geompp.Point3D(10, 0, 0))

    def test_ramer_douglas_peucker(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 5),
                                     geompp.Point3D(6, 0, 0), geompp.Point3D(8, 0, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.RamerDouglasPeucker, 2.0))
        expected = geompp.Polyline3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 5), geompp.Point3D(8, 0, 0)])
        assert reduced.almost_equals(expected)

    def test_visvalingam_whyatt(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(4, 0, 5),
                                     geompp.Point3D(6, 0, 0), geompp.Point3D(8, 0, 0)])
        reduced = pl.reduce(geompp.PolylineDecimationParams(geompp.PolylineDecimationStrategy.VisvalingamWhyatt, 6.0))
        expected = geompp.Polyline3D.make(
            [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 5), geompp.Point3D(8, 0, 0)])
        assert reduced.almost_equals(expected)

class TestPolyline3DExpand:
    def test_default_params_work(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(2, 2, 0)])
        expanded = pl.expand()
        assert expanded.size() >= pl.size()

    def test_invalid_segments_per_corner_raises(self):
        pl = geompp.Polyline3D.make([geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0), geompp.Point3D(2, 2, 0)])
        with pytest.raises(ValueError):
            pl.expand(geompp.PolylineExpansionParams(segments_per_corner=0))
