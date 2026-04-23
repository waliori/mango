# Vendored scenefx

Source: https://github.com/wlrfx/scenefx (tag `0.4.1`)
Commit: 0f06997 (tip of `0.4.1` tag)
Vendored: 2026-04-23

This is a vendored copy of scenefx, consumed via meson subproject from
mango's top-level `meson.build`. Upstream at https://github.com/wlrfx/scenefx.

## Why vendored

Upstream scenefx 0.4.1 ships with scenefx#132 — on rotated outputs, layer
blur fails to stencil-cut to the opaque panel alpha, instead drawing as a
misplaced rectangle. Mango previously worked around this by passing
`backdrop_blur_ignore_transparent=false` on rotated outputs (commit
81b2787 in mango).

PR #154 on scenefx main (merged 2025-11-20, commit 5ad7e35) fixes the
root cause: the stencil pass must sample the buffer's alpha using the
composition of the buffer's transform and the output's transform, not
hardcoded `WL_OUTPUT_TRANSFORM_NORMAL`. PR #154 targets the standalone
`WLR_SCENE_NODE_BLUR` API which only exists on main (wlroots 0.20).
scenefx 0.4.1 uses a buffer-property model — same bug, same fix shape,
applied at the 0.4.1 call site.

The fix is carried as a single mango-local patch on top of this
vendored tree. See `git log` in mango for the scenefx-specific commit.

## To sync from upstream

If upstream ever cuts a 0.4.x release that includes the fix, replace this
tree with the upstream tarball and drop the local patch:

    cd /home/waliori/mango
    rm -rf subprojects/scenefx
    git -C <upstream-clone> archive --format=tar <ref> \
        | tar -xf - -C subprojects/scenefx
    # Re-review any mango-local patches before re-applying.
