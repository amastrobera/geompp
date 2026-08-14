"""
polygon3d binding tests.
"""

import math
import os
import tempfile
import pytest
import geompp

from ._helpers import approx


class TestPolygon3DIsSimple:
    # NOTE: is_simple() correctness rides on the (currently provisional) sweep-line comparator; these encode
    # the intended behaviour and should be re-verified once the real sweep-status ordering is in place.
    def test_is_simple_convex_square_xy_plane_true(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])
        assert p.is_simple()

    def test_is_simple_convex_square_yz_plane_true(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        assert p.is_simple()

    def test_is_simple_self_intersecting_false(self):
        # CCW (positive area) but edges (4,0,0)->(1,3,0) and (3,3,0)->(0,0,0) cross
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0),
        ])
        assert not p.is_simple()

class TestPolygon3D:
    def test_construction(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        assert len(p) == 4

    def test_cw_outer_auto_canonicalizes(self):
        # A lone outer ring has no externally meaningful CCW/CW of its own in 3D (unlike 2D, there's no
        # fixed "which side are you viewing from" convention) -- a "CW" (relative to +Z) ring is legitimate
        # input, auto-canonicalized to whichever plane normal makes the GIVEN order read as positive.
        poly = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(4, 0, 0),
        ])
        assert poly.area() == pytest.approx(16.0)

    def test_with_holes_valid(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole])
        assert len(p) == 4

    def test_with_holes_ccw_hole_throws(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        ccw_hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(3, 1, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(1, 3, 0),
        ]
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [ccw_hole])

    def test_with_holes_self_intersecting_hole_throws(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0),
            geompp.Point3D(6, 6, 0), geompp.Point3D(0, 6, 0),
        ]
        bowtie_hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(3, 1, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0),
        ]
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [bowtie_hole])

    def test_with_holes_two_holes_overlap_throws(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(10, 0, 0),
            geompp.Point3D(10, 10, 0), geompp.Point3D(0, 10, 0),
        ]
        hole_a = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 5, 0),
            geompp.Point3D(5, 5, 0), geompp.Point3D(5, 1, 0),
        ]
        hole_b = [
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 7, 0),
            geompp.Point3D(7, 7, 0), geompp.Point3D(7, 3, 0),
        ]  # overlaps hole_a
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [hole_a, hole_b])

    def test_with_holes_two_holes_touch_at_vertex_does_not_throw(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(10, 0, 0),
            geompp.Point3D(10, 10, 0), geompp.Point3D(0, 10, 0),
        ]
        hole_a = [
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 5, 0),
            geompp.Point3D(5, 5, 0), geompp.Point3D(5, 3, 0),
        ]
        hole_b = [
            geompp.Point3D(5, 5, 0), geompp.Point3D(5, 7, 0),
            geompp.Point3D(7, 7, 0), geompp.Point3D(7, 5, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole_a, hole_b])
        assert len(p.holes()) == 2

    def test_with_holes_hole_strikes_through_outer_throws(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(10, 0, 0),
            geompp.Point3D(10, 10, 0), geompp.Point3D(0, 10, 0),
        ]
        hole = [
            geompp.Point3D(8, 4, 0), geompp.Point3D(8, 6, 0),
            geompp.Point3D(12, 6, 0), geompp.Point3D(12, 4, 0),
        ]
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [hole])

    def test_with_holes_hole_flush_against_outer_edge_does_not_throw(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(2, 2, 0), geompp.Point3D(2, 4, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(4, 2, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole])
        assert len(p.holes()) == 1

    def test_with_holes_hole_entirely_outside_outer_throws(self):
        # Hole never crosses or touches the outer boundary (same plane, disjoint region) — must be
        # rejected by the containment check even though the strikes-through check alone lets it slip by.
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(10, 0, 0),
            geompp.Point3D(10, 10, 0), geompp.Point3D(0, 10, 0),
        ]
        hole = [
            geompp.Point3D(20, 20, 0), geompp.Point3D(20, 22, 0),
            geompp.Point3D(22, 22, 0), geompp.Point3D(22, 20, 0),
        ]
        with pytest.raises(Exception):
            geompp.Polygon3D.make(outer, [hole])

    def test_area_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).area(), 16.0)

    def test_area_with_hole(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        assert approx(geompp.Polygon3D.make(outer, [hole]).area(), 12.0)

    def test_area_self_intersecting_outer_in_xy_plane(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0),
        ])
        assert not p.is_simple()
        assert approx(p.area(), 5.0)

    def test_area_self_intersecting_outer_on_tilted_plane_matches_simplify_sum(self):
        # On a non-axis-aligned plane a naive 2D-projected shoelace on the decomposed loops would be
        # wrong (foreshortening) — cross-validated against simplify() + per-piece area() instead of
        # hand-deriving the tilted value.
        def tilt(x, y):
            return geompp.Point3D(x, y, 0.3 * x + 0.2 * y)

        p = geompp.Polygon3D.make([tilt(0, 0), tilt(4, 0), tilt(1, 3), tilt(3, 3)])
        assert not p.is_simple()
        simplify_total = sum(piece.area() for piece in p.simplify())
        assert approx(p.area(), simplify_total)

    def test_area_far_from_origin(self):
        ox, oy = 1e8, 1e8
        pts = [
            geompp.Point3D(ox,     oy,     0), geompp.Point3D(ox + 4, oy,     0),
            geompp.Point3D(ox + 4, oy + 4, 0), geompp.Point3D(ox,     oy + 4, 0),
        ]
        assert abs(geompp.Polygon3D.make(pts).area() - 16.0) < 1e-4

    def test_perimeter_square(self):
        # 1×1 square → perimeter = 4
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 4.0)

    def test_perimeter_non_xy_plane(self):
        # 1×1 square on YZ plane → same perimeter = 4
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 4.0)

    def test_perimeter_triangle(self):
        # 3-4-5 right triangle → perimeter = 12
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0), geompp.Point3D(0, 3, 0),
        ]
        assert approx(geompp.Polygon3D.make(pts).perimeter_size(), 12.0)

    def test_get_plane_returns_plane(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        pl = geompp.Polygon3D.make(pts).get_plane()
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_get_plane_contains_all_vertices(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        pl = p.get_plane()
        for i in range(len(p)):
            assert pl.contains(p[i])

    def test_get_plane_with_holes(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        p = geompp.Polygon3D.make(outer, [hole])
        pl = p.get_plane()
        assert isinstance(pl, geompp.Plane)
        assert approx(abs(pl.normal.z), 1.0)

    def test_centroid_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        c = geompp.Polygon3D.make(pts).centroid()
        assert isinstance(c, geompp.Point3D)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_centroid_elevated_square(self):
        pts = [
            geompp.Point3D(0, 0, 5), geompp.Point3D(4, 0, 5),
            geompp.Point3D(4, 4, 5), geompp.Point3D(0, 4, 5),
        ]
        c = geompp.Polygon3D.make(pts).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 5.0)

    def test_centroid_self_intersecting_outer_matches_simplify_weighted_average(self):
        # On a tilted (non-axis-aligned) plane so a naive 2D-projected centroid would be wrong too
        # (foreshortening) — cross-validated against simplify() + per-piece area()/centroid().
        def tilt(x, y):
            return geompp.Point3D(x, y, 0.3 * x + 0.2 * y)

        p = geompp.Polygon3D.make([tilt(0, 0), tilt(4, 0), tilt(1, 3), tilt(3, 3)])
        assert not p.is_simple()

        total_area, wx, wy, wz = 0.0, 0.0, 0.0, 0.0
        for piece in p.simplify():
            a = piece.area()
            c = piece.centroid()
            total_area += a
            wx += a * c.x
            wy += a * c.y
            wz += a * c.z

        actual = p.centroid()
        assert approx(actual.x, wx / total_area)
        assert approx(actual.y, wy / total_area)
        assert approx(actual.z, wz / total_area)

    def test_centroid_with_centered_hole(self):
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        c = geompp.Polygon3D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_centroid_with_off_center_hole(self):
        # 6×1 outer, 2×1 CW hole on the right → centroid shifts left to x=2
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(6, 0, 0),
            geompp.Point3D(6, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        hole = [
            geompp.Point3D(4, 0, 0), geompp.Point3D(4, 1, 0),
            geompp.Point3D(6, 1, 0), geompp.Point3D(6, 0, 0),
        ]
        c = geompp.Polygon3D.make(outer, [hole]).centroid()
        assert approx(c.x, 2.0) and approx(c.y, 0.5) and approx(c.z, 0.0)

    def test_to_segments(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        segs = p.to_segments()
        assert len(segs) == 4  # N vertices → N segments (closed)
        assert all(isinstance(s, geompp.LineSegment3D) for s in segs)
        assert all(approx(s.length(), 1.0) for s in segs)
        first = segs[0]
        assert first.first.almost_equals(geompp.Point3D(0, 0, 0))
        assert first.last.almost_equals(geompp.Point3D(1, 0, 0))
        last = segs[3]
        assert last.first.almost_equals(geompp.Point3D(0, 1, 0))
        assert last.last.almost_equals(geompp.Point3D(0, 0, 0))

    def test_almost_equals(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p1 = geompp.Polygon3D.make(pts)
        p2 = geompp.Polygon3D.make(pts)
        assert p1.almost_equals(p2)
        other_pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0),
            geompp.Point3D(2, 2, 0), geompp.Point3D(0, 2, 0),
        ]
        assert not p1.almost_equals(geompp.Polygon3D.make(other_pts))

    def test_to_file_from_file(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        p = geompp.Polygon3D.make(pts)
        with tempfile.NamedTemporaryFile(suffix=".wkt", delete=False) as f:
            path = f.name
        try:
            p.to_file(path)
            assert os.path.exists(path)
            p2 = geompp.Polygon3D.from_file(path)
            assert p.almost_equals(p2)
        finally:
            os.unlink(path)

    def test_distance_to(self):
        # unit square in XY plane
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        sq = geompp.Polygon3D.make(pts)
        assert approx(sq.distance_to(geompp.Point3D(0.5, 0.5, 0)), 0.0)            # interior
        assert approx(sq.distance_to(geompp.Point3D(0.0, 0.5, 0)), 0.0)            # on boundary (edge)
        assert approx(sq.distance_to(geompp.Point3D(1.0, 1.0, 0)), 0.0)            # on boundary (vertex)
        assert approx(sq.distance_to(geompp.Point3D(2.0, 0.5, 0)), 1.0)            # outside in-plane, nearest edge x=1
        assert approx(sq.distance_to(geompp.Point3D(2.0, 2.0, 0)), 2.0 ** 0.5)     # outside, nearest corner

        # off-plane: contains() is false regardless of in-plane position, so this measures true 3D
        # distance to the nearest edge/vertex -- no "inside" shortcut.
        assert approx(sq.distance_to(geompp.Point3D(0.5, 0.5, 1.0)), 1.25 ** 0.5)  # above center
        assert approx(sq.distance_to(geompp.Point3D(2.0, 0.5, 1.0)), 2.0 ** 0.5)   # above + outside in-plane

        # polygon with hole in XY plane -- a point in the hole must measure to the HOLE's boundary.
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert approx(poly.distance_to(geompp.Point3D(0.5, 0.5, 0)), 0.0)  # in the solid region
        assert approx(poly.distance_to(geompp.Point3D(2.0, 2.0, 0)), 1.0)  # hole center
        assert approx(poly.distance_to(geompp.Point3D(1.0, 2.0, 0)), 0.0)  # on the hole boundary

    def test_contains(self):
        # unit square in XY plane
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        sq = geompp.Polygon3D.make(pts)
        assert sq.contains(geompp.Point3D(0.5, 0.5, 0))    # center
        assert sq.contains(geompp.Point3D(0.1, 0.1, 0))    # near corner
        assert sq.contains(geompp.Point3D(0.9, 0.9, 0))    # near corner
        assert not sq.contains(geompp.Point3D(-0.1, 0.5, 0))
        assert not sq.contains(geompp.Point3D(1.1, 0.5, 0))
        assert not sq.contains(geompp.Point3D(0.5, 0.5, 1))   # off-plane
        assert not sq.contains(geompp.Point3D(0.5, 0.5, -1))  # off-plane

        # polygon with hole in XY plane
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.contains(geompp.Point3D(0.5, 0.5, 0))   # inside outer, outside hole
        assert poly.contains(geompp.Point3D(3.5, 3.5, 0))   # inside outer, outside hole
        assert not poly.contains(geompp.Point3D(2, 2, 0))   # inside hole
        assert not poly.contains(geompp.Point3D(-1, 2, 0))  # outside outer
        assert not poly.contains(geompp.Point3D(2, 2, 1))   # above plane

        # boundary — vertices and edge midpoints are included
        assert sq.contains(geompp.Point3D(0,   0,   0))   # vertex
        assert sq.contains(geompp.Point3D(1,   0,   0))   # vertex
        assert sq.contains(geompp.Point3D(0.5, 0,   0))   # bottom edge midpoint
        assert sq.contains(geompp.Point3D(1,   0.5, 0))   # right edge midpoint
        assert sq.contains(geompp.Point3D(0.5, 1,   0))   # top edge midpoint
        assert sq.contains(geompp.Point3D(0,   0.5, 0))   # left edge midpoint
        assert poly.contains(geompp.Point3D(2,   0,   0))  # outer bottom edge
        assert poly.contains(geompp.Point3D(2,   1,   0))  # hole bottom edge

        # square in YZ plane (x=0)
        yz_sq = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        assert yz_sq.contains(geompp.Point3D(0, 0.5, 0.5))
        assert not yz_sq.contains(geompp.Point3D(1, 0.5, 0.5))  # off-plane

    def test_is_on_perimeter(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ]
        sq = geompp.Polygon3D.make(pts)
        # vertices
        assert sq.is_on_perimeter(geompp.Point3D(0,   0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   1,   0))
        assert sq.is_on_perimeter(geompp.Point3D(0,   1,   0))
        # edge midpoints
        assert sq.is_on_perimeter(geompp.Point3D(0.5, 0,   0))
        assert sq.is_on_perimeter(geompp.Point3D(1,   0.5, 0))
        assert sq.is_on_perimeter(geompp.Point3D(0.5, 1,   0))
        assert sq.is_on_perimeter(geompp.Point3D(0,   0.5, 0))
        # interior and outside must be False
        assert not sq.is_on_perimeter(geompp.Point3D(0.5, 0.5, 0))
        assert not sq.is_on_perimeter(geompp.Point3D(-0.1, 0.5, 0))
        assert not sq.is_on_perimeter(geompp.Point3D(0.5,  0.5, 0.01))  # off-plane
        # hole boundary
        outer = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        hole = [
            geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
            geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0),
        ]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.is_on_perimeter(geompp.Point3D(2, 0, 0))  # outer bottom
        assert poly.is_on_perimeter(geompp.Point3D(4, 2, 0))  # outer right
        assert poly.is_on_perimeter(geompp.Point3D(2, 1, 0))  # hole bottom
        assert poly.is_on_perimeter(geompp.Point3D(1, 2, 0))  # hole left
        assert not poly.is_on_perimeter(geompp.Point3D(0.5, 0.5, 0))  # interior
        assert not poly.is_on_perimeter(geompp.Point3D(2,   2,   0))  # inside hole

    def test_from_wkt_whitespace(self):
        sq = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])
        assert sq.almost_equals(geompp.Polygon3D.from_wkt("POLYGON ((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))"))
        assert sq.almost_equals(geompp.Polygon3D.from_wkt("POLYGON ((  0 0 0  ,  1 0 0  ,  1 1 0  ,  0 1 0  ,  0 0 0  ))"))

class TestPolygon3DIntersection:
    def _sq(self):
        return geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(1, 0, 0),
            geompp.Point3D(1, 1, 0), geompp.Point3D(0, 1, 0),
        ])

    def test_intersection_line_hits_center(self):
        result = self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_line_misses_outside(self):
        assert self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1))) is None

    def test_intersection_line_parallel_miss(self):
        assert self._sq().intersection(
            geompp.Line3D.make(geompp.Point3D(0, 0, 1), geompp.Point3D(1, 1, 1))) is None

    def test_intersects_line(self):
        sq = self._sq()
        assert sq.intersects(geompp.Line3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert not sq.intersects(geompp.Line3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1)))

    def test_intersection_ray_hits_center(self):
        result = self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, -1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_ray_pointing_away(self):
        assert self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, 1))) is None

    def test_intersection_ray_misses_outside(self):
        assert self._sq().intersection(
            geompp.Ray3D.make(geompp.Point3D(2, 0.5, 1), geompp.Vector3D(0, 0, -1))) is None

    def test_intersects_ray(self):
        sq = self._sq()
        assert sq.intersects(geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, -1)))
        assert not sq.intersects(geompp.Ray3D.make(geompp.Point3D(0.5, 0.5, 1), geompp.Vector3D(0, 0, 1)))

    def test_intersection_segment_pierces(self):
        result = self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0.5, 0.5, 0))

    def test_intersection_segment_too_short(self):
        assert self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, 0.5), geompp.Point3D(0.5, 0.5, 1))) is None

    def test_intersection_segment_misses_outside(self):
        assert self._sq().intersection(
            geompp.LineSegment3D.make(geompp.Point3D(2, 0.5, -1), geompp.Point3D(2, 0.5, 1))) is None

    def test_intersects_segment(self):
        sq = self._sq()
        assert sq.intersects(geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, -1), geompp.Point3D(0.5, 0.5, 1)))
        assert not sq.intersects(geompp.LineSegment3D.make(geompp.Point3D(0.5, 0.5, 0.5), geompp.Point3D(0.5, 0.5, 1)))

    def test_intersection_non_xy_plane(self):
        # YZ square at x=0; normal = +X. Line along X through center (0, 0.5, 0.5).
        yz = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        result = yz.intersection(geompp.Line3D.make(geompp.Point3D(-1, 0.5, 0.5), geompp.Point3D(1, 0.5, 0.5)))
        assert result is not None
        assert result.almost_equals(geompp.Point3D(0, 0.5, 0.5))

    # ── Polygon3D boolean ops (Polygon3D operand) ───────────────────────────

    def _overlapping(self):
        return geompp.Polygon3D.make([
            geompp.Point3D(0.5, 0.5, 0), geompp.Point3D(1.5, 0.5, 0),
            geompp.Point3D(1.5, 1.5, 0), geompp.Point3D(0.5, 1.5, 0),
        ])

    def test_intersects_polygon_coplanar(self):
        assert self._sq().intersects(self._overlapping())

    def test_union_coplanar(self):
        result = self._sq().union(self._overlapping())
        assert len(result) == 1
        assert abs(result[0].area() - 1.75) < 1e-6

    def test_difference_coplanar(self):
        result = self._sq().difference(self._overlapping())
        assert len(result) == 1
        assert abs(result[0].area() - 0.75) < 1e-6

    def test_xor_coplanar(self):
        result = self._sq().xor(self._overlapping())
        assert len(result) == 1
        assert result[0].has_holes()

    def test_union_not_coplanar_raises(self):
        wall = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 1, 0),
            geompp.Point3D(0, 1, 1), geompp.Point3D(0, 0, 1),
        ])
        with pytest.raises(Exception):
            self._sq().union(wall)

    def test_intersection_polygon_coplanar_returns_polygons(self):
        result = self._sq().intersection(self._overlapping())
        assert result is not None
        assert isinstance(result, list)
        assert isinstance(result[0], geompp.Polygon3D)
        assert abs(result[0].area() - 0.25) < 1e-6

    def test_intersection_polygon_planes_crossing_returns_segments(self):
        a = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ])
        b = geompp.Polygon3D.make([
            geompp.Point3D(1, 2, 3), geompp.Point3D(3, 2, 3),
            geompp.Point3D(3, 2, -1), geompp.Point3D(1, 2, -1),
        ])
        result = a.intersection(b)
        assert result is not None
        assert isinstance(result, list)
        assert isinstance(result[0], geompp.LineSegment3D)
        assert len(result) == 1

    def test_intersection_polygon_parallel_distinct_returns_none(self):
        a = self._sq()
        b = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 5), geompp.Point3D(1, 0, 5),
            geompp.Point3D(1, 1, 5), geompp.Point3D(0, 1, 5),
        ])
        assert a.intersection(b) is None

class TestCentroid3D:
    def test_xy_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ]
        c = geompp.centroid(pts)
        assert isinstance(c, geompp.Point3D)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 0.0)

    def test_elevated_xy_square(self):
        pts = [
            geompp.Point3D(0, 0, 7), geompp.Point3D(4, 0, 7),
            geompp.Point3D(4, 4, 7), geompp.Point3D(0, 4, 7),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 7.0)

    def test_yz_square(self):
        pts = [
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(0, 4, 4), geompp.Point3D(0, 0, 4),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 0.0) and approx(c.y, 2.0) and approx(c.z, 2.0)

    def test_off_origin_yz_square(self):
        pts = [
            geompp.Point3D(5, 0, 0), geompp.Point3D(5, 4, 0),
            geompp.Point3D(5, 4, 4), geompp.Point3D(5, 0, 4),
        ]
        c = geompp.centroid(pts)
        assert approx(c.x, 5.0) and approx(c.y, 2.0) and approx(c.z, 2.0)

    def test_with_explicit_plane(self):
        pts = [
            geompp.Point3D(0, 0, 3), geompp.Point3D(4, 0, 3),
            geompp.Point3D(4, 4, 3), geompp.Point3D(0, 4, 3),
        ]
        pl = geompp.Plane.from_origin_and_normal(geompp.Point3D(0, 0, 3), geompp.Vector3D(0, 0, 1))
        c = geompp.centroid(pts, pl)
        assert approx(c.x, 2.0) and approx(c.y, 2.0) and approx(c.z, 3.0)

    def test_triangle_returns_average(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0), geompp.Point3D(0, 3, 0)]
        c = geompp.centroid(pts)
        assert approx(c.x, 1.0) and approx(c.y, 1.0) and approx(c.z, 0.0)

class TestPolygon3DConvexHull:
    def _star3d(self):
        return geompp.Polygon3D.make([
            geompp.Point3D(-1,  2, 0), geompp.Point3D(-3,  1, 0),
            geompp.Point3D(-1, -1, 0), geompp.Point3D(-2, -4, 0),
            geompp.Point3D( 0, -1, 0), geompp.Point3D( 3, -3, 0),
            geompp.Point3D( 2, -1, 0), geompp.Point3D( 4,  2, 0),
            geompp.Point3D( 2,  1, 0), geompp.Point3D( 0,  5, 0),
        ])

    def test_convex_hull_star_is_pentagon(self):
        hull = self._star3d().convex_hull()
        assert hull.size() == 5

    def test_convex_hull_convex_polygon_unchanged(self):
        square = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ])
        hull = square.convex_hull()
        assert hull.size() == 4

    def test_perimeter_round_trip(self):
        pts = [geompp.Point3D(0, 0, 0), geompp.Point3D(3, 0, 0),
               geompp.Point3D(3, 3, 0), geompp.Point3D(0, 3, 0)]
        poly = geompp.Polygon3D.make(pts)
        back = poly.perimeter()
        assert len(back) == 4
        for orig, restored in zip(pts, back):
            assert (approx(orig.x, restored.x) and approx(orig.y, restored.y)
                    and approx(orig.z, restored.z))

class TestPolygon3DHoles:
    def test_no_holes(self):
        poly = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0),
        ])
        assert poly.has_holes() is False
        assert poly.holes() == []

    def test_with_hole(self):
        outer = [geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
                 geompp.Point3D(4, 4, 0), geompp.Point3D(0, 4, 0)]
        hole = [geompp.Point3D(1, 1, 0), geompp.Point3D(1, 3, 0),
                geompp.Point3D(3, 3, 0), geompp.Point3D(3, 1, 0)]
        poly = geompp.Polygon3D.make(outer, [hole])
        assert poly.has_holes() is True
        holes = poly.holes()
        assert len(holes) == 1
        assert len(holes[0]) == 4
        for orig, restored in zip(hole, holes[0]):
            assert (approx(orig.x, restored.x) and approx(orig.y, restored.y)
                    and approx(orig.z, restored.z))

class TestPolygon3DIsConvex:
    def test_square_xy_plane_is_convex(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(1,0,0),
            geompp.Point3D(1,1,0), geompp.Point3D(0,1,0)])
        assert p.is_convex()

    def test_concave_not_convex(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0,0,0), geompp.Point3D(4,0,0),
            geompp.Point3D(4,4,0), geompp.Point3D(2,2,0), geompp.Point3D(0,4,0)])
        assert not p.is_convex()

    def test_with_hole_not_convex(self):
        outer = [geompp.Point3D(0,0,0), geompp.Point3D(4,0,0),
                 geompp.Point3D(4,4,0), geompp.Point3D(0,4,0)]
        # holes must be CW (reversed winding)
        hole  = [geompp.Point3D(1,1,0), geompp.Point3D(1,2,0),
                 geompp.Point3D(2,2,0), geompp.Point3D(2,1,0)]
        p = geompp.Polygon3D.make(outer, [hole])
        assert not p.is_convex()

class TestPolygon3DSimplify:
    def _bowtie_xy(self):
        # Bowtie in XY plane (z=0): dominant axis = Z.
        return geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])

    def test_already_simple_returns_self(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(2, 0, 0),
            geompp.Point3D(2, 2, 0), geompp.Point3D(0, 2, 0)])
        result = p.simplify()
        assert len(result) == 1
        assert result[0].almost_equals(p)

    def test_bowtie_xy_yields_two_polygons(self):
        result = self._bowtie_xy().simplify()
        assert len(result) == 2

    def test_bowtie_xy_results_are_simple(self):
        for poly in self._bowtie_xy().simplify():
            assert poly.is_simple()

    def test_bowtie_yz_plane_yields_two_polygons(self):
        # Bowtie in YZ plane (x=0): dominant axis = X.
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(0, 1, 3), geompp.Point3D(0, 3, 3)])
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()

    def test_bowtie_xz_plane_yields_two_polygons(self):
        # Bowtie in XZ plane (y=0): dominant axis = Y, projection flips chirality.
        p = geompp.Polygon3D.make([
            geompp.Point3D(3, 0, 3), geompp.Point3D(1, 0, 3),
            geompp.Point3D(4, 0, 0), geompp.Point3D(0, 0, 0)])
        result = p.simplify()
        assert len(result) == 2
        for poly in result:
            assert poly.is_simple()

    def test_bowtie_areas_sum(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])
        result = p.simplify()
        total = sum(poly.area() for poly in result)
        assert abs(total - 5.0) < 0.01

    def test_results_are_coplanar(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(1, 3, 0), geompp.Point3D(3, 3, 0)])
        for poly in p.simplify():
            for i in range(poly.size()):
                assert abs(poly[i].z) < 1e-9

class TestPolygon3DTriangulate:
    def test_flat_convex_quad_two_triangles_full_area(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 2, 0), geompp.Point3D(0, 2, 0)])
        triangles = p.triangulate()
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), p.area())

    def test_tilted_plane_quad_two_triangles_full_area(self):
        # Non-XY plane (X-dominant-axis normal) — proves the polygon's own plane normal is used.
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(0, 4, 0),
            geompp.Point3D(0, 4, 2), geompp.Point3D(0, 0, 2)])
        triangles = p.triangulate()
        assert len(triangles) == 2
        assert approx(sum(t.area() for t in triangles), p.area())

    def test_monotone_polygon_strategy_raises(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 2, 0), geompp.Point3D(0, 2, 0)])
        with pytest.raises(RuntimeError):
            p.triangulate(geompp.TriangulationStrategy.MonotonePolygon)

    def test_delaunay_strategy_raises(self):
        p = geompp.Polygon3D.make([
            geompp.Point3D(0, 0, 0), geompp.Point3D(4, 0, 0),
            geompp.Point3D(4, 2, 0), geompp.Point3D(0, 2, 0)])
        with pytest.raises(RuntimeError):
            p.triangulate(geompp.TriangulationStrategy.Delaunay)
