# `ScopedPrecision` (Python)

Scoped, exception-safe override of the thread-local DECIMAL_PRECISION.

DECIMAL_PRECISION is a thread-global comparison tolerance every AlmostEquals -based operation reads. Changing it for a single call (e.g. to loosen a coplanarity/overlap check for one noisy dataset) otherwise means "mutate the global, remember to restore it" — brittle, and leaks the change to unrelated comparisons if an exception unwinds past the restore. This RAII guard saves the current value on construction and restores it on destruction, so a loosened precision is confined to a block: { geompp::ScopedPrecision sp(1); // epsilon = 0.1 within this scope only auto merged = a.Union(b); // coplanarity/classification use the looser tolerance } // previous precision restored here, even if Union threw Only affects the calling thread (matching DECIMAL_PRECISION 's thread-local storage). Nesting is fine — each guard restores the value that was current when it was constructed.

## Fields

- `saved` (`int`)
