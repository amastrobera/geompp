# `SweepLine2D` (C# / .NET)

## `Get`

`SweepLineElement2D^ Get(size_t seg_id)`


**Parameters**

- `seg_id` (`size_t`) — index of the SegmentList [0, N-1]

**Returns** — (Segment, Above, Below) = pointers to the segment of index seg_id in the SegmentList , the above and below segment pointers. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)

## `Add`

`SweepLineElement2D^ Add(size_t seg_id)`

Adds a segment in the tree, sorted.

**Parameters**

- `seg_id` (`size_t`) — index of the SegmentList [0, N-1]

**Returns** — (Segment = ptr to segment just inserted, Above / Below = ptr to above or below segments in the tree). Above and Below may be either or both std::nullopt. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)

## `Remove`

`SweepLineElement2D^ Remove(size_t seg_id)`

Remove the seg_id from the tree, which remains sorted.

**Parameters**

- `seg_id` (`size_t`) — index of the SegmentList [0, N-1]

**Returns** — (Segment = std::nullopt, Above = ptr to above segment before deletion, Below = ptr to below segment before deletion). Either or both Above and Below can be std::nullopt. If seg_id not found (std::nullopt, std::nullopt, std::nullopt)

## `ReverseRun`

`ReverseRunResult2D^ ReverseRun(std::vector< std::size_t > seg_ids)`

Reverses, in place, the contiguous run of currently-active segments named by seg_ids — the O(1)-per-swap replacement for the old Remove/SetX(x+eps)/Add cycle used to react to a confirmed crossing.

A 2-element seg_ids is the common pairwise-crossing case (equivalent to swapping two adjacent elements); 3+ elements handles several segments meeting at exactly one point in a single reversal instead of a cascade of pairwise swaps that could desync mid-cascade (reversing [A,B,C] to [C,B,A] realizes all three pairwise crossings (A,B), (B,C), and (A,C) at once — the pairs (A,B) and (B,C) were already tested when they first became adjacent, so no pair inside the run needs retesting, only the two new outer pairs this call reports via AboveRun/BelowRun). Locates the run with exactly ONE comparator-driven lookup (for an arbitrary anchor id from seg_ids ), then expands outward by direct vector indexing (no further comparator calls) to confirm the rest of seg_ids occupy the immediately-adjacent slots. Every id in seg_ids is already active (unlike Add() , which places a segment the comparator has never seen before), so the anchor lookup's existing exact-id fallback (see Get() ) makes this safe even if SWEEP_X has gone stale relative to the true post-crossing arrangement: the comparator can only fail to find the anchor (triggering the linear-scan fallback), never report a wrong id as a match, since ids are unique in ACTIVE_SEGMENTS.

**Parameters**

- `seg_ids` (`std::vector< std::size_t >`) — the ids expected to form one contiguous run, in any order; must have size() >= 2.

## `SetX`

`void SetX(double val)`

adjusts the current sweep x coordinate to a desired value, and lets the algorithms continue

**Parameters**

- `val` (`double`) — usually the X of the next_event in the EventQueue.Pop() or the current X + EPSILON

## `GetX`

`double GetX()`
