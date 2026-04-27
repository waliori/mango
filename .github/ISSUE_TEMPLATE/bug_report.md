---
name: Bug report
about: Something in noir isn't working correctly
title: ""
labels: "A: bug"
assignees: ""
---

## Info

<!--Paste noir version from running "noir -v"-->

noir version:
wlroots version:

## Crash track
1. Build noir with the asan flag:
```bash
meson build -Dprefix=/usr -Dasan=true
```
2. Run noir in tty:
```bash
export ASAN_OPTIONS="detect_leaks=1:halt_on_error=0:log_path=/home/xxx/asan.log"
noir
```
3. After noir crashes, paste the log file `/home/xxx/asan.log` here.

## Description

<!--
Only report bugs that can be reproduced on the main line
-->

